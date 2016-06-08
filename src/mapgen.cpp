#include "mapgen.hpp"

#include <algorithm>
#include <random>

#include "map.hpp"
#include "ter.hpp"

// Enable rendering and during map generation for evaluation/demo purposes.
// Comment out to disable, uncomment to enable
//#define DEMO_MODE 1

#ifdef DEMO_MODE
const unsigned int demo_step_ms = 200;
#endif // DEMO_MODE

// Specific demo options (only applicable if DEMO_MODE above is enabled)
//#define DEMO_AREA_BUILD 1
//#define DEMO_TRIM_CORRIDORS 1
//#define DEMO_CONNECT_AREAS_EXTRA 1

namespace mapgen
{

namespace
{

const R map_r(0, 0, map_w - 1, map_h - 1);

std::vector< std::unique_ptr<MapArea> > built_areas;

// Array to track which cells are corridors, to be considered when trimming
// dead-end corridors.
bool corridor_map[map_w][map_h];

void valid_area_sources(const MapArea& area, std::vector<AreaSource>& out)
{
    out.clear();

    // In the "worst" case, every cell inside the wall will be added,
    // reserve space for this
    out.reserve(area.r_.w() * 2 +   // Top and bottom walls
                area.r_.h() * 2);   // Left and right walls

    const R& r = area.r_;

    for (int x = r.p0.x; x <= r.p1.x; ++x)
    {
        for (int y = r.p0.y; y <= r.p1.y; ++y)
        {
            if (map::ter[x][y]->id() != TerId::floor)
            {
                // Cell is inside this area, but it's not floor - not a valid
                // starting position to expand from
                continue;
            }

            const P p(x, y);

            bool pos_ok = true;

            // Do not use the tried position if is adjacent to floor which is
            // outside the map area
            for (const P& d : dir_utils::dir_list)
            {
                const P check_p(p + d);

                if (
                    !area.r_.is_p_inside(check_p) &&
                    map::ter[check_p.x][check_p.y]->id() != TerId::rock_wall)
                {
                    // Foreign floor encountered, this is not a good position!
                    pos_ok = false;
                    break;
                }
            }

            if (!pos_ok)
            {
                continue;
            }

            // Alright, this is a valid starting position to expand from!
            // Add relevant directions to the source bucket.

            if (x == r.p0.x)
            {
                out.push_back(AreaSource(p, Dir::left));
            }

            if (x == r.p1.x)
            {
                out.push_back(AreaSource(p, Dir::right));
            }

            if (y == r.p0.y)
            {
                out.push_back(AreaSource(p, Dir::up));
            }

            if (y == r.p1.y)
            {
                out.push_back(AreaSource(p, Dir::down));
            }
        }
    }
}

void build_areas()
{
    TRACE_FUNC_BEGIN;

    // TODO: Function for getting a random area type

    built_areas.clear();

    // Build first room
    {
        std::unique_ptr<Room> room(new Room);

        const P center(map_mid_x, map_mid_y);

        const AreaSource source(center, Dir::right);

        bool build_ok = room->build(source);

        // The first one should really work
        ASSERT(build_ok);

        built_areas.emplace_back(std::move(room));
    }

    for (int i = 0; i < 5000; ++i)
    {
        const size_t old_idx = rnd::idx(built_areas);

        const MapArea* const old = built_areas[old_idx].get();

        std::vector<AreaSource> source_bucket;

        valid_area_sources(*old, source_bucket);

        if (!source_bucket.empty())
        {
            const size_t start_idx = rnd::idx(source_bucket);

            const AreaSource& source = source_bucket[start_idx];

#if defined DEMO_MODE && defined DEMO_AREA_BUILD
            render::update_vp(source.p);
            render::draw_map_state();
            render::draw_char_on_map(source.p,
                                     source.dir == Dir::right   ? '>' :
                                     source.dir == Dir::left    ? '<' :
                                     source.dir == Dir::down    ? 'V' : '^',
                                     clr_yellow);
            io::update_scr();
            io::sleep(demo_step_ms * 2);
#endif // DEMO_MODE and DEMO_AREA_BUILD

            std::unique_ptr<MapArea> area(nullptr);

            // TODO: See TODO at top of function
            if (rnd::percent(50))
            {
                area.reset(new Room);
            }
            else
            {
                area.reset(new Corridor);
            }

            if (area->build(source))
            {
                // Great! Let's keep the area.
                built_areas.emplace_back(std::move(area));

                // Fill in the gap
                const P gap_p(source.p + dir_utils::offset(source.dir));

                map::ter[gap_p.x][gap_p.y] = ter::mk(TerId::floor, gap_p);

                // Treat the gap as corridor (so it will be considered for
                // trimming later)
                corridor_map[gap_p.x][gap_p.y] = true;
            }
        }
    }

    TRACE_FUNC_END;
}

void trim_corridors()
{
    TRACE_FUNC_BEGIN;

    for (int x = 1; x < map_w - 1; ++x)
    {
        for (int y = 1; y < map_h - 1; ++y)
        {
            if (corridor_map[x][y])
            {
                int nr_cardinal_floor = 0;

                const P p(x, y);

                for (const P& d : dir_utils::cardinal_list)
                {
                    const P check_p(p + d);

                    if (map::ter[check_p.x][check_p.y]->id() == TerId::floor)
                    {
                        ++nr_cardinal_floor;
                    }
                }

                if (nr_cardinal_floor == 1)
                {
                    // Looks like a dead end, fill it with rock wall
                    map::ter[x][y] = ter::mk(TerId::rock_wall, p);

#if defined DEMO_MODE && defined DEMO_TRIM_CORRIDORS
                    render::update_vp(p);
                    render::draw_char_on_map(p,
                                             'X',
                                             clr_yellow);
                    io::update_scr();
                    io::sleep(demo_step_ms / 2);
                    render::draw_map_state();
                    io::sleep(demo_step_ms / 2);
#endif // DEMO_MODE and DEMO_TRIM_CORRIDORS

                    corridor_map[x][y] = false;

                    // This may have caused another cell to be a dead-end, so
                    // we start over from the beginning. (Yes this is probably
                    // grossly ineffective, But it seems fast enough...)
                    x = 0;
                    y = 0;
                }
            }
        }
    }

    TRACE_FUNC_END;
}

void connect_areas_extra()
{
    TRACE_FUNC_BEGIN;

    // Used by floodfill
    int flood[map_w][map_h];
    bool blocked[map_w][map_h];

    for (int x = 0; x < map_w; ++x)
    {
        for (int y = 0; y < map_h; ++y)
        {
            blocked[x][y] = map::ter[x][y]->blocks();
        }
    }

    std::vector<AreaSource> source_bucket;

    for (auto& area : built_areas)
    {
        valid_area_sources(*area.get(), source_bucket);

        if (source_bucket.empty())
        {
            continue;
        }

        for (const AreaSource& source : source_bucket)
        {
            const P d(dir_utils::offset(source.dir));

            // One step out from the source position
            const P wall_p(source.p + d);

            // Sanity check: We ALWAYS expect that one step out from a
            // valid source position, there is a wall cell
            if (map::ter[wall_p.x][wall_p.y]->id() != TerId::rock_wall)
            {
                ASSERT(false);

                // Release mode robustness
                continue;
            }

            // One step further beyond the wall
            const P beyond_wall_p(wall_p + d);

            if (
                map::is_pos_inside_map(beyond_wall_p, false) &&
                map::ter[beyond_wall_p.x][beyond_wall_p.y]->id() == TerId::floor)
            {
                // Aha! There is another area next to our wall!

                // With the current map, how many steps are there to the
                // cell in the other area?

                // Search at most this far:
                const int travel_lmt = 20;

                floodfill::run(source.p,
                               (const bool*)blocked,
                               (int*)flood,
                               map_dims,
                               travel_lmt,
                               beyond_wall_p);

                const int flood_val_beyond = flood[beyond_wall_p.x][beyond_wall_p.y];

                // If it takes many steps to reach the other area
                // even though they are adjacent, this is annoying.
                // Connect the areas!

                if (flood_val_beyond == 0)
                {
                    map::ter[wall_p.x][wall_p.y] = ter::mk(TerId::floor, wall_p);

                    blocked[wall_p.x][wall_p.y] = false;

#if defined DEMO_MODE && defined DEMO_CONNECT_AREAS_EXTRA
                    render::update_vp(wall_p);
                    render::draw_map_state();
                    render::draw_char_on_map(wall_p, 'X', clr_green_lgt);
                    io::update_scr();
                    io::sleep(demo_step_ms * 2);

                    render::draw_map_state();
                    io::sleep(demo_step_ms * 2);
#endif // DEMO_MODE and DEMO_CONNECT_AREAS_EXTRA
                }
            }
        } // Source bucket loop
    } // Map area loop

    TRACE_FUNC_END;
}

void convert_walls()
{
    TRACE_FUNC_BEGIN;

    for (int x = 1; x < map_w - 1; ++x)
    {
        for (int y = 1; y < map_h - 1; ++y)
        {
            if (map::ter[x][y]->id() != TerId::rock_wall)
            {
                continue;
            }

            const P p(x, y);

            bool is_adj_to_floor = false;

            for (const P& d : dir_utils::dir_list)
            {
                const P p_adj(p + d);

                if (map::ter[p_adj.x][p_adj.y]->id() == TerId::floor)
                {
                    is_adj_to_floor = true;
                    break;
                }
            }

            if (is_adj_to_floor)
            {
                map::ter[p.x][p.y] = ter::mk(TerId::wall, p);
            }
        }
    }

    TRACE_FUNC_END;
}

void fill_floor(const R& r)
{
    for (int x = r.p0.x; x <= r.p1.x; ++x)
    {
        for (int y = r.p0.y; y <= r.p1.y; ++y)
        {
            map::ter[x][y] = ter::mk(TerId::floor, P(x, y));
        }
    }

#if defined DEMO_MODE && defined DEMO_AREA_BUILD
    render::update_vp(r.center());
    render::draw_map_state();
    io::sleep(demo_step_ms);
#endif // DEMO_MODE and DEMO_AREA_BUILD
}

bool allow_build_at(const R& r)
{
    if (!is_area_inside(r, map_r, false))
    {
        return false;
    }

#if defined DEMO_MODE && defined DEMO_AREA_BUILD
    render::update_vp(r.center());
    render::draw_map_state();
    io::sleep(demo_step_ms);
#endif // DEMO_MODE and DEMO_AREA_BUILD

    const R check_r(r.p0 - 1, r.p1 + 1);

    for (int x = check_r.p0.x; x <= check_r.p1.x; ++x)
    {
        for (int y = check_r.p0.y; y <= check_r.p1.y; ++y)
        {
            if (map::ter[x][y]->id() != TerId::rock_wall)
            {
#if defined DEMO_MODE && defined DEMO_AREA_BUILD
                render::draw_char_on_map(P(x, y), 'X', clr_red_lgt);
                io::update_scr();
                io::sleep(demo_step_ms * 2);
#endif // DEMO_MODE and DEMO_AREA_BUILD
                return false;
            }
#if defined DEMO_MODE && defined DEMO_AREA_BUILD
            else // Free cell
            {
                render::draw_char_on_map(P(x, y), 'X', clr_green_lgt);
            }
#endif // DEMO_MODE and DEMO_AREA_BUILD
        }
    }

#if defined DEMO_MODE && defined DEMO_AREA_BUILD
    io::update_scr();
    io::sleep(demo_step_ms);
#endif // DEMO_MODE and DEMO_AREA_BUILD
    return true;
}

void populate_mon()
{
    BoolMap blocked;

    for (int x = 0; x < map_w; ++x)
    {
        for (int y = 0; y < map_h; ++y)
        {
            blocked.data[x][y] = map::ter[x][y]->blocks();
        }
    }

    for (const auto& mon : map::monsters)
    {
        const P& p = mon->p();

        blocked.data[p.x][p.y] = true;
    }

    std::vector<P> p_bucket;

    blocked.cells_with_value(false, p_bucket);

    const size_t max_nr = 10;

    for (size_t i = 0; i < max_nr; ++i)
    {
        const size_t idx = rnd::idx(p_bucket);

        const P& p = p_bucket[idx];

        map::monsters.emplace_back(mon::mk(MonId::mutant_humanoid, p));

        p_bucket.erase(begin(p_bucket) + idx);
    }
}

} // namespace

bool Room::build(const AreaSource& source)
{
    // TODO: Normal distribution
    const P dims(rnd::range(3, 18),
                 rnd::range(3, 12));

    const P p0_orig(source.p - (dims / 2));
    const P p1_orig(p0_orig + dims - 1);

    r_ = R(p0_orig, p1_orig);

    const P d(dir_utils::offset(source.dir));

    while (r_.is_p_inside(source.p))
    {
        r_ += d;
    }

    r_ += d;

    if (allow_build_at(r_))
    {
        fill_floor(r_);

        return true;
    }

    return false;
}

bool Corridor::build(const AreaSource& source)
{
    // TODO: Normal distribution
    const Axis axis = (source.dir == Dir::right || source.dir == Dir::left) ?
                      Axis::hor : Axis::ver;

    const int len = rnd::range(8, 28);

    const P dims(axis == Axis::hor ? len    : 1,
                 axis == Axis::hor ? 1      : len);

    const P p0_orig(source.p - (dims / 2));
    const P p1_orig(p0_orig + dims - 1);

    r_ = R(p0_orig, p1_orig);

    const P d(dir_utils::offset(source.dir));

    while (r_.is_p_inside(source.p))
    {
        r_ += d;
    }

    r_ += d;

    if (allow_build_at(r_))
    {
        // Track which cells are corridor (used later when trimming dead-ends)
        for (int x = r_.p0.x; x <= r_.p1.x; ++x)
        {
            for (int y = r_.p0.y; y <= r_.p1.y; ++y)
            {
                corridor_map[x][y] = true;
            }
        }

        fill_floor(r_);

        return true;
    }

    return false;
}

void run()
{
    std::fill_n(*corridor_map, nr_map_cells, false);

    build_areas();

    trim_corridors();

    connect_areas_extra();

    convert_walls();

    built_areas.clear();

    populate_mon();
}

} // namespace

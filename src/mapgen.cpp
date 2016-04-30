#include "mapgen.hpp"

#include "map.hpp"
#include "ter.hpp"

// Enable rendering and during map generation for evaluation/demo purposes.
// Comment out to disable, uncomment to enable
//#define DEMO_MODE 1

#ifdef DEMO_MODE
const unsigned int demo_step_ms = 200;
#endif // DEMO_MODE

// Specific demo options (only applicable if DEMO_MODE above is enabled)
//#define DEMO_AREA_BUILD     1
#define DEMO_CORRIDOR_TRIM  1

namespace mapgen
{

namespace
{

const R map_r(0, 0, map_w - 1, map_h - 1);

// Array to track which cells are corridors, to be considered when trimming
// dead-end corridors.
bool corridor_map[map_w][map_h];

void build_areas()
{
    TRACE_FUNC_BEGIN;

    // TODO: Function for getting a random area type

    // Successfully built areas
    std::vector< std::unique_ptr<MapArea> > built_areas;

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

    for (int i = 0; i < 20000; ++i)
    {
        const size_t old_idx = rnd::idx(built_areas);

        const MapArea* const old = built_areas[old_idx].get();

        const R old_r = old->r_;

        std::vector<AreaSource> source_bucket;

        for (int x = old_r.p0.x; x <= old_r.p1.x; ++x)
        {
            for (int y = old_r.p0.y; y <= old_r.p1.y; ++y)
            {
                if (map::ter[x][y]->id() == TerId::floor)
                {
                    const P p(x, y);

                    if (x == old_r.p0.x)
                    {
                        source_bucket.push_back(AreaSource(p, Dir::left));
                    }

                    if (x == old_r.p1.x)
                    {
                        source_bucket.push_back(AreaSource(p, Dir::right));
                    }

                    if (y == old_r.p0.y)
                    {
                        source_bucket.push_back(AreaSource(p, Dir::up));
                    }

                    if (y == old_r.p1.y)
                    {
                        source_bucket.push_back(AreaSource(p, Dir::down));
                    }
                }
            }
        }

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
            if (rnd::percent(75))
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

#if defined DEMO_MODE && defined DEMO_CORRIDOR_TRIM
                    render::update_vp(p);
                    render::draw_char_on_map(p,
                                             'X',
                                             clr_yellow);
                    io::update_scr();
                    io::sleep(demo_step_ms / 2);
                    render::draw_map_state();
                    io::sleep(demo_step_ms / 2);
#endif // DEMO_MODE and DEMO_CORRIDOR_TRIM

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

void convert_to_constr_walls()
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

} // namespace

void run()
{
    std::fill_n(*corridor_map, nr_map_cells, false);

    build_areas();

    trim_corridors();

    convert_to_constr_walls();
}

bool Room::build(const AreaSource& source)
{
    // TODO: Normal distribution
    const P dims(rnd::range(3, 21),
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

    const int len = rnd::range(5, 14);

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

} // namespace

#include "mapgen.hpp"

#include "map.hpp"
#include "ter.hpp"

// Enable rendering and during map generation for evaluation/demo purposes.
// Comment out to disable, uncomment to enable
//#define DEMO_MODE 1

#ifdef DEMO_MODE
const unsigned int demo_step_ms = 250;
#endif // DEMO_MODE

namespace mapgen
{

namespace
{

const R map_r(0, 0, map_w - 1, map_h - 1);

void fill_floor(const R& r)
{
    for (int x = r.p0.x; x <= r.p1.x; ++x)
    {
        for (int y = r.p0.y; y <= r.p1.y; ++y)
        {
            map::ter[x][y] = ter::mk(TerId::floor, P(x, y));
        }
    }

#ifdef DEMO_MODE
    render::update_vp(r.center());
    render::draw_map_state();
    io::sleep(demo_step_ms);
#endif // DEMO_MODE
}

bool allow_build_at(const R& r)
{
    if (!is_area_inside(r, map_r, false))
    {
        return false;
    }

#ifdef DEMO_MODE
    render::update_vp(r.center());
    render::draw_map_state();
    io::sleep(demo_step_ms);
#endif // DEMO_MODE

    const R check_r(r.p0 - 1, r.p1 + 1);

    for (int x = check_r.p0.x; x <= check_r.p1.x; ++x)
    {
        for (int y = check_r.p0.y; y <= check_r.p1.y; ++y)
        {
            if (map::ter[x][y]->id() != TerId::rock_wall)
            {
#ifdef DEMO_MODE
                render::draw_char_on_map(P(x, y), 'X', clr_red_lgt);
                io::update_scr();
                io::sleep(demo_step_ms * 2);
#endif // DEMO_MODE
                return false;
            }
#ifdef DEMO_MODE
            else // Free cell
            {
                render::draw_char_on_map(P(x, y), 'X', clr_green_lgt);
            }
#endif // DEMO_MODE
        }
    }

#ifdef DEMO_MODE
    io::update_scr();
    io::sleep(demo_step_ms);
#endif // DEMO_MODE
    return true;
}

} // namespace

void run()
{
    // TODO: Function for getting a random area type

    std::vector<Dir> cardinals =
    {
        Dir::right,
        Dir::left,
        Dir::down,
        Dir::up
    };

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

#ifdef DEMO_MODE
            render::update_vp(source.p);
            render::draw_map_state();
            render::draw_char_on_map(source.p,
                                     source.dir == Dir::right   ? '>' :
                                     source.dir == Dir::left    ? '<' :
                                     source.dir == Dir::down    ? 'V' : '^',
                                     clr_yellow);
            io::update_scr();
            io::sleep(demo_step_ms * 2);
#endif // DEMO_MODE

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
            }
        }
    }

        TRACE << "Changing rock walls around floor to constructed walls" << std::endl;
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
    }

bool Room::build(const AreaSource& source)
{
    // TODO: Normal distribution
    const P dims(rnd::range(3, 19),
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
        fill_floor(r_);

        return true;
    }

    return false;
}

} // namespace

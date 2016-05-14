#include "fov.hpp"

#include "bresenham.hpp"

namespace fov
{

namespace
{

FovData fov_data_buffer[map_w][map_h];

// Make the FOV a bit more permissive by always marking walls adjacent to
// seen non-blocking cells as visible
void fov_wall_hack(const R& area)
{
    // NOTE: The given area will occasionally include the edge of the map,
    // and we check all cells adjacent to each cell in the area. This is OK
    // however, since we only check cells adjacent to non-blocking cells!
    // (There will never ever be non-blocking cells on the edge of the map.)

    for (int x = area.p0.x; x <= area.p1.x; ++x)
    {
        for (int y = area.p0.y; y <= area.p1.y; ++y)
        {
            if (
                map::player_fov[x][y] &&
                !map::ter[x][y]->blocks())
            {
                const P p(x, y);

                for (const P& d : dir_utils::dir_list)
                {
                    const P other_p = p + d;

                    if (map::ter[other_p.x][other_p.y]->blocks())
                    {
                        map::player_fov[other_p.x][other_p.y] = true;
                    }
                }
            }
        }
    }
}

} // namespace

void run(const P& p, const R& area, FovData out[map_w][map_h])
{
    for (int x = area.p0.x; x <= area.p1.x; ++x)
    {
        for (int y = area.p0.y; y <= area.p1.y; ++y)
        {
            const P tgt(x, y);

            los(p, tgt, out[tgt.x][tgt.y]);
        }
    }
}

void los(const P& p0, const P& p1, FovData& out)
{
    std::vector<P> line;

    // We ensure symmetry here by requiring that the target is visible from
    // the origin, AND that the origin is visible from the target.
    //
    // Note that this could also have been achieved by "OR" instead of "AND",
    // but this would have createad situations where you could, for example,
    // see a cell, but not shoot at it.

    // TODO: This is repeated twice, break it out into a function

    bresenham(p0, p1, line);

    if (!line.empty())
    {
        for (auto it = begin(line); it != end(line); ++it)
        {
            const P check_p = *it;

            // TODO: Should be a separate flag such as "blocks_vision"
            if (map::ter[check_p.x][check_p.y]->blocks())
            {
                out.blocked_hard = true;
                break;
            }
        }
    }

    bresenham(p1, p0, line);

    if (!line.empty())
    {
        for (auto it = begin(line); it != end(line); ++it)
        {
            const P check_p = *it;

            // TODO: Should be a separate flag such as "blocks_vision"
            if (map::ter[check_p.x][check_p.y]->blocks())
            {
                out.blocked_hard = true;
                break;
            }
        }
    }
}

void update_player_fov()
{
    ASSERT(map::player);

    const P& player_p = map::player->p();

    std::fill_n(*fov_data_buffer, nr_map_cells, FovData());

    std::fill_n(*map::player_fov, nr_map_cells, false);

    const int r = los_base_range;

    const R area(std::max(player_p.x - r, 0),
                 std::max(player_p.y - r, 0),
                 std::min(player_p.x + r, map_w - 1),
                 std::min(player_p.y + r, map_h - 1));

    run(player_p, area, fov_data_buffer);

    // Update FOV
    for (int x = area.p0.x; x <= area.p1.x; ++x)
    {
        for (int y = area.p0.y; y <= area.p1.y; ++y)
        {
            const FovData& f = fov_data_buffer[x][y];

            if (!f.blocked_hard && !f.blocked_by_dark)
            {
                map::player_fov[x][y]       = true;
                map::player_explored[x][y]  = true;
            }
        }
    }

    // Make more walls visible
    fov_wall_hack(area);

    // Update exploration
    for (int x = 0; x < map_w; ++x)
    {
        for (int y = 0; y < map_h; ++y)
        {
            if (map::player_fov[x][y])
            {
                map::player_explored[x][y]  = true;
            }
        }
    }
}

} // fov

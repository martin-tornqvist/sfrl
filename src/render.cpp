#include "render.hpp"

#include "rl_utils.hpp"
#include "map.hpp"

namespace render
{

namespace
{

// ASCII codes for box characters
const unsigned char box_ver                 = 179;
const unsigned char box_ver_left            = 180;
const unsigned char box_ver_right           = 195;

const unsigned char box_hor                 = 196;
const unsigned char box_hor_up              = 193;
const unsigned char box_hor_down            = 194;

const unsigned char box_hor_ver             = 197;

const unsigned char box_up_left_corner      = 218;
const unsigned char box_lower_left_corner   = 192;
const unsigned char box_up_right_corner     = 191;
const unsigned char box_lower_right_corner  = 217;

// Get box character for wall, based on surrounding walls and player FOV
unsigned char get_wall_char(const P& p)
{
    // Info on adjacent (seen) walls
    bool inf[3][3] = {};

    // The wall we are trying to decide character for is in the center of the info array
    const P info_center_p(1, 1);

    // Populate the info array
    for (const P& d : dir_utils::cardinal_list)
    {
        const P map_adj_p(p + d);

        if (map::is_pos_inside_map(map_adj_p, true))
        {
            // TODO: Consider FOV

            const P info_p(info_center_p + d);

            const Ter* adj_t = map::ter[map_adj_p.x][map_adj_p.y].get();

            const RenderData d = adj_t->render_d();

            inf[info_p.x][info_p.y] = d.draw_as_wall;
        }
    }

    const bool u = inf[1][0];
    const bool d = inf[1][2];
    const bool l = inf[0][1];
    const bool r = inf[2][1];

    if ( u &&  d && !l && !r) return box_ver;
    if ( u &&  d &&  l && !r) return box_ver_left;
    if ( u &&  d && !l &&  r) return box_ver_right;

    if (!u && !d &&  l &&  r) return box_hor;
    if ( u && !d &&  l &&  r) return box_hor_up;
    if (!u &&  d &&  l &&  r) return box_hor_down;

    if ( u &&  d &&  l &&  r) return box_hor_ver;

    if (!u &&  d && !l &&  r) return box_up_left_corner;
    if ( u && !d && !l &&  r) return box_lower_left_corner;
    if (!u &&  d &&  l && !r) return box_up_right_corner;
    if ( u && !d &&  l && !r) return box_lower_right_corner;

    return box_hor_ver;
}

} // namespace

void vp_update(const P& p,
               const P& map_window_dim,
               const int trigger_dist,
               R& vp)
{
    // map_window_dim must not be bigger than the map
    ASSERT(map_window_dim.x <= map_w);
    ASSERT(map_window_dim.y <= map_h);

    // Our distances from the viewport edges (right, left, down, up)
    const int r = vp.p1.x - p.x;
    const int l = p.x - vp.p0.x;
    const int d = vp.p1.y - p.y;
    const int u = p.y - vp.p0.y;

    // Time to do horizontal adjustment?
    if (r <= trigger_dist || l <= trigger_dist)
    {
        // NOTE: If window width is even (i.e. no center cell), we lean left
        vp.p0.x = p.x - (map_window_dim.x / 2);

        vp.p0.x = std::max(vp.p0.x, 0);
        vp.p0.x = std::min(vp.p0.x, map_w - map_window_dim.x);
    }

    // Time to do vertical adjustment?
    if (d <= trigger_dist || u <= trigger_dist)
    {
        // NOTE: If window height is even (i.e. no center cell), we lean up
        vp.p0.y = p.y - (map_window_dim.y / 2);

        vp.p0.y = std::max(vp.p0.y, 0);
        vp.p0.y = std::min(vp.p0.y, map_h - map_window_dim.y);
    }

    vp.p1.x = vp.p0.x + map_window_dim.x - 1;
    vp.p1.y = vp.p0.y + map_window_dim.y - 1;

    // The viewport should have the same size as the map window
    ASSERT((vp.p1.x - vp.p0.x + 1) == map_window_dim.x);
    ASSERT((vp.p1.y - vp.p0.y + 1) == map_window_dim.y);

    // The viewport should be inside the map
    ASSERT(vp.p0.x >= 0);
    ASSERT(vp.p0.y >= 0);
    ASSERT(vp.p1.x < map_w);
    ASSERT(vp.p1.y < map_h);
}

void draw_map(const R& vp)
{
    for (int x = vp.p0.x; x <= vp.p1.x; ++x)
    {
        for (int y = vp.p0.y; y <= vp.p1.y; ++y)
        {
            const P p(x, y);

            const P             scr_p   = p - vp.p0;
            const Ter* const    t       = map::ter[x][y].get();
            char                c       = 0;

            const RenderData d = t->render_d();

            if (d.draw_as_wall)
            {
                // Special case, draw walls as box sides
                c = get_wall_char(p);
            }
            else // Do not draw as wall
            {
                c   = d.c;
            }

            if (c)
            {
                // A character has been set
                io::draw_char(scr_p,
                              c,
                              d.clr,
                              clr_black);
            }
        }
    }

    // TODO: Only draw monsters inside vp

    for (auto& mon : map::monsters)
    {
        if (mon->is_allive())
        {
            const P& mon_p = mon->p();

            if (vp.is_p_inside(mon_p))
            {
                const P mon_scr_p   = mon_p - vp.p0;

                const RenderData d = mon->render_d();

                io::draw_char(mon_scr_p,
                              d.c,
                              d.clr,
                              clr_black);
            }
        }
    }

    io::update_scr();
}

} // render

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------
// #[cfg(test)]
// mod tests {
//     use super::*;
//     use utils::*;

//     #[test]
//     fn test_vp()
//     {
//         let mut p = P::new_xy(50, 30);
//         let map_window_dim = P::new_xy(100, 60);
//         let trigger_dist = 2;
//         let mut vp = R::new_pp(&P::new(), &map_window_dim.offs(-1));
//         let vp_before = vp;

//         // Verify that viewport does *NOT* change when focus is in center
//         vp_update(&p, &map_window_dim, trigger_dist, &mut vp);

//         ASSERT(vp == vp_before);

//         // Verify that viewport does *NOT* change when focus is almost at trigger
//         // distance
//         vp = vp_before;
//         p.x = 96;

//         vp_update(&p, &map_window_dim, trigger_dist, &mut vp);

//         ASSERT(vp == vp_before);

//         // Verify that viewport changes when focus is at trigger distance
//         vp = vp_before;
//         p.x = 97;

//         vp_update(&p, &map_window_dim, trigger_dist, &mut vp);

//         ASSERT(vp != vp_before);

//         // Verify viewport change when focus is further than trigger distance
//         vp = vp_before;
//         p.x = 98;

//         vp_update(&p, &map_window_dim, trigger_dist, &mut vp);

//         ASSERT(vp != vp_before);
//     }
// }

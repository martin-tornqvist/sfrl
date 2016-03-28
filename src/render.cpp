#include "render.hpp"

namespace render
{

void vp_update(const P& p,
               const P& map_window_dim,
               const int trigger_dist,
               const R& vp)
{
    // map_window_dim must not be bigger than the map
    assert(map_window_dim.x <= MAP_W);
    assert(map_window_dim.y <= MAP_H);

    // Our distances from the viewport edges (right, left, down, up)
    const int r = vp.p1.x - p.x;
    const int l = p.x - vp.p0.x;
    const int d = vp.p1.y - p.y;
    const int u = p.y - vp.p0.y;

    // Time to do horizontal adjustment?
    if (r <= trigger_dist) || (l <= trigger_dist) {
        // NOTE: If window width is even (i.e. no center cell), we lean left
        vp.p0.x = p.x - (map_window_dim.x / 2);

        vp.p0.x = std::max(vp.p0.x, 0);
        vp.p0.x = std::min(vp.p0.x, MAP_W - map_window_dim.x);
    }

    // Time to do vertical adjustment?
    if (d <= trigger_dist) || (u <= trigger_dist) {
        // NOTE: If window height is even (i.e. no center cell), we lean up
        vp.p0.y = p.y - (map_window_dim.y / 2);

        vp.p0.y = std::max(vp.p0.y, 0);
        vp.p0.y = std::min(vp.p0.y, MAP_H - map_window_dim.y);
    }

    vp.p1.x = vp.p0.x + map_window_dim.x - 1;
    vp.p1.y = vp.p0.y + map_window_dim.y - 1;

    // The viewport should have the same size as the map window
    assert((vp.p1.x - vp.p0.x + 1) == map_window_dim.x);
    assert((vp.p1.y - vp.p0.y + 1) == map_window_dim.y);

    // The viewport should be inside the map
    assert(vp.p0.x >= 0);
    assert(vp.p0.y >= 0);
    assert(vp.p1.x < (MAP_W as i32));
    assert(vp.p1.y < (MAP_H as i32));
}

pub fn draw_map(game_map: &Map, vp: &R)
{
    for (int x = vp.p0.x; x <= vp.p1.x; ++x)
    {
        for (int y = vp.p0.y; y <= vp.p1.y; ++y)
        {
            const P scr_p = P(x, y) - vp.p0;

            char ch = 0;
            io::Clr fg = io::Clr::white;
            io::Clr bg = io::Clr::black;

            const Ter ter = game_map.ter[x][y];

            // TODO: This should not be decided here
            switch ter {
                case Ter::floor:
                    ch = '.';
                    fg = io::Clr::White;
                    bg = io::Clr::Black;
                    break;

                case Ter::wall:
                    ch = '#';
                    fg = io::Clr::White;
                    bg = io::Clr::Black;
                    break;
                }

            io::draw_char(&scr_p,
                          ch,
                          fg,
                          bg,
                          io::FontWgt::Normal);
        }
    }

    // TODO: Iterate over all monsters
    // TODO: Only draw monsters inside vp

    const P& mon_p    = game_map.monsters[0].p();
    const P mon_scr_p = mon_p - vp.p0;

    io::draw_char(mon_scr_p,
                  '@',
                  io::Clr::White,
                  io::Clr::Black,
                  io::FontWgt::Bold);

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

//         assert(vp == vp_before);

//         // Verify that viewport does *NOT* change when focus is almost at trigger
//         // distance
//         vp = vp_before;
//         p.x = 96;

//         vp_update(&p, &map_window_dim, trigger_dist, &mut vp);

//         assert(vp == vp_before);

//         // Verify that viewport changes when focus is at trigger distance
//         vp = vp_before;
//         p.x = 97;

//         vp_update(&p, &map_window_dim, trigger_dist, &mut vp);

//         assert(vp != vp_before);

//         // Verify viewport change when focus is further than trigger distance
//         vp = vp_before;
//         p.x = 98;

//         vp_update(&p, &map_window_dim, trigger_dist, &mut vp);

//         assert(vp != vp_before);
//     }
// }

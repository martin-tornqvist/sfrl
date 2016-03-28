#ifndef RENDER_HPP
#define RENDER_HPP

namespace render
{

void vp_update(const P& p,
               const P& map_window_dim,
               const int trigger_dist,
               const R& vp);

pub fn draw_map(game_map: &Map, vp: &R);

} // render

#endif // RENDER_HPP

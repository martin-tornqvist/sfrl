#ifndef RENDER_HPP
#define RENDER_HPP

class P;
class R;
class Map;

namespace render
{

void vp_update(const P& p,
               const P& map_window_dim,
               const int trigger_dist,
               R& vp);

void draw_map(const Map& map, const R& vp);

} // render

#endif // RENDER_HPP

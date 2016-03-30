#ifndef RENDER_HPP
#define RENDER_HPP

#include "io.hpp"

class P;
class R;
class Map;

struct RenderData
{
    RenderData() :
        c   (0),
        clr (clr_white) {}

    unsigned char c;
    Clr clr;
};

namespace render
{

void vp_update(const P& p,
               const P& map_window_dim,
               const int trigger_dist,
               R& vp);

void draw_map(const Map& map, const R& vp);

} // render

#endif // RENDER_HPP

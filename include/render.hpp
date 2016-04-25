#ifndef RENDER_HPP
#define RENDER_HPP

#include "io.hpp"

class P;
class R;

struct RenderData
{
    RenderData(unsigned char c = 0,
               Clr clr = clr_white,
               Clr bg_clr = clr_black) :
        c               (c),
        clr             (clr),
        bg_clr          (bg_clr),
        draw_as_wall    (false) {}

    unsigned char c;
    Clr clr;
    Clr bg_clr;
    bool draw_as_wall; // Dynamic "box" drawing
};

namespace render
{

void init();

void draw_map_state();

} // render

#endif // RENDER_HPP

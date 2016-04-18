#ifndef RENDER_HPP
#define RENDER_HPP

#include "io.hpp"

class P;
class R;

struct RenderData
{
    RenderData() :
        c               (0),
        clr             (clr_white),
        draw_as_wall    (false) {}

    RenderData(unsigned char c, Clr clr) :
        c               (c),
        clr             (clr),
        draw_as_wall    (false) {}

    unsigned char c;
    Clr clr;
    bool draw_as_wall; // Dynamic "box" drawing
};

namespace render
{

void init();

void draw_map_state();

} // render

#endif // RENDER_HPP

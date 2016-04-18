#include "msg.hpp"

#include "io.hpp"
#include "render.hpp"

namespace msg
{

namespace
{

Msg msg;
int nr_msgs = 0;

} // namespace

void init()
{
    nr_msgs = 0;
}

void draw()
{
    ASSERT(nr_msgs <= 2);

    // Clear this area
    const P scr_dim = io::scr_dim();
    io::clear_area(R(0, 0, scr_dim.x, 1));

    // Draw message
    if (nr_msgs >= 1)
    {
        io::draw_text(P(0, 0), msg.str, msg.clr);

        // If there are more messages waiting, draw a notice and wait for user input
        if (nr_msgs == 2)
        {
            wait_more();
        }
    }
}

void add(const std::string& str, Clr clr)
{
    ASSERT(!str.empty());
    ASSERT(nr_msgs <= 1);

    ++nr_msgs;

    // Draw existing message first, and query for proceeding
    if (nr_msgs == 2)
    {
        render::draw_map_state();

        io::wait_for_proceed();

        --nr_msgs;
    }

    msg.str = str;
    msg.clr = clr;

    // NOTE: This will cause the renderer to call the draw function in thie module
    render::draw_map_state();
}

void clear()
{
    nr_msgs = 0;
}

void wait_more()
{
    if (nr_msgs >= 1)
    {
        const int msg_w = msg.str.length();

        io::draw_text(P(msg_w + 1, 0), "(more)", clr_black, clr_white);
    }
}

} // msg

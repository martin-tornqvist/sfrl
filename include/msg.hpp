#ifndef MSG_HPP
#define MSG_HPP

#include <string>

#include "io.hpp"

struct Msg
{
    Msg(const std::string& str, const Clr& clr) :
        str (str),
        clr (clr) {}

    Msg() :
        str (),
        clr (clr_white) {}

    std::string str;
    Clr clr;
};

namespace msg
{

void init();

// Called by the renderer in map state
// NOTE: When a message is added which triggers a "more" prompt, this module
// (msg) will call the renderer to draw the map. This will in turn cause the
// renderer to call this draw() function.
void draw();

void add(const std::string& str, Clr clr = clr_white);

void clear();

void wait_more();

} // msg

#endif // MSG_HPP

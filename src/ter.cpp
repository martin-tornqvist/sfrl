#include "ter.hpp"

#include "map.hpp"

namespace ter
{

void bump(Ter& t)
{
    if (t.id == TerId::door_closed)
    {
        open(t);
    }
}

void open(Ter& t)
{
    if (t.id == TerId::door_closed)
    {
        t = ter_factory::mk(TerId::door_open, t.p);
    }
}

void close(Ter& t)
{

}

} // ter

namespace ter_factory
{

// TODO: This could/should be loaded externally (e.g. Lua)
Ter mk(const TerId id, const P& p)
{
    Ter t;

    t.p = p;
    t.id = id;

    switch (id)
    {
    case TerId::floor:
        t.render_d.c = '.';
        t.render_d.clr = clr_gray;
        t.allow_mv = true;
        break;

    case TerId::wall:
        // NOTE: No need to set the character here; the renderer handles it dynamically.
        t.render_d.clr = clr_blue;
        t.allow_mv = false;
        break;

    case TerId::door_closed:
        t.render_d.c = '+';
        t.render_d.clr = clr_white_high;
        t.allow_mv = false;
        break;

    case TerId::door_open:
        t.render_d.c = '_';
        t.render_d.clr = clr_white_high;
        t.allow_mv = true;
        break;

    case TerId::end:
        ASSERT(false);
        break;

    } // switch

    return t;
}

Ter& mk_on_map(const TerId id, const P& p, Map& map)
{
    Ter& t = map.ter[p.x][p.y];

    t = mk(id, p);

    return t;
}

} // ter_factory

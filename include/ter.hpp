#ifndef TER_HPP
#define TER_HPP

#include "io.hpp"
#include "render.hpp"

class P;
class Map;

enum class TerId
{
    floor,
    wall,
    door_closed,
    door_open,
    end
};

struct Ter
{
    Ter() :
        p           (),
        render_d    (),
        allow_mv    (true) {}

    TerId id;
    P p;
    RenderData render_d;
    bool allow_mv;
};

namespace ter
{

void bump(Ter& t);

void open(Ter& t);

void close(Ter& t);

} // ter

namespace ter_factory
{

// NOTE: Does not put the terrain on the map, as the pos parameter might suggest.
// The parameter is merely used for initializing the internal data. Use the
// function below to immediately place it on the map (convenience).
Ter mk(const TerId id, const P& p);

Ter& mk_on_map(const TerId id, const P& p, Map& map);

} // ter_factory

#endif // TER_HPP

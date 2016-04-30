#ifndef MAPGEN_HPP
#define MAPGEN_HPP

#include <vector>
#include <memory>

#include "rl_utils.hpp"

enum class Axis
{
    hor,
    ver
};

namespace mapgen
{

void run();

struct AreaSource
{
    AreaSource(const P& p = P(), Dir dir = Dir::center) :
        p   (p),
        dir (dir) {}

    P p;
    Dir dir;
};

class MapArea
{
public:
    MapArea() :
        r_() {}

    virtual ~MapArea() {}

    virtual bool build(const AreaSource& source) = 0;

    R r_;
};

class Room : public MapArea
{
public:
    Room() :
        MapArea() {}

    bool build(const AreaSource& source);
};

class Corridor : public MapArea
{
public:
    Corridor() :
        MapArea() {}

    bool build(const AreaSource& source);
};

} // namespace

#endif // MAPGEN_HPP

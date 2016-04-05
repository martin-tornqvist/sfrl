#include "map.hpp"

Map::Map() :
    monsters()
{
    for (int x = 0; x < map_w; ++x)
    {
        for (int y = 0; y < map_h; ++y)
        {
            ter[x][y] = ter::mk(TerId::wall, P(x, y));
        }
    }
}

namespace map
{

bool is_pos_inside_map(const P& p, const bool COUNT_EDGE_AS_INSIDE)
{
    if (COUNT_EDGE_AS_INSIDE)
    {
        return p.x >= 0     &&
               p.y >= 0     &&
               p.x < map_w  &&
               p.y < map_h;
    }
    else //Edge counts as outside the map
    {
        return p.x > 0          &&
               p.y > 0          &&
               p.x < map_w - 1  &&
               p.y < map_h - 1;
    }
}

} // map

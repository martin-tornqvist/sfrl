#include "map.hpp"

Map::Map()
{
    for (int x = 0; x < map_w; ++x)
    {
        for (int y = 0; y < map_h; ++y)
        {
            ter[x][y] = Ter::wall;
        }
    }
}

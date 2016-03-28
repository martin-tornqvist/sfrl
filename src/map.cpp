#include "map.hpp"

Map::Map()
{
    for (int x = 0; x < MAP_W; ++x)
    {
        for (int y = 0; y < MAP_H; ++y)
        {
            ter[x][y] = Ter::wall;
        }
    }
}

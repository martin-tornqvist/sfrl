#ifndef MAP_HPP
#define MAP_HPP

#include <vector>

#include "mon.hpp"
#include "ter.hpp"

const int map_w = 256;
const int map_h = 256;

class Map
{
public:
    Map();

    Ter ter[map_w][map_h];
    std::vector<Mon> monsters;
};

namespace map
{

bool is_pos_inside_map(const P& p, const bool COUNT_EDGE_AS_INSIDE);

} // map

#endif // MAP_HPP

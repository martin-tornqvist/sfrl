#ifndef MAP_HPP
#define MAP_HPP

#include <vector>

#include "mon.hpp"
#include "io.hpp"

const int map_w = 256;
const int map_h = 256;

enum class Ter
{
    floor,
    wall,
};

struct TerData
{
    char g;
    Clr clr;
};

class Map
{
public:
    Map();

    Ter ter[map_w][map_h];
    std::vector<Mon> monsters;
};

#endif // MAP_HPP

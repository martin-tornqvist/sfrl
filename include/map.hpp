#ifndef MAP_HPP
#define MAP_HPP

#include <vector>

#include "mon.hpp"
#include "io.hpp"

const int MAP_W = 256;
const int MAP_H = 256;

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

    Ter ter[MAP_W][MAP_H];
    std::vector<Mon> monsters;
};

#endif // MAP_HPP

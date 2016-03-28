#include <vector>

#include "mon.hpp"

const int MAP_W = 256;
const int MAP_H = 256;

enum class Ter
{
    floor,
    wall,
}

struct TerData
{
    char g;
    io::Clr clr;
};

struct Map
{
    ter[MAP_W][MAP_H];
    std::vector<Mon> monsters;
}

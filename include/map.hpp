#ifndef MAP_HPP
#define MAP_HPP

#include <vector>
#include <memory>

#include "mon.hpp"
#include "ter.hpp"

const int map_w = 256;
const int map_h = 256;

namespace map
{

extern std::unique_ptr<Ter>                 ter[map_w][map_h];  // Terrain
extern std::vector< std::unique_ptr<Mon> >  monsters;           // Monsters

// NOTE: This serves two purposes:
// 1) It is the definition of WHICH monster the player character is
// 2) It is a convenient shortcut to the player object (which often needs to be referred to)
//
// The monster vector above is the actual owner of the player object however.
//
extern Mon* player;

void init();

void clear_terrain();

// Removes all monsters except the one pointed to by the player pointer above.
// This should be called before moving to another map.
void clear_non_player_monsters();

void remove_mon(Mon* mon);

bool is_pos_inside_map(const P& p, const bool COUNT_EDGE_AS_INSIDE);

Mon* living_mon_at(const P& p);

} // map

#endif // MAP_HPP

#include "map.hpp"

#include "rl_utils.hpp"
#include "fov.hpp"

void BoolMap::cells_with_value(const bool value, std::vector<P>& out)
{

    to_vec((bool*)data,
           value,
           map_w,
           map_h,
           out);
}

namespace map
{

std::unique_ptr<Ter> ter[map_w][map_h];

std::vector< std::unique_ptr<Mon> > monsters;

bool player_fov[map_w][map_h];

bool player_explored [map_w][map_h];

Mon* player = nullptr;

void init()
{
    player = nullptr;

    monsters.clear();

    std::fill_n(*player_explored, nr_map_cells, false);

    clear_terrain();
}

void clear_terrain()
{
    for (int x = 0; x < map_w; ++x)
    {
        for (int y = 0; y < map_h; ++y)
        {
            ter[x][y] = ter::mk(TerId::rock_wall, P(x, y));
        }
    }
}

void clear_non_player_monsters()
{
    for (auto it = begin(monsters); it != end(monsters); /* No increment */)
    {
        if (it->get() != player)
        {
            // Not player, bye bye!
            it = monsters.erase(it);
        }
        else // This is the player object
        {
            // Just skip this element
            ++it;
        }
    }
}

void remove_mon(Mon* mon)
{
    for (auto it = begin(monsters); it != end(monsters); ++it)
    {
        if (it->get() == mon)
        {
            monsters.erase(it);
            return;
        }
    }

    ASSERT(false);
}

bool is_pos_inside_map(const P& p, const bool COUNT_EDGE_AS_INSIDE)
{
    if (COUNT_EDGE_AS_INSIDE)
    {
        return p.x >= 0     &&
               p.y >= 0     &&
               p.x < map_w  &&
               p.y < map_h;
    }
    else // Edge counts as outside the map
    {
        return p.x > 0          &&
               p.y > 0          &&
               p.x < map_w - 1  &&
               p.y < map_h - 1;
    }
}

Mon* living_mon_at(const P& p)
{
    for (auto& mon : monsters)
    {
        if (mon->is_allive() && mon->p() == p)
        {
            return mon.get();
        }
    }

    return nullptr;
}

} // map

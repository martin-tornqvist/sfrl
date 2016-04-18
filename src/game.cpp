#include "game.hpp"

#include "rl_utils.hpp"
#include "time.hpp"
#include "msg.hpp"
#include "map.hpp"

namespace game
{

bool quit = false;

void init()
{
    quit = false;

    ter::init();
    mon::init();
    map::init();
    msg::init();
}

void run_session()
{
    init();

    for (int x = 1; x < map_w - 1; ++x)
    {
        for (int y = 1; y < map_h - 1; ++y)
        {
            map::ter[x][y] = ter::mk(TerId::floor, P(x, y));
        }
    }

    map::ter[4][4] = ter::mk(TerId::door, P(4, 4));

    map::ter[4][6] = ter::mk(TerId::force_field, P(4, 6));

    // Make a player monster
    {
        std::unique_ptr<Mon> player = mon::mk(MonId::player, P(2, 4));

        map::player = player.get();

        map::monsters.emplace_back(std::move(player));
    }

    msg::add("Welcome to the game, idiot!");

    // Make a monster
    map::monsters.emplace_back(
        mon::mk(MonId::mutant_humanoid, P(12, 6)));

    while (!quit)
    {
        // Run one tick of game time
        time::tick();

        if (!map::player->is_allive())
        {
            quit = true;
        }
    }
}

} // game

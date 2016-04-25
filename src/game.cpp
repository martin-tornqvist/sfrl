#include "game.hpp"

#include "rl_utils.hpp"
#include "time.hpp"
#include "msg.hpp"
#include "map.hpp"
#include "mapgen.hpp"

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

    mapgen::run();

    // Make a player monster in a random free cell
    {
        BoolMap blocked_cells;

        for (int x = 0; x < map_w; ++x)
        {
            for (int y = 0; y < map_h; ++y)
            {
                const bool blocks = map::ter[x][y]->blocks();

                blocked_cells.set(P(x, y), blocks);
            }
        }

        for (const auto& mon : map::monsters)
        {
            blocked_cells.set(mon->p(), true);
        }

        std::vector<P> free_cells_vec;

        blocked_cells.cells_with_value(false, free_cells_vec);

        ASSERT(!free_cells_vec.empty());

        const P& player_pos = rnd::element(free_cells_vec);

        std::unique_ptr<Mon> player = mon::mk(MonId::player, player_pos);

        map::player = player.get();

        map::monsters.emplace_back(std::move(player));
    }

    msg::add("Welcome to the game, idiot!");

    // Make a monster
//    map::monsters.emplace_back(
//        mon::mk(MonId::mutant_humanoid, P(12, 6)));

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

#include "io.hpp"

#include "map.hpp"
#include "mon.hpp"
#include "render.hpp"
#include "ter.hpp"
#include "time.hpp"

#if defined(_WIN32) && defined(SDL_MODE)
#undef main
#endif
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    io::init();

    ter::init();
    mon::init();

    map::init();

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

    // Make a monster
    map::monsters.emplace_back(
        mon::mk(MonId::mutant_humanoid, P(12, 6)));

    R vp;

    io::clear_scr();

    // ------------------------------------------------------------------------
    // Game loop
    // ------------------------------------------------------------------------
    int prev_turn_nr = -1;

    while (true)
    {
        const int cur_turn_nr = time::turn();

        // Is this a new turn (i.e. is this the first tick in this turn)?
        const bool is_new_turn = cur_turn_nr > prev_turn_nr;

        prev_turn_nr = cur_turn_nr;

        // Check if game window is big enough
        P scr_dim(io::scr_dim());

        if (scr_dim.x < 80 || scr_dim.y < 24)
        {
            // Too small! Just print a warning
            io::clear_scr();

            io::draw_text(P(0, 0),
                          "Game window too small!",
                          clr_red);
        }
        else if (is_new_turn) // Window is big enough
        {
            // OK, we have a new turn, let's draw the map
            P map_window_dim = scr_dim;

            map_window_dim.x = std::min(map_window_dim.x, map_w);
            map_window_dim.y = std::min(map_window_dim.y, map_h);

            const int vp_trigger_dist = 3;

            render::vp_update(map::monsters[0]->p(),
                              map_window_dim,
                              vp_trigger_dist,
                              vp);

            render::draw_map(vp);
        }

        // Run one tick of game time
        time::tick();

        // If screen has been resized, trigger a viewport update on next iteration
        if (io::scr_dim() != scr_dim)
        {
            vp = R();
        }
    }

    io::cleanup();

    return 0;
}

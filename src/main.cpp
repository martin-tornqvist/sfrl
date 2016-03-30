#include "io.hpp"

#include "map.hpp"
#include "mon.hpp"
#include "render.hpp"
#include "ter.hpp"

#if defined(_WIN32) && defined(SDL_MODE)
#undef main
#endif
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    io::init();

    Map map;

    for (int x = 1; x < map_w - 1; ++x)
    {
        for (int y = 1; y < map_h - 1; ++y)
        {
            ter_factory::mk_on_map(TerId::floor, P(x, y), map);
        }
    }

    ter_factory::mk_on_map(TerId::door_closed, P(4, 4), map);

    {
        Mon player = Mon(P(2, 4));

        map.monsters.push_back(player);
    }

    R vp;

    io::clear_scr();

    // ------------------------------------------------------------------------
    // Game loop
    // ------------------------------------------------------------------------
    while (true)
    {
        P scr_dim(io::scr_dim());

        if (scr_dim.x < 80 || scr_dim.y < 24)
        {
            io::clear_scr();

            io::draw_text(P(0, 0),
                          "Game window too small!",
                          clr_red);
        }
        else //Window is big enough
        {
            P map_window_dim = scr_dim;

            map_window_dim.x = std::min(map_window_dim.x, map_w);
            map_window_dim.y = std::min(map_window_dim.y, map_h);

            const int vp_trigger_dist = 3;

            render::vp_update(map.monsters[0].p(),
                              map_window_dim,
                              vp_trigger_dist,
                              vp);

            render::draw_map(map, vp);
        }

        Input inp = io::get_input();

        if (inp.c == 'q')
        {
            break;
        }
        else if (inp.c == '1')
        {
            map.monsters[0].mv(Dir::down_left, map);
        }
        else if (inp.c == '2' || inp.keycode == key_down)
        {
            map.monsters[0].mv(Dir::down, map);
        }
        else if (inp.c == '3')
        {
            map.monsters[0].mv(Dir::down_right, map);
        }
        else if (inp.c == '4' || inp.keycode == key_left)
        {
            map.monsters[0].mv(Dir::left, map);
        }
        else if (inp.c == '5' || inp.c == ',')
        {
            map.monsters[0].mv(Dir::center, map);
        }
        else if (inp.c == '6' || inp.keycode == key_right)
        {
            map.monsters[0].mv(Dir::right, map);
        }
        else if (inp.c == '7')
        {
            map.monsters[0].mv(Dir::up_left, map);
        }
        else if (inp.c == '8' || inp.keycode == key_up)
        {
            map.monsters[0].mv(Dir::up, map);
        }
        else if (inp.c == '9')
        {
            map.monsters[0].mv(Dir::up_right, map);
        }

        // If screen has been resized, trigger a viewport update next iteration
        if (io::scr_dim() != scr_dim)
        {
            vp = R();
        }
    }

    io::cleanup();

    return 0;
}

#include "io.hpp"

#include "map.hpp"
#include "mon.hpp"
#include "render.hpp"

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
            map.ter[x][y] = Ter::floor;
        }
    }

    map.ter[18][3] = Ter::wall;

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
            map.monsters[0].mv(Dir::down_left);
        }
        else if (inp.c == '2' || inp.keycode == key_down)
        {
            map.monsters[0].mv(Dir::down);
        }
        else if (inp.c == '3')
        {
            map.monsters[0].mv(Dir::down_right);
        }
        else if (inp.c == '4' || inp.keycode == key_left)
        {
            map.monsters[0].mv(Dir::left);
        }
        else if (inp.c == '5' || inp.c == ',')
        {
            map.monsters[0].mv(Dir::center);
        }
        else if (inp.c == '6' || inp.keycode == key_right)
        {
            map.monsters[0].mv(Dir::right);
        }
        else if (inp.c == '7')
        {
            map.monsters[0].mv(Dir::up_left);
        }
        else if (inp.c == '8' || inp.keycode == key_up)
        {
            map.monsters[0].mv(Dir::up);
        }
        else if (inp.c == '9')
        {
            map.monsters[0].mv(Dir::up_right);
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

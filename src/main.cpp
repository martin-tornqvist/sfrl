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

    for (int x = 1; x < MAP_W - 1; ++x)
    {
        for (int y = 1; y < MAP_H - 1; ++y)
        {
            map.ter[x][y] = Ter::floor;
        }
    }

    map.ter[18][3] = Ter::wall;

    Mon player = Mon(P(2, 4));

    map.monsters.push_back(player);

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

            map_window_dim.x = std::min(map_window_dim.x, MAP_W);
            map_window_dim.y = std::min(map_window_dim.y, MAP_H);

            const int VP_TRIGGER_DIST = 3;

            render::vp_update(player.p(), map_window_dim, VP_TRIGGER_DIST, vp);

            render::draw_map(map, vp);
        }

        Input inp = io::get_input();

        if (inp.c == 'q')
        {
            break;
        }

//        match inp {
//            ('q', 0) => {
//                break;
//            }
//
//            ('6', 0) | ('\n', io::KEY_RIGHT) => {
//                player.mv(Dir::Right);
//            }
//
//            ('4', 0) | ('\n', io::KEY_LEFT) => {
//                player.mv(Dir::Left);
//            }
//
//            ('2', 0) | ('\n', io::KEY_DOWN) => {
//                player.mv(Dir::Down);
//            }
//
//            ('8', 0) | ('\n', io::KEY_UP) => {
//                player.mv(Dir::Up);
//            }
//
//            ('3', 0) => {
//                player.mv(Dir::DownRight);
//            }
//
//            ('9', 0) => {
//                player.mv(Dir::UpRight);
//            }
//
//            ('1', 0) => {
//                player.mv(Dir::DownLeft);
//            }
//
//            ('7', 0) => {
//                player.mv(Dir::UpLeft);
//            }
//
//            _ => {}
//        }

        // If screen has been resized, trigger a viewport update next iteration
        if (io::scr_dim() != scr_dim)
        {
            vp = R();
        }
    }

    io::cleanup();
}

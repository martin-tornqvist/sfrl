// TODO: There is probably a better way to do this(?)
#[cfg(unix)]
pub mod ncurses;
#[cfg(unix)]
pub use ncurses::io;

mod utils;
mod mon;
mod map;
mod render;

use utils::*;
use mon::*;
use map::*;
use render::*;

use std::cmp;

fn main()
{
    println!("main()");

    io::init();

    let mut game_map = Map::new();

    for x in 1..MAP_W - 1 {
        if x % 2 == 0 {
            game_map.ter[x][0] = Ter::Floor;
        }
    }

    for x in 1..MAP_W - 1 {
        for y in 1..MAP_H - 1 {
            game_map.ter[x][y] = Ter::Floor;
        }
    }

    game_map.ter[18][3] = Ter::Wall;

    let player = Mon::new(&P::new_xy(2, 4));

    game_map.monsters.push(player);

    let mut vp = R::new();

    io::clear_scr();

    // ------------------------------------------------------------------------
    // Game loop
    // ------------------------------------------------------------------------
    loop {
        let scr_dim = io::scr_dim();

        if scr_dim.x < 80 || scr_dim.y < 24 {
            io::clear_scr();

            io::draw_text(&P { x: 0, y: 0 },
                          &format!("Game window too small!"),
                          io::Clr::Red,
                          io::Clr::Black,
                          io::FontWgt::Bold);

        } else {
            let mut map_window_dim = scr_dim;

            map_window_dim.x = cmp::min(map_window_dim.x, (MAP_W as i32));
            map_window_dim.y = cmp::min(map_window_dim.y, (MAP_H as i32));

            let player_p = *game_map.monsters[0].p();

            let vp_trigger_dist = 3;

            vp_update(&player_p, &map_window_dim, vp_trigger_dist, &mut vp);

            draw_map(&game_map, &vp);
        }

        let player = &mut game_map.monsters[0];

        let inp = io::get_input();

        match inp {
            ('q', 0) => {
                break;
            }

            ('6', 0) | ('\n', io::KEY_RIGHT) => {
                player.mv(Dir::Right);
            }

            ('4', 0) | ('\n', io::KEY_LEFT) => {
                player.mv(Dir::Left);
            }

            ('2', 0) | ('\n', io::KEY_DOWN) => {
                player.mv(Dir::Down);
            }

            ('8', 0) | ('\n', io::KEY_UP) => {
                player.mv(Dir::Up);
            }

            ('3', 0) => {
                player.mv(Dir::DownRight);
            }

            ('9', 0) => {
                player.mv(Dir::UpRight);
            }

            ('1', 0) => {
                player.mv(Dir::DownLeft);
            }

            ('7', 0) => {
                player.mv(Dir::UpLeft);
            }

            _ => {}
        }

        // If screen has been resized, trigger a viewport update next iteration
        if io::scr_dim() != scr_dim {
            vp = R::new();
        }
    }

    io::cleanup();

    println!("main() [DONE]");
}

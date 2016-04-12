#include "ai.hpp"

#include "io.hpp"
#include "mon.hpp"
#include "map.hpp"

void PlayerMonCtl::act()
{
    while (true)
    {
        Input inp = io::get_input();

        /* if (inp.c == 'q')
        {
            break;
        }
        else */ if (inp.c == '1')
        {
            mon_->mv(Dir::down_left);
        }
        else if (inp.c == '2' || inp.keycode == key_down)
        {
            mon_->mv(Dir::down);
        }
        else if (inp.c == '3')
        {
            mon_->mv(Dir::down_right);
        }
        else if (inp.c == '4' || inp.keycode == key_left)
        {
            mon_->mv(Dir::left);
        }
        else if (inp.c == '5' || inp.c == ',')
        {
            mon_->mv(Dir::center);
        }
        else if (inp.c == '6' || inp.keycode == key_right)
        {
            mon_->mv(Dir::right);
        }
        else if (inp.c == '7')
        {
            mon_->mv(Dir::up_left);
        }
        else if (inp.c == '8' || inp.keycode == key_up)
        {
            mon_->mv(Dir::up);
        }
        else if (inp.c == '9')
        {
            mon_->mv(Dir::up_right);
        }

        if (mon_->has_acted_this_tick_)
        {
            break;
        }
    }
}

void Ai::act()
{
    P d = map::player->p() - mon_->p();

    d = d.signs();

    const Dir dir = dir_utils::dir(d);

    mon_->mv(dir);

}

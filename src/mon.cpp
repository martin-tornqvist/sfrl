#include "mon.hpp"

#include "rl_utils.hpp"
#include "map.hpp"

void Mon::mv(const Dir dir, Map& map)
{
    const P new_p = p_ + dir_utils::offset(dir);

    Ter* t = map.ter[new_p.x][new_p.y].get();

    if (!t->blocks())
    {
        const P old_p = p_;

        p_ = new_p;

        Ter* old_t = map.ter[old_p.x][old_p.y].get();

        old_t->on_step_off(this);
    }

    t->on_bump(this);
}

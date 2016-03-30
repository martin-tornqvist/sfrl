#include "mon.hpp"

#include "rl_utils.hpp"
#include "map.hpp"

void Mon::mv(const Dir dir, Map& map)
{
    const P new_p = p_ + dir_utils::offset(dir);

    Ter& t = map.ter[new_p.x][new_p.y];

    if (t.allow_mv)
    {
        p_ = new_p;
    }

    ter::bump(t);
}

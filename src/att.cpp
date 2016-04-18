#include "att.hpp"

#include "mon.hpp"

namespace att
{

void melee(Mon& attacker, Mon& defender)
{
    if (rnd::one_in(3))
    {
        defender.die();
    }
}

} // att

#include "att.hpp"

#include "mon.hpp"

namespace att
{

void melee(Mon& attacker, Mon& defender)
{
    const int dmg = rnd::range(1, 4);

    defender.take_dmg(dmg);
}

} // att

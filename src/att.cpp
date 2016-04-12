#include "att.hpp"

#include "mon.hpp"

namespace att
{

void melee(Mon& attacker, Mon& defender)
{
    defender.die();
}

} // att

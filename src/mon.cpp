#include "mon.hpp"

#include "rl_utils.hpp"

void Mon::mv(const Dir dir)
{
    p_ += dir_utils::offset(dir);
}

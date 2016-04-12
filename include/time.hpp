#ifndef TIME_HPP
#define TIME_HPP

namespace time
{

int turn();

// Advances time. This should ONLY be called from the main game loop.
void tick();

}

#endif // TIME_HPP

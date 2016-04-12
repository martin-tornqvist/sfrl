#include "time.hpp"

#include "rl_utils.hpp"
#include "io.hpp"
#include "map.hpp"
#include "render.hpp"

namespace time
{

namespace
{

const int ticks_per_turn_ = 20;

int tick_nr_;

} // namepsace

int turn()
{
    return tick_nr_ / ticks_per_turn_;
}

void tick()
{
    ++tick_nr_;

    const bool has_monsters = !map::monsters.empty();

    ASSERT(has_monsters);

    if (!has_monsters)
    {
        return;
    }


    // TODO: Prune dead monsters from the vector sometimes (e.g. 100th tick or so)


    // NOTE: We must take into account that elements may be pushed back on the
    // monster vector while we iterate over it (e.g. monsters spawning monsters).
    // Using size() as exit condition should be a good strategy here.
    for (size_t i = 0; i < map::monsters.size(); ++i)
    {
        auto& mon = map::monsters[i];

        if (mon->is_allive())
        {
            mon->has_acted_this_tick_ = false;

            if (mon->ticks_delay_ <= 0)
            {
                mon->act();

                // Set delay until next action
                const int speed = mon->speed();

                mon->ticks_delay_ = ticks_per_turn_ * 100 / speed;
            }
            else // Monster cannot yet act
            {
                --mon->ticks_delay_;
            }
        }
    }

    // This is a new turn
    if (tick_nr_ % ticks_per_turn_ == 0)
    {
        // Notify terrain of new turn event
        for (int x = 0; x < map_w; ++x)
        {
            for (int y = 0; y < map_h; ++y)
            {
                map::ter[x][y]->on_new_turn();
            }
        }
    }
}

} // time

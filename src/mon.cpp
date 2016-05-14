#include "mon.hpp"

#include "rl_utils.hpp"
#include "map.hpp"
#include "att.hpp"
#include "ai.hpp"
#include "msg.hpp"

namespace mon
{

MonData data[(size_t)MonId::end];

//-----------------------------------------------------------------------------
// Monster data
//-----------------------------------------------------------------------------
void init()
{
    {
        // Player
        MonData& d = data[(size_t)MonId::player];
        d.render_d = RenderData('@', clr_white_high);
        d.speed = 100;
        d.life = 8;
        d.energy = 20;
    }
    {
        // Humanoid mutant
        MonData& d = data[(size_t)MonId::mutant_humanoid];
        d.render_d = RenderData('P', clr_white);
        d.speed = 80;
        d.life = 8;
        d.energy = 20;
    }
}

//-----------------------------------------------------------------------------
// Monster factory
//-----------------------------------------------------------------------------
std::unique_ptr<Mon> mk(MonId id, const P& p)
{
    MonData& d = data[(size_t)id];

    std::unique_ptr<Mon> mon(new Mon(d, id, p));

    // TODO: Trait configuration should perhaps be controlled by the data instead
    switch (id)
    {
    case MonId::player:
        mon->ctl_.reset(new PlayerMonCtl(*mon.get()));
        break;

    case MonId::mutant_humanoid:
        mon->ctl_.reset(new Ai(*mon.get()));
        break;

    default:
        // No trait configuration needed
        break;
    }

    return mon;
}

} // mon

Mon::Mon(MonData& data, MonId id, const P& p) :
    data_                   (data),
    ctl_                    (nullptr),
    ticks_delay_            (0),
    has_acted_this_tick_    (true),
    state_                  (MonState::alive),
    id_                     (id),
    p_                      (p),
    life_                   (data.life),
    energy_                 (data.energy) {}

RenderData Mon::render_d() const
{
    return data_.render_d;
}

bool Mon::is_player() const
{
    return this == map::player;
}

void Mon::act()
{
    const bool has_controller = (bool)ctl_;

    ASSERT(has_controller);

    if (!is_allive() || !has_controller)
    {
        return;
    }

    ctl_->act();
}

void Mon::mv(const Dir dir)
{
    if (dir == Dir::center)
    {
        // Nothing to do, just wait
        has_acted_this_tick_ = true;
    }
    else // Not center direction
    {
        const P tgt_p = p_ + dir_utils::offset(dir);

        Mon* const tgt_mon = map::living_mon_at(tgt_p);

        // Attack other monster?
        if (tgt_mon)
        {
            att::melee(*this, *tgt_mon);

            has_acted_this_tick_ = true;
        }
        else // No monster at target position
        {
            Ter* t = map::ter[tgt_p.x][tgt_p.y].get();

            if (!t->blocks())
            {
                const P old_p = p_;

                p_ = tgt_p;

                Ter* old_t = map::ter[old_p.x][old_p.y].get();

                old_t->on_step_off(this);

                has_acted_this_tick_ = true;
            }

            // NOTE: This may set "has acted" flag
            t->on_bump(this);
        }
    }
}

void Mon::take_dmg(const int dmg)
{
    life_ -= dmg;

    if (life_ <= 0)
    {
        die();
    }
}

void Mon::die()
{
    state_ = MonState::dead;

    msg::add("The monster dies.");
    msg::add("Welcome to level 999!");
}

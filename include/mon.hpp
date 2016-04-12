#ifndef MON_HPP
#define MON_HPP

#include <memory>

#include "rl_utils.hpp"
#include "render.hpp"
#include "ai.hpp"

class Mon;

enum class MonId
{
    player,
    mutant_humanoid,
    end
};

struct MonData
{
    MonData() :
        render_d    (),
        speed       () {}

    RenderData render_d;

    // Measured in percent (default is 100%)
    int speed;
};

enum class MonState
{
    alive,
    dead
};

namespace mon
{

extern MonData data[(size_t)MonId::end];

void init();

std::unique_ptr<Mon> mk(MonId id, const P& p);

} // ter

class Mon
{
public:
    Mon(MonData& data, MonId id, const P& p);

    ~Mon() {}

    const P& p() const
    {
        return p_;
    }

    bool is_allive() const
    {
        return state_ == MonState::alive;
    }

    RenderData render_d() const;

    int speed() const;

    void act();

    void mv(const Dir dir);

    void die();

    MonData& data_;

    std::unique_ptr<MonCtl> ctl_;

    int ticks_delay_;

    // Tracks if actor has acted yet in the current tick
    bool has_acted_this_tick_;

    MonState state_;

private:
    MonId id_;
    P p_;
};

#endif // MON_HPP

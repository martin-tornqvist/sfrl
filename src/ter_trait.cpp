#include "ter_trait.hpp"

#include "ter.hpp"
#include "mon.hpp"
#include "msg.hpp"

namespace ter
{

namespace trait
{

void AutoCloseTimer::trigger()
{
    ASSERT((bool)ter_.trait_openable_);

    if (ter_.trait_openable_)
    {
        ter_.trait_openable_->try_close(nullptr);
    }
}

bool Openable::try_open(Mon* const mon)
{
    if (is_open_)
    {
        return false;
    }

    bool allowed = true;

    if (open_cond_)
    {
        allowed = open_cond_->run(mon);
    }

    if (allowed)
    {
        for (auto& trigger : ter_.triggers_)
        {
            trigger->on_open();
        }

        msg::add("The thing opens.");

        is_open_ = true;

        if (mon)
        {
            mon->has_acted_this_tick_ = true;
        }
    }

    return is_open_;
}

bool Openable::try_close(Mon* const mon)
{
    (void)mon;

    if (!is_open_)
    {
        return false;
    }

    msg::add("The thing closes.");

    is_open_ = false;

    if (mon)
    {
        mon->has_acted_this_tick_ = true;
    }

    return true;
}

void BumpOpen::operator()(Mon* const mon)
{
    ASSERT((bool)ter_.trait_openable_);

    if (ter_.trait_openable_)
    {
        ter_.trait_openable_->try_open(mon);
    }
}

void StepOffClose::operator()(Mon* const mon)
{
    ASSERT((bool)ter_.trait_openable_);

    if (ter_.trait_openable_)
    {
        ter_.trait_openable_->try_close(mon);
    }
}

bool DoorPhys::blocks() const
{
    ASSERT((bool)ter_.trait_openable_);

    if (ter_.trait_openable_)
    {
        return !ter_.trait_openable_->is_open();
    }

    return false;
}

RenderData DoorRender::render_d() const
{
    ASSERT((bool)ter_.trait_openable_);

    RenderData out;

    if (ter_.trait_openable_)
    {
        out.c   = ter_.trait_openable_->is_open() ? '_' : '+';
        out.clr = ter_.data_.render_d.clr;
    }

    return out;
}

RenderData ForceFieldRender::render_d() const
{
    ASSERT((bool)ter_.trait_openable_);

    RenderData out;

    if (ter_.trait_openable_)
    {
        out.c   = ter_.data_.render_d.c;
        out.clr = ter_.trait_openable_->is_open() ? clr_gray_drk : clr_red_lgt;
    }

    return out;
}

} // trait
} // ter

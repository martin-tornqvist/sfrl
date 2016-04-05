#include "ter.hpp"

#include <algorithm>

#include "map.hpp"

namespace ter
{

TerData data[(size_t)TerId::end];

//---------------------------------------------------------- Terrain data
void init()
{
    {
        // Floor
        TerData& d = data[(size_t)TerId::floor];
        d.render_d = RenderData('.', clr_gray);
        d.blocks = false;
    }
    {
        // Wall
        TerData& d = data[(size_t)TerId::wall];
        d.render_d.clr = clr_brown_drk;
        d.render_d.draw_as_wall = true;
        d.blocks = true;
    }
    {
        // Door
        TerData& d = data[(size_t)TerId::door];
        d.render_d.clr = clr_white;
    }
    {
        // Force field
        TerData& d = data[(size_t)TerId::force_field];
        d.render_d.c = 240;
    }
}

//---------------------------------------------------------- Terrain factory
std::unique_ptr<Ter> mk(TerId id, const P& p)
{
    TerData& d = data[(size_t)id];

    std::unique_ptr<Ter> ter(new Ter(d, id, p));

    // TODO: Trait configuration should perhaps be controlled by the data instead
    switch (id)
    {
    case TerId::door:
    {
        ter->trait_phys_        .reset(new trait::DoorPhys(*ter));
        ter->trait_render_      .reset(new trait::DoorRender(*ter));
        ter->trait_bump_        .reset(new trait::BumpOpen(*ter));
        ter->trait_step_off_    .reset(new trait::StepOffClose(*ter));
        ter->trait_openable_    .reset(new trait::Openable(*ter));
        auto test_cond = std::unique_ptr<trait::TerCond>(new trait::TestTerCond);
        ter->trait_openable_->set_open_cond(std::move(test_cond));
        ter->triggers_.emplace_back(new trait::AutoCloseTimer(*ter));
    }
    break;

    case TerId::force_field:
    {
        ter->trait_phys_        .reset(new trait::DoorPhys(*ter));
        ter->trait_render_      .reset(new trait::ForceFieldRender(*ter));
        ter->trait_bump_        .reset(new trait::BumpOpen(*ter));
        ter->trait_openable_    .reset(new trait::Openable(*ter));
    }
    break;

    default:
        break;
    }

    return ter;
}

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

        is_open_ = true;
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

    is_open_ = false;

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

Ter::Ter(TerData& data, TerId id, const P& p) :
    data_               (data),
    trait_phys_         (nullptr),
    trait_render_       (nullptr),
    trait_bump_         (nullptr),
    trait_step_off_     (nullptr),
    trait_openable_     (nullptr),
    triggers_           (),
    id_                 (id),
    p_                  (p) {}

RenderData Ter::render_d() const
{
    if (trait_render_)
    {
        return trait_render_->render_d();
    }

    return data_.render_d;
}

bool Ter::blocks() const
{
    if (trait_phys_)
    {
        return trait_phys_->blocks();
    }

    return data_.blocks;
}

void Ter::on_new_turn()
{
    for (auto& trigger : triggers_)
    {
        trigger->on_new_turn();
    }
}

void Ter::on_bump(Mon* const mon)
{
    if (trait_bump_)
    {
        (*trait_bump_)(mon);
    }
}

void Ter::on_step_off(Mon* const mon)
{
    if (trait_step_off_)
    {
        (*trait_step_off_)(mon);
    }
}

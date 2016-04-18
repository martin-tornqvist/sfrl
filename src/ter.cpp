#include "ter.hpp"

#include <algorithm>

#include "map.hpp"

namespace ter
{

TerData data[(size_t)TerId::end];

//-----------------------------------------------------------------------------
// Terrain data
//-----------------------------------------------------------------------------
void init()
{
    {
        // Floor
        TerData& d = data[(size_t)TerId::floor];
        d.render_d = RenderData('.', clr_green);
        d.blocks = false;
    }
    {
        // Wall
        TerData& d = data[(size_t)TerId::wall];
        d.render_d.clr = clr_blue;
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

//-----------------------------------------------------------------------------
// Terrain factory
//-----------------------------------------------------------------------------
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

#ifndef TER_HPP
#define TER_HPP

#include <memory>

#include "ter_trait.hpp"
#include "render.hpp"

class Ter;
class Mon;

enum class TerId
{
    floor,
    wall,
    door,
    force_field,
    end
};

struct TerData
{
    TerData() :
        render_d    (),
        blocks      (false) {}

    RenderData render_d;
    bool blocks;
};

namespace ter
{

extern TerData data[(size_t)TerId::end];

void init();

std::unique_ptr<Ter> mk(TerId id, const P& p);

} // ter

class Ter
{
public:
    Ter(TerData& data, TerId id, const P& p);

    Ter() = delete;

    Ter(const Ter&) = delete;

    Ter& operator=(const Ter&) = delete;

    ~Ter() {}

    TerId id() const
    {
        return id_;
    }

    const P& p() const
    {
        return p_;
    }

    RenderData render_d() const;

    bool blocks() const;

    void on_new_turn();

    void on_bump(Mon* const mon);
    void on_step_off(Mon* const mon);

    TerData& data_;

    std::unique_ptr<ter::trait::Phys>       trait_phys_;
    std::unique_ptr<ter::trait::Render>     trait_render_;
    std::unique_ptr<ter::trait::Bump>       trait_bump_;
    std::unique_ptr<ter::trait::StepOff>    trait_step_off_;
    std::unique_ptr<ter::trait::Openable>   trait_openable_;

    std::vector< std::unique_ptr<ter::trait::TerTrigger> > triggers_;

private:
    TerId id_;
    P p_;
};

#endif // TER_HPP

#ifndef TER_HPP
#define TER_HPP

#include <memory>

#include "render.hpp"

// TODO:
// * Consider splitting into multiple files, such as ter, ter_trait, ter_cond, and ter_trigger.

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

namespace trait
{

//---------------------------------------------------------- Conditions
class TerCond
{
public:
    TerCond() {}

    virtual ~TerCond() {}

    virtual bool run(Mon* const mon) = 0;
};

class TestTerCond: public TerCond
{
public:
    TestTerCond() :
        allow_(true) {}

    bool run(Mon* const mon) override
    {
        (void)mon;

        allow_ = !allow_;

        return allow_;
    }

private:
    bool allow_;
};

//---------------------------------------------------------- Triggers
class TerTrigger
{
public:
    TerTrigger(Ter& ter) :
        ter_(ter) {}

    virtual ~TerTrigger() {}

    // Events to listen for - derived classes should implement at least one of these,
    // then subsequently call "trigger"
    virtual void on_new_turn() {}
    virtual void on_open() {}
    virtual void on_close() {}

protected:
    // The derived trigger should call this on itself
    virtual void trigger() = 0;

    Ter& ter_;
};

class TimedTerTrigger : public TerTrigger
{
public:
    TimedTerTrigger(Ter& ter) :
        TerTrigger      (ter),
        nr_turns_left_  (-1) {}

    virtual ~TimedTerTrigger() {}

    void on_new_turn() override final
    {
        // NOTE: -1 number of turns left means we are idling

        if (nr_turns_left_ > 0)
        {
            // We are counting down
            --nr_turns_left_;
        }
        else if (nr_turns_left_ == 0)
        {
            // Time to run the trigger!
            trigger();

            nr_turns_left_ = -1;
        }
    }

protected:
    // This should be called by inherited classes, when starting countdown
    void set_timer(const int nr_turns)
    {
        nr_turns_left_ = nr_turns;
    }

private:
    int nr_turns_left_;
};

// Attempts to close the terrain a number of turns after it was opened
class AutoCloseTimer : public TimedTerTrigger
{
public:
    AutoCloseTimer(Ter& ter) :
        TimedTerTrigger(ter) {}

    void on_open() override
    {
        set_timer(5);
    }

protected:
    void trigger() override;
};

//---------------------------------------------------------- Base trait
class Trait
{
public:
    Trait(Ter& ter) :
        ter_(ter) {}

    virtual ~Trait() {}

protected:
    Ter& ter_;
};

//---------------------------------------------------------- Render traits
class Render : public Trait
{
public:
    Render(Ter& ter) :
        Trait(ter) {}

    virtual ~Render() {}

    virtual RenderData render_d() const = 0;
};

class DoorRender : public Render
{
public:
    DoorRender(Ter& ter) :
        Render(ter) {}

    RenderData render_d() const override;
};

class ForceFieldRender : public Render
{
public:
    ForceFieldRender(Ter& ter) :
        Render(ter) {}

    RenderData render_d() const override;
};

//---------------------------------------------------------- Physics traits
class Phys : public Trait
{
public:
    Phys(Ter& ter) :
        Trait(ter) {}

    virtual ~Phys() {}

    virtual bool blocks() const = 0;
};

class DoorPhys : public Phys
{
public:
    DoorPhys(Ter& ter) :
        Phys(ter) {}

    bool blocks() const override;
};

//---------------------------------------------------------- Openable traits
class Openable : public Trait
{
public:
    Openable(Ter& ter) :
        Trait       (ter),
        is_open_    (false),
        open_cond_  (nullptr) {}

    Openable(const Openable&) = delete;

    Openable& operator=(Openable&) = delete;

    bool try_open(Mon* const mon);

    bool try_close(Mon* const mon);

    bool is_open() const
    {
        return is_open_;
    }

    void set_open_cond(std::unique_ptr<TerCond>&& cond)
    {
        open_cond_ = std::move(cond);
    }

private:
    bool is_open_;

    std::unique_ptr<TerCond> open_cond_;
};

//---------------------------------------------------------- Bump traits
class Bump: public Trait
{
public:
    Bump(Ter& ter) :
        Trait(ter) {}

    virtual ~Bump() {}

    virtual void operator()(Mon* const mon) = 0;
};

class BumpOpen : public Bump
{
public:
    BumpOpen(Ter& ter) :
        Bump(ter) {}

    void operator()(Mon* const mon) override;
};

//---------------------------------------------------------- Step off traits
class StepOff: public Trait
{
public:
    StepOff(Ter& ter) :
        Trait(ter) {}

    virtual ~StepOff() {}

    virtual void operator()(Mon* const mon) = 0;
};

class StepOffClose : public StepOff
{
public:
    StepOffClose(Ter& ter) :
        StepOff(ter) {}

    void operator()(Mon* const mon) override;
};

} // trait

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

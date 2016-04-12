#ifndef AI_HPP
#define AI_HPP

class Mon;

class MonCtl
{
public:
    MonCtl(Mon& mon) :
        mon_(&mon) {}

    MonCtl(const MonCtl&) = delete;

    MonCtl& operator=(const MonCtl&) = delete;

    virtual ~MonCtl() {}

    virtual void act() = 0;

protected:
    Mon* const mon_;
};

class Ai : public MonCtl
{
public:
    Ai(Mon& mon) :
        MonCtl(mon) {}

    void act() override;
};

class PlayerMonCtl : public MonCtl
{
public:
    PlayerMonCtl(Mon& mon) :
        MonCtl(mon) {}

    void act() override;
};

#endif // AI_HPP

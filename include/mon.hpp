#ifndef MON_HPP
#define MON_HPP

#include "rl_utils.hpp"

class Mon
{
public:
    Mon(const P& p) :
        p_(p) {}

    ~Mon() {}

    void mv(const Dir dir);

    const P& p() const
    {
        return p_;
    }

private:
    P p_;
};

#endif // MON_HPP

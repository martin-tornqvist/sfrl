#ifndef UTILS_HPP
#define UTILS_HPP

// -----------------------------------------------------------------------------
// Direction
// -----------------------------------------------------------------------------
enum class Dir
{
    right,
    left,
    down,
    up,
    down_right,
    up_right,
    down_left,
    up_left,
}

// -----------------------------------------------------------------------------
// Position
// -----------------------------------------------------------------------------
struct P
{
    P() :
        x(0),
        y(0) {}

    P offs(int v)
    {
        return P(x + v, y + v);
    }

    bool operator==(const P& p)
    {
        return x == p.x && y == p.y;
    }

    P operator+(const P& p)
    {
        return P(x + p.x, y + p.y);
    }
    
    P operator-(const P& p)
    {
        return P(x - p.x, y - p.y);
    }

    int x, y;
};

void p_offset(const Dir dir, P& p);

P to_offset(const Dir dir);

// -----------------------------------------------------------------------------
// Rectangle
// -----------------------------------------------------------------------------
pub struct R
{
    R() :
        p0(),
        p1() {}

    R(const P& p0, const P& p1) :
        p0(p0),
        p1(p1) {}

    bool operator==(const Rect& r)
    {
        return p0 == r.p0 && p1 == r.p1;
    }

    P p0, p1;
}

#endif // UTILS_HPP

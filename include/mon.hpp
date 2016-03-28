use utils::*;

class Mon
{
public:
    Mon() :
        p_() {}
    
    ~Mon() {}

    void mv(const Dir dir);
    
    const P& p()
    {
        return p_;
    }

private:
    P p_;
};

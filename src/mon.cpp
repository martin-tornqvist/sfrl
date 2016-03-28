use utils::*;


void Mon::mv(const Dir dir)
{
    const d = P;
    
    p_offset(dir, d);
    
    p_ = p_ + d;
}

#include "utils.hpp"

// -----------------------------------------------------------------------------
// Position
// -----------------------------------------------------------------------------
void p_offset(const Dir dir, P& p)
{
    const P d = to_offset(dir);

    p = p + d;
}

P to_offset(dir: Dir)
{
    switch dir
    {
    case Dir::right:      return P( 1,  0);
    case Dir::left:       return P(-1,  0);
    case Dir::down:       return P( 0,  1);
    case Dir::up:         return P( 0, -1);
    case Dir::down_right: return P( 1,  1);
    case Dir::up_right:   return P( 1, -1);
    case Dir::down_left:  return P(-1,  1);
    case Dir::up_left:    return P(-1, -1);
    }

    return P();
}

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------
// #[cfg(test)]
// mod tests {
//     use super::*;

//     #[test]
//     fn test_p()
//     {
//         // Verify copy
//         let p0 = P::new_xy(3, 5);

//         let p1 = p0;

//         assert_eq!(p0.x, p1.x);
//         assert_eq!(p0.y, p1.y);

//         // Verify comparison
//         assert!(p0 == p1);

//         // Verify offset by value
//         let p2 = p0.offs(100);

//         assert_eq!(103, p2.x);
//         assert_eq!(105, p2.y);

//         // Verify addition
//         let p3 = p0 + P::new_xy(200, 300);

//         assert_eq!(203, p3.x);
//         assert_eq!(305, p3.y);

//         // Verify subtraction
//         let p4 = p0 - P::new_xy(10, 1);

//         assert_eq!(-7, p4.x);
//         assert_eq!(4, p4.y);
//     }
// }

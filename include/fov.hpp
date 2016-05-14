#ifndef FOV_HPP
#define FOV_HPP

#include "rl_utils.hpp"
#include "map.hpp"

struct FovData
{
    FovData() :
        blocked_hard    (false),
        blocked_by_dark (false) {}

    bool blocked_hard;
    bool blocked_by_dark;
};

namespace fov
{

const int los_base_range = 12;

// Update current FOV and exploration
void update_player_fov();

// NOTE: The result array is not cleared by this function prior to running FOV
// on it, this is the caller's responsibility (if desired)
void run(const P& p, const R& area, FovData out[map_w][map_h]);

void los(const P& p0, const P& p1, FovData& out);

} // fov

#endif // FOV_HPP

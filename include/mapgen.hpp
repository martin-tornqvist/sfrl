#ifndef MAPGEN_HPP
#define MAPGEN_HPP

#include <vector>
#include <memory>

#include "rl_utils.hpp"

enum class Axis
{
    hor,
    ver
};

class BTree
{
public:
    BTree(const R& r = R()) :
        area_   (r),
        child1_ (nullptr),
        child2_ (nullptr) {}

    ~BTree() {}

    // Splits this node along the given axis, and creates two sub trees
    // (which are then leaf nodes).
    // The "len" parameter is the resulting width or height (depending on axis)
    // of the first child. The second child gets the remaining total length.
    void split(const Axis axis, const int len);

    // Area of the game map that this node covers
    R area_;

    // NOTE: The child trees should always either:
    // * BOTH be null, or
    // * BOTH point to their own existing trees
    std::unique_ptr<BTree> child1_;
    std::unique_ptr<BTree> child2_;
};

namespace mapgen
{

void run();

} // namespace

#endif // MAPGEN_HPP

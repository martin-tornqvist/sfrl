#include "mapgen.hpp"

#include "map.hpp"
#include "ter.hpp"

void BTree::split(const Axis axis, const int len)
{
    // We never want to split a non-leaf node
    ASSERT(!child1_);
    ASSERT(!child2_);

    ASSERT(len > 0);

    R a1(area_);
    R a2(area_);

    if (axis == Axis::ver)
    {
        const int w_tot = area_.w();

        ASSERT(len < w_tot);

        a1.p1.x = area_.p0.x + len - 1;
        a2.p0.x = a1.p1.x + 1;

        ASSERT((a1.w() + a2.w()) == area_.w());
    }
    else // Horizontal split
    {
        const int h_tot = area_.h();

        ASSERT(len < h_tot);

        a1.p1.y = area_.p0.y + len - 1;
        a2.p0.y = a1.p1.y + 1;

        ASSERT((a1.h() + a2.h()) == area_.h());
    }

    child1_.reset(new BTree(a1));
    child2_.reset(new BTree(a2));
}

namespace mapgen
{

namespace
{

const int room_min_len = 2;

void mk_room(const R& a)
{
    ASSERT(a.min_dim() >= room_min_len);

    for (int x = a.p0.x; x <= a.p1.x; ++x)
    {
        for (int y = a.p0.y; y <= a.p1.y; ++y)
        {
            map::ter[x][y] = ter::mk(TerId::floor, P(x, y));
        }
    }
}

void run_bsp(BTree& tree, std::vector<BTree*>& leafs)
{
    // See NOTE at the top of the "run" function below for critical information
    // regarding room placement rules.

    ASSERT(!tree.child1_ && !tree.child2_);

    ASSERT(tree.area_.min_dim() >= room_min_len);

    const int part_min_len_splittable   = (room_min_len * 2) + 2;
    const int w                         = tree.area_.w();
    const int h                         = tree.area_.h();

    // We always try to split the longer side
    const Axis axis = (w > h) ? Axis::ver : Axis::hor;

    const int len = axis == Axis::ver ? w : h;

    // We do not always split partitions as much as possible - sometimes make
    // the partition a leaf node, eventhough it could have been split more.
    const int part_min_len_force_split = 39;

    Fraction chance_skip_split(1, 3);

    const bool split_ok =
        (len >= part_min_len_splittable) &&
        (len >= part_min_len_force_split || !chance_skip_split.roll());

    if (split_ok)
    {
        // Node is splittable
        // TODO: It would probably be best to split with a normal distribution
        // from center.
        const int split_len = rnd::range(room_min_len + 1, len - room_min_len - 1);

        tree.split(axis, split_len);

        ASSERT(tree.child1_ && tree.child2_);

        run_bsp(*tree.child1_, leafs);
        run_bsp(*tree.child2_, leafs);
    }
    else // No split to be done, this is now permanently a leaf node
    {
        leafs.push_back(&tree);
    }
}

} // namespace

void run()
{
    // NOTE: Rooms are allowed to overlap the left and upper borders of the
    // partitions, but NOT the right and lower borders. This is so that two
    // rooms in adjacent partitions can be placed one wall cell apart.
    // This rule must be enforced throughout the map generation.
    // (If rooms were allowed to overlap ALL partition boundaries, then some
    // rooms would have adjacent floor cells. If they were not allowed to
    // overlap ANY boundaries, then two rooms could never be closer than
    // two wall cells apart.)

    // The root partition overlap the right and lower boundaries of the map -
    // these boundaries will be forbidden for rooms on partition level.
    BTree root(R(1, 1, map_w - 1, map_h - 1));

    std::vector<BTree*> leafs;

    run_bsp(root, leafs);

    for (BTree* leaf : leafs)
    {
//        if (!rnd::one_in(3))
//        {
//            continue;
//        }

        R usable_area(leaf->area_);

        // Forbid right and lower partition boundaries
        --usable_area.p1;

        const Range w_range(room_min_len, usable_area.w());
        const Range h_range(room_min_len, usable_area.h());

        // TODO: Normal distribution is probably better here
        // TODO: There should be some zones were we explicitly set all rooms to
        // max size - this would create a sort of "warehouse" look if the rooms
        // are big, or an "office" look if the rooms are small.
        const P dims(w_range.roll(), h_range.roll());

        const P xy_min(usable_area.p0);
        const P xy_max(usable_area.p1 - dims + 1);

        const Range x_range(xy_min.x, xy_max.x);
        const Range y_range(xy_min.y, xy_max.y);

        const P room_x0y0(x_range.roll(), y_range.roll());
        const P room_x1y1(room_x0y0 + dims - 1);

        const R room_area(room_x0y0, room_x1y1);

        mk_room(room_area);
    }

    // Make "constructed" walls around floor
    for (int x = 1; x < map_w - 1; ++x)
    {
        for (int y = 1; y < map_h - 1; ++y)
        {
            if (map::ter[x][y]->id() != TerId::rock_wall)
            {
                continue;
            }

            const P p(x, y);

            bool is_adj_to_floor = false;

            for (const P& d : dir_utils::dir_list)
            {
                const P p_adj(p + d);

                if (map::ter[p_adj.x][p_adj.y]->id() == TerId::floor)
                {
                    is_adj_to_floor = true;
                    break;
                }
            }

            if (is_adj_to_floor)
            {
                map::ter[p.x][p.y] = ter::mk(TerId::wall, p);
            }
        }
    }
}

} // namespace

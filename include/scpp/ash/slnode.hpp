/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ASH_SLNODE_HPP
#define SCPP_ASH_SLNODE_HPP

#include <sc/ash/slnode.h>

namespace sc {

/**
 * @addtogroup Node
 * @{
 */

template <typename NodeT>
struct NodeTraits;

/** @} */

/**
 * @addtogroup SlNode
 * @{
 */

template<>
struct NodeTraits<ScSlNode> {
    typedef ScSlNode Node;
    static Node*
    next(Node* node) noexcept
    {
        return node->next;
    }
    static const Node*
    next(const Node* node) noexcept
    {
        return node->next;
    }
};

/** @} */

} // sc

#endif // SCPP_ASH_SLNODE_HPP

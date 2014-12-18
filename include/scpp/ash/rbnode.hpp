/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ASH_RBNODE_HPP
#define SCPP_ASH_RBNODE_HPP

#include <sc/ash/rbnode.h>

namespace sc {

/**
 * @addtogroup Node
 * @{
 */

template <typename NodeT>
struct NodeTraits;

/** @} */

/**
 * @addtogroup RbNode
 * @{
 */

template<>
struct NodeTraits<ScRbNode> {
    typedef ScRbNode Node;
    static Node*
    next(Node* node) noexcept
    {
        return sc_rbnode_next(node);
    }
    static const Node*
    next(const Node* node) noexcept
    {
        return sc_rbnode_next(const_cast<Node*>(node));
    }
    static Node*
    prev(Node* node) noexcept
    {
        return sc_rbnode_prev(node);
    }
    static const Node*
    prev(const Node* node) noexcept
    {
        return sc_rbnode_prev(const_cast<Node*>(node));
    }
};

/** @} */

} // sc

#endif // SCPP_ASH_RBNODE_HPP

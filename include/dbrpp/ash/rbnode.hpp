/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_ASH_RBNODE_HPP
#define DBRPP_ASH_RBNODE_HPP

#include <dbr/ash/rbnode.h>

namespace dbr {

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
struct NodeTraits<DbrRbNode> {
    typedef DbrRbNode Node;
    static Node*
    next(Node* node) noexcept
    {
        return dbr_rbnode_next(node);
    }
    static const Node*
    next(const Node* node) noexcept
    {
        return dbr_rbnode_next(const_cast<Node*>(node));
    }
    static Node*
    prev(Node* node) noexcept
    {
        return dbr_rbnode_prev(node);
    }
    static const Node*
    prev(const Node* node) noexcept
    {
        return dbr_rbnode_prev(const_cast<Node*>(node));
    }
};

/** @} */

} // dbr

#endif // DBRPP_ASH_RBNODE_HPP

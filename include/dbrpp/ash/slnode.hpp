/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_ASH_SLNODE_HPP
#define DBRPP_ASH_SLNODE_HPP

#include <dbr/ash/slnode.h>

namespace dbr {

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
struct NodeTraits<DbrSlNode> {
    typedef DbrSlNode Node;
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

} // dbr

#endif // DBRPP_ASH_SLNODE_HPP

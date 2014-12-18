/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ASH_DLNODE_HPP
#define SCPP_ASH_DLNODE_HPP

#include <sc/ash/dlnode.h>

namespace sc {

/**
 * @addtogroup Node
 * @{
 */

template <typename NodeT>
struct NodeTraits;

/** @} */

/**
 * @addtogroup DlNode
 * @{
 */

template<>
struct NodeTraits<ScDlNode> {
    typedef ScDlNode Node;
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
    static Node*
    prev(Node* node) noexcept
    {
        return node->prev;
    }
    static const Node*
    prev(const Node* node) noexcept
    {
        return node->prev;
    }
};

/** @} */

} // sc

#endif // SCPP_ASH_DLNODE_HPP

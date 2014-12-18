/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_ASH_DLNODE_HPP
#define DBRPP_ASH_DLNODE_HPP

#include <dbr/ash/dlnode.h>

namespace dbr {

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
struct NodeTraits<DbrDlNode> {
    typedef DbrDlNode Node;
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

} // dbr

#endif // DBRPP_ASH_DLNODE_HPP

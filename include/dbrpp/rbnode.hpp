/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
 *
 *  This file is part of Doobry written by Mark Aylett.
 *
 *  Doobry is free software; you can redistribute it and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Doobry is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program; if
 *  not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA.
 */
#ifndef DBRPP_RBNODE_HPP
#define DBRPP_RBNODE_HPP

#include <dbr/rbnode.h>

namespace dbr {

/**
 * @addtogroup Node
 * @{
 */

template <typename NodeT>
struct NodeTraits;

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

#endif // DBRPP_RBNODE_HPP

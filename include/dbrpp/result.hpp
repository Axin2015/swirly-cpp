/*
 *  Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
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
#ifndef DBRPP_RESULT_HPP
#define DBRPP_RESULT_HPP

#include <dbrpp/iter.hpp>
#include <dbrpp/order.hpp>
#include <dbrpp/slnode.hpp>
#include <dbrpp/trade.hpp>

#include <limits>

namespace dbr {

class ResultOrders {
    struct Policy : NodeTraits<DbrSlNode> {
        typedef DbrOrder Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_result_order_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_result_order_entry(const_cast<Node*>(node));
        }
    };
    DbrResult result_;
public:
    typedef Policy::Entry ValueType;
    typedef Policy::Entry* Pointer;
    typedef Policy::Entry& Reference;
    typedef const Policy::Entry* ConstPointer;
    typedef const Policy::Entry& ConstReference;

    typedef ForwardIterator<Policy> Iterator;
    typedef ConstForwardIterator<Policy> ConstIterator;

    typedef std::ptrdiff_t DifferenceType;
    typedef size_t SizeType;

    // Standard typedefs.

    typedef ValueType value_type;
    typedef Pointer pointer;
    typedef Reference reference;
    typedef ConstPointer const_pointer;
    typedef ConstReference const_reference;

    typedef Iterator iterator;
    typedef ConstIterator const_iterator;

    typedef DifferenceType difference_type;
    typedef DifferenceType distance_type;
    typedef SizeType size_type;

    explicit
    ResultOrders(DbrResult result) noexcept
    : result_(result)
    {
    }
    void
    swap(ResultOrders& rhs) noexcept
    {
        std::swap(result_, rhs.result_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return result_.first_order;
    }
    ConstIterator
    begin() const noexcept
    {
        return result_.first_order;
    }
    Iterator
    end() noexcept
    {
        return nullptr;
    }
    ConstIterator
    end() const noexcept
    {
        return nullptr;
    }

    // Accessor.

    Reference
    front() noexcept
    {
        return *begin();
    }
    ConstReference
    front() const noexcept
    {
        return *begin();
    }
    SizeType
    size() const noexcept
    {
        return std::distance(begin(), end());
    }
    SizeType
    max_size() const noexcept
    {
        return std::numeric_limits<SizeType>::max();
    }
    bool
    empty() const noexcept
    {
        return result_.first_order == nullptr;
    }
};

class ResultTrades {
    struct Policy : NodeTraits<DbrSlNode> {
        typedef DbrOrder Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_result_order_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_result_order_entry(const_cast<Node*>(node));
        }
    };
    DbrResult result_;
public:
    typedef Policy::Entry ValueType;
    typedef Policy::Entry* Pointer;
    typedef Policy::Entry& Reference;
    typedef const Policy::Entry* ConstPointer;
    typedef const Policy::Entry& ConstReference;

    typedef ForwardIterator<Policy> Iterator;
    typedef ConstForwardIterator<Policy> ConstIterator;

    typedef std::ptrdiff_t DifferenceType;
    typedef size_t SizeType;

    // Standard typedefs.

    typedef ValueType value_type;
    typedef Pointer pointer;
    typedef Reference reference;
    typedef ConstPointer const_pointer;
    typedef ConstReference const_reference;

    typedef Iterator iterator;
    typedef ConstIterator const_iterator;

    typedef DifferenceType difference_type;
    typedef DifferenceType distance_type;
    typedef SizeType size_type;

    explicit
    ResultTrades(DbrResult result) noexcept
    : result_(result)
    {
    }
    void
    swap(ResultTrades& rhs) noexcept
    {
        std::swap(result_, rhs.result_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return result_.first_order;
    }
    ConstIterator
    begin() const noexcept
    {
        return result_.first_order;
    }
    Iterator
    end() noexcept
    {
        return nullptr;
    }
    ConstIterator
    end() const noexcept
    {
        return nullptr;
    }

    // Accessor.

    Reference
    front() noexcept
    {
        return *begin();
    }
    ConstReference
    front() const noexcept
    {
        return *begin();
    }
    SizeType
    size() const noexcept
    {
        return std::distance(begin(), end());
    }
    SizeType
    max_size() const noexcept
    {
        return std::numeric_limits<SizeType>::max();
    }
    bool
    empty() const noexcept
    {
        return result_.first_order == nullptr;
    }
};

class Result {
    DbrResult impl_;
public:
    operator DbrResult&() noexcept
    {
        return impl_;
    }
    DbrResult*
    c_arg() noexcept
    {
        return &impl_;
    }
    OrderRef
    new_order() const noexcept
    {
        return OrderRef(*impl_.new_order);
    }
    ResultOrders
    orders() const noexcept
    {
        return ResultOrders(impl_);
    }
    ResultTrades
    trades() const noexcept
    {
        return ResultTrades(impl_);
    }
};

} // dbr

#endif // DBRPP_RESULT_HPP

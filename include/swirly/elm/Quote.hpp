/*
 * Swirly Order-Book and Matching-Engine.
 * Copyright (C) 2013, 2015 Swirly Cloud Limited.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#ifndef SWIRLY_ELM_QUOTE_HPP
#define SWIRLY_ELM_QUOTE_HPP

#include <swirly/elm/Request.hpp>

#include <boost/intrusive/list.hpp>

namespace swirly {

/**
 * @addtogroup Entity
 * @{
 */

class SWIRLY_API Quote : public Request {

    OrderPtr order_;
    const Ticks ticks_;
    const Millis expiry_;

 public:
    boost::intrusive::set_member_hook<> idHook_;

    Quote(const StringView& trader, const StringView& market, const StringView& contr,
          Jday settlDay, Iden id, const StringView& ref, const OrderPtr& order, Side side,
          Lots lots, Ticks ticks, Millis created, Millis expiry) noexcept;

    ~Quote() noexcept override;

    // Copy.
    Quote(const Quote&) = delete;
    Quote& operator =(const Quote&) = delete;

    // Move.
    Quote(Quote&&);
    Quote& operator =(Quote&&) = delete;

    void clearOrder() noexcept;

    const OrderPtr& order() const noexcept
    {
        return order_;
    }
    Ticks ticks() const noexcept
    {
        return ticks_;
    }
    Millis expiry() const noexcept
    {
        return expiry_;
    }
};

using QuotePtr = boost::intrusive_ptr<Quote>;
using QuoteIdSet = RequestIdSet<Quote>;

/** @} */

} // swirly

#endif // SWIRLY_ELM_QUOTE_HPP

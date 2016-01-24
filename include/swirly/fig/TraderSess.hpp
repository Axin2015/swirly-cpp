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
#ifndef SWIRLY_ELM_TRADERSESS_HPP
#define SWIRLY_ELM_TRADERSESS_HPP

#include <swirly/elm/Exec.hpp>
#include <swirly/elm/Order.hpp>
#include <swirly/elm/Posn.hpp>
#include <swirly/elm/Quote.hpp>
#include <swirly/elm/Trader.hpp>

#include <boost/intrusive/unordered_set.hpp>

namespace swirly {

class Factory;

/**
 * @addtogroup Entity
 * @{
 */

class SWIRLY_API TraderSess : public Trader {
    using LinkModeOption = boost::intrusive::link_mode<boost::intrusive::auto_unlink>;

    const Factory& factory_;
    OrderIdSet orders_;
    ExecIdSet trades_;
    TraderPosnSet posns_;
    QuoteIdSet quotes_;

 public:
    boost::intrusive::unordered_set_member_hook<LinkModeOption> emailHook_;

    TraderSess(const StringView& mnem, const StringView& display, const StringView& email,
               const Factory& factory) noexcept
    :   Trader{mnem, display, email},
        factory_{factory}
    {
    }

    ~TraderSess() noexcept override;

    // Copy.
    TraderSess(const TraderSess&) = delete;
    TraderSess& operator =(const TraderSess&) = delete;

    // Move.
    TraderSess(TraderSess&&);
    TraderSess& operator =(TraderSess&&) = delete;

    PosnPtr lazyPosn(const StringView& contr, Jday settlDay) throw (std::bad_alloc);
};

/** @} */

} // swirly

#endif // SWIRLY_ELM_TRADERSESS_HPP

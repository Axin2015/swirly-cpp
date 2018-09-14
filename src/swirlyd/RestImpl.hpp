/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
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
#ifndef SWIRLYD_RESTIMPL_HPP
#define SWIRLYD_RESTIMPL_HPP

#include "EntitySet.hpp"
#include "Page.hpp"

#include <swirly/lob/App.hpp>

namespace swirly {

class SWIRLY_API RestImpl {
  public:
    RestImpl(MsgQueue& mq, std::size_t max_execs)
    : app_{mq, max_execs}
    {
    }
    ~RestImpl();

    // Copy.
    RestImpl(const RestImpl&) = delete;
    RestImpl& operator=(const RestImpl&) = delete;

    // Move.
    RestImpl(RestImpl&&);
    RestImpl& operator=(RestImpl&&);

    void load(Time now, const Model& model) { app_.load(now, model); }

    void get_ref_data(Time now, EntitySet es, std::ostream& out) const;

    void get_asset(Time now, std::ostream& out) const;

    void get_asset(Time now, Symbol symbol, std::ostream& out) const;

    void get_instr(Time now, std::ostream& out) const;

    void get_instr(Time now, Symbol symbol, std::ostream& out) const;

    void get_sess(Time now, Symbol accnt, EntitySet es, Page page, std::ostream& out) const;

    void get_market(Time now, std::ostream& out) const;

    void get_market(Time now, Symbol instr, std::ostream& out) const;

    void get_market(Time now, Symbol instr, IsoDate settl_date, std::ostream& out) const;

    void get_order(Time now, Symbol accnt, std::ostream& out) const;

    void get_order(Time now, Symbol accnt, Symbol instr, std::ostream& out) const;

    void get_order(Time now, Symbol accnt, Symbol instr, IsoDate settl_date,
                   std::ostream& out) const;

    void get_order(Time now, Symbol accnt, Symbol instr, IsoDate settl_date, Id64 id,
                   std::ostream& out) const;

    void get_exec(Time now, Symbol accnt, Page page, std::ostream& out) const;

    void get_trade(Time now, Symbol accnt, std::ostream& out) const;

    void get_trade(Time now, Symbol accnt, Symbol instr, std::ostream& out) const;

    void get_trade(Time now, Symbol accnt, Symbol instr, IsoDate settl_date,
                   std::ostream& out) const;

    void get_trade(Time now, Symbol accnt, Symbol instr, IsoDate settl_date, Id64 id,
                   std::ostream& out) const;

    void get_posn(Time now, Symbol accnt, std::ostream& out) const;

    void get_posn(Time now, Symbol accnt, Symbol instr, std::ostream& out) const;

    void get_posn(Time now, Symbol accnt, Symbol instr, IsoDate settl_date,
                  std::ostream& out) const;

    void post_market(Time now, Symbol instr, IsoDate settl_date, MarketState state,
                     std::ostream& out);

    void put_market(Time now, Symbol instr, IsoDate settl_date, MarketState state,
                    std::ostream& out);

    void post_order(Time now, Symbol accnt, Symbol instr, IsoDate settl_date, std::string_view ref,
                    Side side, Lots lots, Ticks ticks, Lots min_lots, std::ostream& out);

    void put_order(Time now, Symbol accnt, Symbol instr, IsoDate settl_date, ArrayView<Id64> ids,
                   Lots lots, std::ostream& out);

    void post_trade(Time now, Symbol accnt, Symbol instr, IsoDate settl_date, std::string_view ref,
                    Side side, Lots lots, Ticks ticks, LiqInd liq_ind, Symbol cpty,
                    std::ostream& out);

    void delete_trade(Time now, Symbol accnt, Symbol instr, IsoDate settl_date,
                      ArrayView<Id64> ids);

  private:
    LobApp app_;
};

} // namespace swirly

#endif // SWIRLYD_RESTIMPL_HPP

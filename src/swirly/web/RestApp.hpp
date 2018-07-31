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
#ifndef SWIRLY_WEB_RESTAPP_HPP
#define SWIRLY_WEB_RESTAPP_HPP

#include <swirly/web/EntitySet.hpp>
#include <swirly/web/Page.hpp>

#include <swirly/lob/App.hpp>

namespace swirly {
inline namespace web {

class SWIRLY_API RestApp {
  public:
    RestApp(MsgQueue& mq, std::size_t max_execs)
    : app_{mq, max_execs}
    {
    }
    ~RestApp();

    // Copy.
    RestApp(const RestApp&) = delete;
    RestApp& operator=(const RestApp&) = delete;

    // Move.
    RestApp(RestApp&&);
    RestApp& operator=(RestApp&&);

    void load(const Model& model, Time now) { app_.load(model, now); }

    void get_ref_data(EntitySet es, Time now, std::ostream& out) const;

    void get_asset(Time now, std::ostream& out) const;

    void get_asset(Symbol symbol, Time now, std::ostream& out) const;

    void get_instr(Time now, std::ostream& out) const;

    void get_instr(Symbol symbol, Time now, std::ostream& out) const;

    void get_sess(Symbol accnt, EntitySet es, Page page, Time now, std::ostream& out) const;

    void get_market(Time now, std::ostream& out) const;

    void get_market(Symbol instr, Time now, std::ostream& out) const;

    void get_market(Symbol instr, IsoDate settl_date, Time now, std::ostream& out) const;

    void get_order(Symbol accnt, Time now, std::ostream& out) const;

    void get_order(Symbol accnt, Symbol instr, Time now, std::ostream& out) const;

    void get_order(Symbol accnt, Symbol instr, IsoDate settl_date, Time now,
                   std::ostream& out) const;

    void get_order(Symbol accnt, Symbol instr, IsoDate settl_date, Id64 id, Time now,
                   std::ostream& out) const;

    void get_exec(Symbol accnt, Page page, Time now, std::ostream& out) const;

    void get_trade(Symbol accnt, Time now, std::ostream& out) const;

    void get_trade(Symbol accnt, Symbol instr, Time now, std::ostream& out) const;

    void get_trade(Symbol accnt, Symbol instr, IsoDate settl_date, Time now,
                   std::ostream& out) const;

    void get_trade(Symbol accnt, Symbol instr, IsoDate settl_date, Id64 id, Time now,
                   std::ostream& out) const;

    void get_posn(Symbol accnt, Time now, std::ostream& out) const;

    void get_posn(Symbol accnt, Symbol instr, Time now, std::ostream& out) const;

    void get_posn(Symbol accnt, Symbol instr, IsoDate settl_date, Time now,
                  std::ostream& out) const;

    void post_market(Symbol instr, IsoDate settl_date, MarketState state, Time now,
                     std::ostream& out);

    void put_market(Symbol instr, IsoDate settl_date, MarketState state, Time now,
                    std::ostream& out);

    void post_order(Symbol accnt, Symbol instr, IsoDate settl_date, std::string_view ref, Side side,
                    Lots lots, Ticks ticks, Lots min_lots, Time now, std::ostream& out);

    void put_order(Symbol accnt, Symbol instr, IsoDate settl_date, ArrayView<Id64> ids, Lots lots,
                   Time now, std::ostream& out);

    void post_trade(Symbol accnt, Symbol instr, IsoDate settl_date, std::string_view ref, Side side,
                    Lots lots, Ticks ticks, LiqInd liq_ind, Symbol cpty, Time now,
                    std::ostream& out);

    void delete_trade(Symbol accnt, Symbol instr, IsoDate settl_date, ArrayView<Id64> ids,
                      Time now);

  private:
    App app_;
};

} // namespace web
} // namespace swirly

#endif // SWIRLY_WEB_RESTAPP_HPP

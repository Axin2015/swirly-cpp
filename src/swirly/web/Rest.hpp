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
#ifndef SWIRLY_WEB_REST_HPP
#define SWIRLY_WEB_REST_HPP

#include <swirly/web/EntitySet.hpp>
#include <swirly/web/Page.hpp>

#include <swirly/clob/Serv.hpp>

namespace swirly {
inline namespace web {

class SWIRLY_API Rest {
  public:
    Rest(Journ& journ, std::size_t pipeCapacity, std::size_t maxExecs)
    : serv_{journ, pipeCapacity, maxExecs}
    {
    }
    ~Rest();

    // Copy.
    Rest(const Rest&) = delete;
    Rest& operator=(const Rest&) = delete;

    // Move.
    Rest(Rest&&);
    Rest& operator=(Rest&&);

    void load(const Model& model, Time now) { serv_.load(model, now); }

    void getRefData(EntitySet es, Time now, std::ostream& out) const;

    void getAsset(Time now, std::ostream& out) const;

    void getAsset(Symbol symbol, Time now, std::ostream& out) const;

    void getInstr(Time now, std::ostream& out) const;

    void getInstr(Symbol symbol, Time now, std::ostream& out) const;

    void getAccnt(Symbol symbol, EntitySet es, Page page, Time now, std::ostream& out) const;

    void getMarket(Time now, std::ostream& out) const;

    void getMarket(Symbol instrSymbol, Time now, std::ostream& out) const;

    void getMarket(Symbol instrSymbol, IsoDate settlDate, Time now, std::ostream& out) const;

    void getOrder(Symbol accntSymbol, Time now, std::ostream& out) const;

    void getOrder(Symbol accntSymbol, Symbol instrSymbol, Time now, std::ostream& out) const;

    void getOrder(Symbol accntSymbol, Symbol instrSymbol, IsoDate settlDate, Time now,
                  std::ostream& out) const;

    void getOrder(Symbol accntSymbol, Symbol instrSymbol, IsoDate settlDate, Id64 id, Time now,
                  std::ostream& out) const;

    void getExec(Symbol accntSymbol, Page page, Time now, std::ostream& out) const;

    void getTrade(Symbol accntSymbol, Time now, std::ostream& out) const;

    void getTrade(Symbol accntSymbol, Symbol instrSymbol, Time now, std::ostream& out) const;

    void getTrade(Symbol accntSymbol, Symbol instrSymbol, IsoDate settlDate, Time now,
                  std::ostream& out) const;

    void getTrade(Symbol accntSymbol, Symbol instrSymbol, IsoDate settlDate, Id64 id, Time now,
                  std::ostream& out) const;

    void getPosn(Symbol accntSymbol, Time now, std::ostream& out) const;

    void getPosn(Symbol accntSymbol, Symbol instrSymbol, Time now, std::ostream& out) const;

    void getPosn(Symbol accntSymbol, Symbol instrSymbol, IsoDate settlDate, Time now,
                 std::ostream& out) const;

    void postMarket(Symbol instrSymbol, IsoDate settlDate, MarketState state, Time now,
                    std::ostream& out);

    void putMarket(Symbol instrSymbol, IsoDate settlDate, MarketState state, Time now,
                   std::ostream& out);

    void postOrder(Symbol accntSymbol, Symbol instrSymbol, IsoDate settlDate, std::string_view ref,
                   Side side, Lots lots, Ticks ticks, Lots minLots, Time now, std::ostream& out);

    void putOrder(Symbol accntSymbol, Symbol instrSymbol, IsoDate settlDate, ArrayView<Id64> ids,
                  Lots lots, Time now, std::ostream& out);

    void postTrade(Symbol accntSymbol, Symbol instrSymbol, IsoDate settlDate, std::string_view ref,
                   Side side, Lots lots, Ticks ticks, LiqInd liqInd, Symbol cpty, Time now,
                   std::ostream& out);

    void deleteTrade(Symbol accntSymbol, Symbol instrSymbol, IsoDate settlDate, ArrayView<Id64> ids,
                     Time now);

  private:
    Serv serv_;
};

} // namespace web
} // namespace swirly

#endif // SWIRLY_WEB_REST_HPP

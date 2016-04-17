/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
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
#include <swirly/fir/Rest.hpp>

#include <swirly/fig/Response.hpp>
#include <swirly/fig/TraderSess.hpp>

#include <swirly/elm/Exception.hpp>
#include <swirly/elm/MarketBook.hpp>

#include <swirly/ash/JulianDay.hpp>

using namespace std;

namespace swirly {

Rest::~Rest() noexcept = default;

Rest::Rest(Rest&&) = default;

Rest& Rest::operator=(Rest&&) = default;

void Rest::getRec(EntitySet es, Millis now, ostream& out) const
{
  int i{0};
  out << '{';
  if (es.asset()) {
    out << "\"assets\":";
    getAsset(now, out);
    ++i;
  }
  if (es.contr()) {
    if (i > 0) {
      out << ',';
    }
    out << "\"contrs\":";
    getContr(now, out);
    ++i;
  }
  if (es.market()) {
    if (i > 0) {
      out << ',';
    }
    out << "\"markets\":";
    getMarket(now, out);
    ++i;
  }
  if (es.trader()) {
    if (i > 0) {
      out << ',';
    }
    out << "\"traders\":";
    getTrader(now, out);
    ++i;
  }
  out << '}';
}

void Rest::getAsset(Millis now, ostream& out) const
{
  const auto& assets = serv_.assets();
  out << '[';
  copy(assets.begin(), assets.end(), OStreamJoiner(out, ','));
  out << ']';
}

void Rest::getAsset(string_view mnem, Millis now, ostream& out) const
{
  const auto& assets = serv_.assets();
  auto it = assets.find(mnem);
  if (it == assets.end()) {
    throw NotFoundException{errMsg() << "asset '" << mnem << "' does not exist"};
  }
  out << *it;
}

void Rest::getContr(Millis now, ostream& out) const
{
  const auto& contrs = serv_.contrs();
  out << '[';
  copy(contrs.begin(), contrs.end(), OStreamJoiner(out, ','));
  out << ']';
}

void Rest::getContr(string_view mnem, Millis now, ostream& out) const
{
  const auto& contrs = serv_.contrs();
  auto it = contrs.find(mnem);
  if (it == contrs.end()) {
    throw NotFoundException{errMsg() << "contr '" << mnem << "' does not exist"};
  }
  out << *it;
}

void Rest::getMarket(Millis now, ostream& out) const
{
  const auto& markets = serv_.markets();
  out << '[';
  copy(markets.begin(), markets.end(), OStreamJoiner(out, ','));
  out << ']';
}

void Rest::getMarket(string_view mnem, Millis now, ostream& out) const
{
  out << serv_.market(mnem);
}

void Rest::getTrader(Millis now, ostream& out) const
{
  const auto& traders = serv_.traders();
  out << '[';
  copy(traders.begin(), traders.end(), OStreamJoiner(out, ','));
  out << ']';
}

void Rest::getTrader(string_view mnem, Millis now, ostream& out) const
{
  out << serv_.trader(mnem);
}

void Rest::getSess(string_view trader, EntitySet es, Millis now, ostream& out) const
{
  auto& sess = serv_.trader(trader);
  int i{0};
  out << '{';
  if (es.order()) {
    out << "\"orders\":";
    out << "[]";
    ++i;
  }
  if (es.trade()) {
    if (i > 0) {
      out << ',';
    }
    out << "\"trades\":";
    out << "[]";
    ++i;
  }
  if (es.posn()) {
    if (i > 0) {
      out << ',';
    }
    out << "\"posns\":";
    out << "[]";
    ++i;
  }
  if (es.view()) {
    if (i > 0) {
      out << ',';
    }
    out << "\"views\":";
    getView(now, out);
    ++i;
  }
  out << '}';
}

void Rest::getOrder(string_view trader, Millis now, ostream& out) const
{
  auto& sess = serv_.trader(trader);
  const auto& orders = sess.orders();
  out << '[';
  copy(orders.begin(), orders.end(), OStreamJoiner(out, ','));
  out << ']';
}

void Rest::getOrder(string_view trader, string_view market, Millis now, ostream& out) const
{
  auto& sess = serv_.trader(trader);
  const auto& orders = sess.orders();
  out << '[';
  copy_if(orders.begin(), orders.end(), OStreamJoiner(out, ','),
          [market](const auto& order) { return order.market() == market; });
  out << ']';
}

void Rest::getOrder(string_view trader, string_view market, Iden id, Millis now, ostream& out) const
{
  auto& sess = serv_.trader(trader);
  const auto& orders = sess.orders();
  auto it = orders.find(market, id);
  if (it == orders.end()) {
    throw OrderNotFoundException{errMsg() << "order '" << id << "' does not exist"};
  }
  out << *it;
}

void Rest::getTrade(string_view trader, Millis now, ostream& out) const
{
  auto& sess = serv_.trader(trader);
  const auto& trades = sess.trades();
  out << '[';
  copy(trades.begin(), trades.end(), OStreamJoiner(out, ','));
  out << ']';
}

void Rest::getTrade(string_view trader, string_view market, Millis now, ostream& out) const
{
  auto& sess = serv_.trader(trader);
  const auto& trades = sess.trades();
  out << '[';
  copy_if(trades.begin(), trades.end(), OStreamJoiner(out, ','),
          [market](const auto& trade) { return trade.market() == market; });
  out << ']';
}

void Rest::getTrade(string_view trader, string_view market, Iden id, Millis now, ostream& out) const
{
  auto& sess = serv_.trader(trader);
  const auto& trades = sess.trades();
  auto it = trades.find(market, id);
  if (it == trades.end()) {
    throw NotFoundException{errMsg() << "trade '" << id << "' does not exist"};
  }
  out << *it;
}

void Rest::getPosn(string_view trader, Millis now, ostream& out) const
{
  auto& sess = serv_.trader(trader);
  const auto& posns = sess.posns();
  out << '[';
  copy(posns.begin(), posns.end(), OStreamJoiner(out, ','));
  out << ']';
}

void Rest::getPosn(string_view trader, string_view contr, Millis now, ostream& out) const
{
  auto& sess = serv_.trader(trader);
  const auto& posns = sess.posns();
  out << '[';
  copy_if(posns.begin(), posns.end(), OStreamJoiner(out, ','),
          [contr](const auto& posn) { return posn.contr() == contr; });
  out << ']';
}

void Rest::getPosn(string_view trader, string_view contr, IsoDate settlDate, Millis now,
                   ostream& out) const
{
  auto& sess = serv_.trader(trader);
  const auto& posns = sess.posns();
  auto it = posns.find(contr, maybeIsoToJd(settlDate));
  if (it == posns.end()) {
    throw NotFoundException{errMsg() << "posn for '" << contr << "' on '" << settlDate
                                     << "' does not exist"};
  }
  out << *it;
}

void Rest::getView(Millis now, ostream& out) const
{
  const auto& markets = serv_.markets();
  out << '[';
  transform(markets.begin(), markets.end(), OStreamJoiner(out, ','),
            [](const auto& market) { return static_cast<const MarketBook&>(market).view(); });
  out << ']';
}

void Rest::getView(string_view market, Millis now, ostream& out) const
{
  out << serv_.market(market).view();
}

void Rest::postMarket(string_view mnem, string_view display, string_view contr, IsoDate settlDate,
                      IsoDate expiryDate, MarketState state, Millis now, ostream& out)
{
  const auto settlDay = maybeIsoToJd(settlDate);
  const auto expiryDay = maybeIsoToJd(expiryDate);
  const auto& book = serv_.createMarket(mnem, display, contr, settlDay, expiryDay, state, now);
  out << book;
}

void Rest::putMarket(string_view mnem, optional<string_view> display, optional<MarketState> state,
                     Millis now, ostream& out)
{
  const auto& book = serv_.updateMarket(mnem, display, state, now);
  out << book;
}

void Rest::postTrader(string_view mnem, string_view display, string_view email, Millis now,
                      ostream& out)
{
  const auto& trader = serv_.createTrader(mnem, display, email, now);
  out << trader;
}

void Rest::putTrader(string_view mnem, string_view display, Millis now, ostream& out)
{
  const auto& trader = serv_.updateTrader(mnem, display, now);
  out << trader;
}

void Rest::postOrder(string_view trader, string_view market, string_view ref, Side side, Lots lots,
                     Ticks ticks, Lots minLots, Millis now, ostream& out)
{
  auto& sess = serv_.trader(trader);
  auto& book = serv_.market(market);
  Response resp;
  serv_.createOrder(sess, book, ref, side, lots, ticks, minLots, now, resp);
  out << resp;
}

void Rest::putOrder(string_view trader, string_view market, ArrayView<Iden> ids, Lots lots,
                    Millis now, ostream& out)
{
  auto& sess = serv_.trader(trader);
  auto& book = serv_.market(market);
  Response resp;
  if (lots > 0_lts) {
    if (ids.size() == 1) {
      serv_.reviseOrder(sess, book, ids[0], lots, now, resp);
    } else {
      serv_.reviseOrder(sess, book, ids, lots, now, resp);
    }
  } else {
    serv_.cancelOrder(sess, book, ids, now, resp);
  }
  out << resp;
}

void Rest::deleteOrder(string_view market, ArrayView<Iden> ids, Millis now, ostream& out)
{
  // FIXME: Not implemented.
  out << "{\"market\":\"" << market << "\",\"ids\":[";
  copy(ids.begin(), ids.end(), OStreamJoiner(out, ','));
  out << "]}";
}

void Rest::postTrade(string_view market, Millis now, ostream& out)
{
  // FIXME: Not implemented.
  out << "{\"market\":\"" << market << "\"}";
}

void Rest::deleteTrade(string_view market, ArrayView<Iden> ids, Millis now, ostream& out)
{
  // FIXME: Not implemented.
  out << "{\"market\":\"" << market << "\",\"ids\":[";
  copy(ids.begin(), ids.end(), OStreamJoiner(out, ','));
  out << "]}";
}

} // swirly

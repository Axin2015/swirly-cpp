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

void Rest::getAsset(Mnem mnem, Millis now, ostream& out) const
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

void Rest::getContr(Mnem mnem, Millis now, ostream& out) const
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

void Rest::getMarket(Mnem mnem, Millis now, ostream& out) const
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

void Rest::getTrader(Mnem mnem, Millis now, ostream& out) const
{
  out << serv_.trader(mnem);
}

void Rest::getSess(std::string_view eTrader, EntitySet es, Millis now, ostream& out) const
{
  const auto& sess = serv_.traderFromEmail(eTrader);
  int i{0};
  out << '{';
  if (es.order()) {
    out << "\"orders\":";
    getOrder(sess, now, out);
    ++i;
  }
  if (es.trade()) {
    if (i > 0) {
      out << ',';
    }
    out << "\"trades\":";
    getTrade(sess, now, out);
    ++i;
  }
  if (es.posn()) {
    if (i > 0) {
      out << ',';
    }
    out << "\"posns\":";
    getPosn(sess, now, out);
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

void Rest::getOrder(std::string_view eTrader, Millis now, ostream& out) const
{
  getOrder(serv_.traderFromEmail(eTrader), now, out);
}

void Rest::getOrder(std::string_view eTrader, Mnem market, Millis now, ostream& out) const
{
  const auto& sess = serv_.traderFromEmail(eTrader);
  const auto& orders = sess.orders();
  out << '[';
  copy_if(orders.begin(), orders.end(), OStreamJoiner(out, ','),
          [market](const auto& order) { return order.market() == market; });
  out << ']';
}

void Rest::getOrder(std::string_view eTrader, Mnem market, Iden id, Millis now, ostream& out) const
{
  const auto& sess = serv_.traderFromEmail(eTrader);
  const auto& orders = sess.orders();
  auto it = orders.find(market, id);
  if (it == orders.end()) {
    throw OrderNotFoundException{errMsg() << "order '" << id << "' does not exist"};
  }
  out << *it;
}

void Rest::getTrade(std::string_view eTrader, Millis now, ostream& out) const
{
  getTrade(serv_.traderFromEmail(eTrader), now, out);
}

void Rest::getTrade(std::string_view eTrader, Mnem market, Millis now, ostream& out) const
{
  const auto& sess = serv_.traderFromEmail(eTrader);
  const auto& trades = sess.trades();
  out << '[';
  copy_if(trades.begin(), trades.end(), OStreamJoiner(out, ','),
          [market](const auto& trade) { return trade.market() == market; });
  out << ']';
}

void Rest::getTrade(std::string_view eTrader, Mnem market, Iden id, Millis now, ostream& out) const
{
  const auto& sess = serv_.traderFromEmail(eTrader);
  const auto& trades = sess.trades();
  auto it = trades.find(market, id);
  if (it == trades.end()) {
    throw NotFoundException{errMsg() << "trade '" << id << "' does not exist"};
  }
  out << *it;
}

void Rest::getPosn(std::string_view eTrader, Millis now, ostream& out) const
{
  getPosn(serv_.traderFromEmail(eTrader), now, out);
}

void Rest::getPosn(std::string_view eTrader, Mnem contr, Millis now, ostream& out) const
{
  const auto& sess = serv_.traderFromEmail(eTrader);
  const auto& posns = sess.posns();
  out << '[';
  copy_if(posns.begin(), posns.end(), OStreamJoiner(out, ','),
          [contr](const auto& posn) { return posn.contr() == contr; });
  out << ']';
}

void Rest::getPosn(std::string_view eTrader, Mnem contr, IsoDate settlDate, Millis now,
                   ostream& out) const
{
  const auto& sess = serv_.traderFromEmail(eTrader);
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

void Rest::getView(ArrayView<Mnem> markets, Millis now, std::ostream& out) const
{
  if (markets.size() == 1) {
    out << serv_.market(markets[0]).view();
  } else {
    out << '[';
    transform(markets.begin(), markets.end(), OStreamJoiner(out, ','),
              [this](const auto& market) { return this->serv_.market(market).view(); });
    out << ']';
  }
}

void Rest::postMarket(Mnem mnem, string_view display, Mnem contr, IsoDate settlDate,
                      IsoDate expiryDate, MarketState state, Millis now, ostream& out)
{
  const auto settlDay = maybeIsoToJd(settlDate);
  const auto expiryDay = maybeIsoToJd(expiryDate);
  const auto& book = serv_.createMarket(mnem, display, contr, settlDay, expiryDay, state, now);
  out << book;
}

void Rest::putMarket(Mnem mnem, optional<string_view> display, optional<MarketState> state,
                     Millis now, ostream& out)
{
  const auto& book = serv_.updateMarket(mnem, display, state, now);
  out << book;
}

void Rest::postTrader(Mnem mnem, string_view display, string_view email, Millis now, ostream& out)
{
  const auto& trader = serv_.createTrader(mnem, display, email, now);
  out << trader;
}

void Rest::putTrader(Mnem mnem, string_view display, Millis now, ostream& out)
{
  const auto& trader = serv_.updateTrader(mnem, display, now);
  out << trader;
}

void Rest::postOrder(std::string_view eTrader, Mnem market, string_view ref, Side side, Lots lots,
                     Ticks ticks, Lots minLots, Millis now, ostream& out)
{
  auto& sess = serv_.traderFromEmail(eTrader);
  auto& book = serv_.market(market);
  Response resp;
  serv_.createOrder(sess, book, ref, side, lots, ticks, minLots, now, resp);
  out << resp;
}

void Rest::putOrder(std::string_view eTrader, Mnem market, ArrayView<Iden> ids, Lots lots,
                    Millis now, ostream& out)
{
  auto& sess = serv_.traderFromEmail(eTrader);
  auto& book = serv_.market(market);
  Response resp;
  if (lots > 0_lts) {
    if (ids.size() == 1) {
      serv_.reviseOrder(sess, book, ids[0], lots, now, resp);
    } else {
      serv_.reviseOrder(sess, book, ids, lots, now, resp);
    }
  } else {
    if (ids.size() == 1) {
      serv_.cancelOrder(sess, book, ids[0], now, resp);
    } else {
      serv_.cancelOrder(sess, book, ids, now, resp);
    }
  }
  out << resp;
}

void Rest::deleteOrder(std::string_view eTrader, Mnem market, ArrayView<Iden> ids, Millis now)
{
  auto& sess = serv_.traderFromEmail(eTrader);
  serv_.archiveOrder(sess, market, ids, now);
}

void Rest::postTrade(Mnem trader, Mnem market, string_view ref, Side side, Lots lots, Ticks ticks,
                     LiqInd liqInd, Mnem cpty, Millis now, ostream& out)
{
  auto& sess = serv_.trader(trader);
  auto& book = serv_.market(market);
  auto trades = serv_.createTrade(sess, book, ref, side, lots, ticks, liqInd, cpty, now);
  out << '[' << *trades.first;
  if (trades.second) {
    out << ',' << *trades.second;
  }
  out << ']';
}

void Rest::deleteTrade(std::string_view eTrader, Mnem market, ArrayView<Iden> ids, Millis now)
{
  auto& sess = serv_.traderFromEmail(eTrader);
  serv_.archiveTrade(sess, market, ids, now);
}

void Rest::getOrder(const TraderSess& sess, Millis now, std::ostream& out) const
{
  const auto& orders = sess.orders();
  out << '[';
  copy(orders.begin(), orders.end(), OStreamJoiner(out, ','));
  out << ']';
}

void Rest::getTrade(const TraderSess& sess, Millis now, std::ostream& out) const
{
  const auto& trades = sess.trades();
  out << '[';
  copy(trades.begin(), trades.end(), OStreamJoiner(out, ','));
  out << ']';
}

void Rest::getPosn(const TraderSess& sess, Millis now, std::ostream& out) const
{
  const auto& posns = sess.posns();
  out << '[';
  copy(posns.begin(), posns.end(), OStreamJoiner(out, ','));
  out << ']';
}

} // swirly

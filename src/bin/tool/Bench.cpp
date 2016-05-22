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
#include <swirly/fig/Mock.hpp>
#include <swirly/fig/Response.hpp>
#include <swirly/fig/Serv.hpp>
#include <swirly/fig/TraderSess.hpp>

#include <swirly/elm/MarketBook.hpp>

#include <swirly/ash/Conf.hpp>
#include <swirly/ash/Log.hpp>
#include <swirly/ash/Profile.hpp>
#include <swirly/ash/Time.hpp>

using namespace std;
using namespace swirly;

namespace {

MarketBook& createMarket(Serv& serv, string_view mnem, string_view display, string_view contr,
                         Jday settlDay, Jday expiryDay, MarketState state, Millis now)
{
  auto it = serv.markets().find(mnem);
  if (it != serv.markets().end()) {
    return static_cast<MarketBook&>(*it);
  }
  return serv.createMarket(mnem, display, contr, settlDay, expiryDay, state, now);
}

TraderSess& createTrader(Serv& serv, string_view mnem, string_view display, string_view email,
                         Millis now)
{
  auto it = serv.traders().find(mnem);
  if (it != serv.traders().end()) {
    return static_cast<TraderSess&>(*it);
  }
  return serv.createTrader(mnem, display, email, now);
}

class Archiver {
 public:
  explicit Archiver(Serv& serv) noexcept : serv_(serv) {}
  void operator()(TraderSess& sess, std::string_view market, Millis now)
  {
    ids_.clear();
    for (const auto& order : sess.orders()) {
      if (order.market() == market && order.done()) {
        ids_.push_back(order.id());
      }
    }
    serv_.archiveOrder(sess, market, ids_, now);
    ids_.clear();
    for (const auto& trade : sess.trades()) {
      if (trade.market() == market) {
        ids_.push_back(trade.id());
      }
    }
    serv_.archiveTrade(sess, market, ids_, now);
  }

 private:
  Serv& serv_;
  vector<Iden> ids_;
};

} // anonymous

int main(int argc, char* argv[])
{
  int ret = 1;
  try {

    unique_ptr<Journ> journ;
    unique_ptr<Model> model;
    if (argc > 1) {
      Conf conf;
      conf.set("sqlite_journ", argv[1]);
      conf.set("sqlite_model", argv[1]);
      journ = swirly::makeJourn(conf);
      model = swirly::makeModel(conf);
    } else {
      journ = make_unique<MockJourn>();
      model = make_unique<MockModel>();
    }

    const auto now = getTimeOfDay();

    Serv serv{*journ};
    serv.load(*model, now);
    model = nullptr;

    auto& book = createMarket(serv, "EURUSD"_sv, "EURUSD"_sv, "EURUSD"_sv, 0_jd, 0_jd, 0, now);

    auto& eddayl
      = createTrader(serv, "EDDAYL"_sv, "Eddie Aylett"_sv, "eddie.aylett@swirlycloud.com"_sv, now);
    auto& gosayl
      = createTrader(serv, "GOSAYL"_sv, "Goska Aylett"_sv, "goska.aylett@swirlycloud.com"_sv, now);
    auto& marayl
      = createTrader(serv, "MARAYL"_sv, "Mark Aylett"_sv, "mark.aylett@swirlycloud.com"_sv, now);
    auto& pipayl = createTrader(serv, "PIPAYL"_sv, "Pippin Aylett"_sv,
                                "pippin.aylett@swirlycloud.com"_sv, now);

    Profile maker{"maker"_sv};
    Profile taker{"taker"_sv};

    Archiver arch{serv};
    Response resp;
    for (int i = 0; i < 5100; ++i) {

      // Reset profiles after warmup period.
      if (i == 100) {
        maker.clear();
        taker.clear();
      }

      const auto now = getTimeOfDay();

      // Maker sell-side.
      {
        TimeRecorder tr{maker};
        resp.clear();
        serv.createOrder(gosayl, book, ""_sv, Side::Sell, 10_lts, 12348_tks, 1_lts, now, resp);
      }
      {
        TimeRecorder tr{maker};
        resp.clear();
        serv.createOrder(marayl, book, ""_sv, Side::Sell, 10_lts, 12348_tks, 1_lts, now, resp);
      }
      {
        TimeRecorder tr{maker};
        resp.clear();
        serv.createOrder(gosayl, book, ""_sv, Side::Sell, 10_lts, 12347_tks, 1_lts, now, resp);
      }
      {
        TimeRecorder tr{maker};
        resp.clear();
        serv.createOrder(marayl, book, ""_sv, Side::Sell, 5_lts, 12347_tks, 1_lts, now, resp);
      }
      {
        TimeRecorder tr{maker};
        resp.clear();
        serv.createOrder(gosayl, book, ""_sv, Side::Sell, 5_lts, 12346_tks, 1_lts, now, resp);
      }

      // Maker buy-side.
      {
        TimeRecorder tr{maker};
        resp.clear();
        serv.createOrder(marayl, book, ""_sv, Side::Buy, 5_lts, 12344_tks, 1_lts, now, resp);
      }
      {
        TimeRecorder tr{maker};
        resp.clear();
        serv.createOrder(gosayl, book, ""_sv, Side::Buy, 5_lts, 12343_tks, 1_lts, now, resp);
      }
      {
        TimeRecorder tr{maker};
        resp.clear();
        serv.createOrder(marayl, book, ""_sv, Side::Buy, 10_lts, 12343_tks, 1_lts, now, resp);
      }
      {
        TimeRecorder tr{maker};
        resp.clear();
        serv.createOrder(gosayl, book, ""_sv, Side::Buy, 10_lts, 12342_tks, 1_lts, now, resp);
      }
      {
        TimeRecorder tr{maker};
        resp.clear();
        serv.createOrder(marayl, book, ""_sv, Side::Buy, 10_lts, 12342_tks, 1_lts, now, resp);
      }

      // Taker sell-side.
      {
        TimeRecorder tr{taker};
        resp.clear();
        serv.createOrder(eddayl, book, ""_sv, Side::Sell, 40_lts, 12342_tks, 1_lts, now, resp);
      }

      // Taker buy-side.
      {
        TimeRecorder tr{taker};
        resp.clear();
        serv.createOrder(pipayl, book, ""_sv, Side::Buy, 40_lts, 12348_tks, 1_lts, now, resp);
      }

      arch(eddayl, "EURUSD"_sv, now);
      arch(gosayl, "EURUSD"_sv, now);
      arch(marayl, "EURUSD"_sv, now);
      arch(pipayl, "EURUSD"_sv, now);
    }

    ret = 0;
  } catch (const exception& e) {
    SWIRLY_ERROR(logMsg() << "exception: " << e.what());
  }
  return ret;
}

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
#include <swirly/fig/Accnt.hpp>
#include <swirly/fig/MemPool.hpp>
#include <swirly/fig/Response.hpp>
#include <swirly/fig/Serv.hpp>
#include <swirly/fig/Test.hpp>

#include <swirly/elm/Date.hpp>

#include <swirly/ash/Conf.hpp>
#include <swirly/ash/Log.hpp>
#include <swirly/ash/Profile.hpp>
#include <swirly/ash/Time.hpp>

using namespace std;
using namespace swirly;

namespace {

const Market& createMarket(Serv& serv, Mnem contrMnem, JDay settlDay, MarketState state, Time now)
{
  const auto& contr = serv.contr(contrMnem);
  const auto marketId = toMarketId(contr.id(), settlDay);
  auto it = serv.markets().find(marketId);
  if (it != serv.markets().end()) {
    return *it;
  }
  return serv.createMarket(contr, settlDay, state, now);
}

class Archiver {
 public:
  explicit Archiver(Serv& serv) noexcept : serv_(serv) {}
  void operator()(const Accnt& accnt, Id64 marketId, Time now)
  {
    ids_.clear();
    for (const auto& trade : accnt.trades()) {
      if (trade.marketId() == marketId) {
        ids_.push_back(trade.id());
      }
    }
    serv_.archiveTrade(accnt, marketId, ids_, now);
  }

 private:
  Serv& serv_;
  vector<Id64> ids_;
};

MemPool memPool;

} // anonymous

namespace swirly {

void* alloc(size_t size)
{
  return memPool.alloc(size);
}

void dealloc(void* ptr, size_t size) noexcept
{
  return memPool.dealloc(ptr, size);
}

} // swirly

int main(int argc, char* argv[])
{
  int ret = 1;
  try {

    memPool.reserve(1 << 20);

    unique_ptr<Journ> journ;
    unique_ptr<Model> model;
    if (argc > 1) {
      Conf conf;
      conf.set("sqlite_journ", argv[1]);
      conf.set("sqlite_model", argv[1]);
      journ = swirly::makeJourn(conf);
      model = swirly::makeModel(conf);
    } else {
      journ = make_unique<TestJourn>();
      model = make_unique<TestModel>();
    }

    const BusinessDay busDay{RollHour, NewYork};
    const auto now = UnixClock::now();

    Serv serv{*journ, 1 << 10, 1 << 4};
    serv.load(*model, now);
    model = nullptr;

    auto& market = createMarket(serv, "EURUSD"_sv, busDay(now), 0, now);

    auto& eddayl = serv.accnt("EDDAYL"_sv);
    auto& gosayl = serv.accnt("GOSAYL"_sv);
    auto& marayl = serv.accnt("MARAYL"_sv);
    auto& pipayl = serv.accnt("PIPAYL"_sv);

    Profile maker{"maker"_sv};
    Profile taker{"taker"_sv};

    Archiver arch{serv};
    Response resp;
    for (int i = 0; i < 25100; ++i) {

      // Reset profiles after warmup period.
      if (i == 100) {
        maker.clear();
        taker.clear();
      }

      // Maker sell-side.
      {
        TimeRecorder tr{maker};
        resp.clear();
        serv.createOrder(gosayl, market, ""_sv, Side::Sell, 10_lts, 12348_tks, 1_lts, now, resp);
      }
      {
        TimeRecorder tr{maker};
        resp.clear();
        serv.createOrder(marayl, market, ""_sv, Side::Sell, 10_lts, 12348_tks, 1_lts, now, resp);
      }
      {
        TimeRecorder tr{maker};
        resp.clear();
        serv.createOrder(gosayl, market, ""_sv, Side::Sell, 10_lts, 12347_tks, 1_lts, now, resp);
      }
      {
        TimeRecorder tr{maker};
        resp.clear();
        serv.createOrder(marayl, market, ""_sv, Side::Sell, 5_lts, 12347_tks, 1_lts, now, resp);
      }
      {
        TimeRecorder tr{maker};
        resp.clear();
        serv.createOrder(gosayl, market, ""_sv, Side::Sell, 5_lts, 12346_tks, 1_lts, now, resp);
      }

      // Maker buy-side.
      {
        TimeRecorder tr{maker};
        resp.clear();
        serv.createOrder(marayl, market, ""_sv, Side::Buy, 5_lts, 12344_tks, 1_lts, now, resp);
      }
      {
        TimeRecorder tr{maker};
        resp.clear();
        serv.createOrder(gosayl, market, ""_sv, Side::Buy, 5_lts, 12343_tks, 1_lts, now, resp);
      }
      {
        TimeRecorder tr{maker};
        resp.clear();
        serv.createOrder(marayl, market, ""_sv, Side::Buy, 10_lts, 12343_tks, 1_lts, now, resp);
      }
      {
        TimeRecorder tr{maker};
        resp.clear();
        serv.createOrder(gosayl, market, ""_sv, Side::Buy, 10_lts, 12342_tks, 1_lts, now, resp);
      }
      {
        TimeRecorder tr{maker};
        resp.clear();
        serv.createOrder(marayl, market, ""_sv, Side::Buy, 10_lts, 12342_tks, 1_lts, now, resp);
      }

      // Taker sell-side.
      {
        TimeRecorder tr{taker};
        resp.clear();
        serv.createOrder(eddayl, market, ""_sv, Side::Sell, 40_lts, 12342_tks, 1_lts, now, resp);
      }

      // Taker buy-side.
      {
        TimeRecorder tr{taker};
        resp.clear();
        serv.createOrder(pipayl, market, ""_sv, Side::Buy, 40_lts, 12348_tks, 1_lts, now, resp);
      }

      arch(eddayl, market.id(), now);
      arch(gosayl, market.id(), now);
      arch(marayl, market.id(), now);
      arch(pipayl, market.id(), now);
    }

    ret = 0;
  } catch (const exception& e) {
    SWIRLY_ERROR(logMsg() << "exception: " << e.what());
  }
  return ret;
}

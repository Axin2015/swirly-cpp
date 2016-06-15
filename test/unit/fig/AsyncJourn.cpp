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
#include <swirly/fig/AsyncJourn.hpp>

#include <swirly/elm/Exec.hpp>
#include <swirly/elm/Journ.hpp>
#include <swirly/elm/Msg.hpp>

#include <swirly/ash/JulianDay.hpp>
#include <swirly/ash/Time.hpp>

#include <swirly/tea/Test.hpp>

#include <queue>

using namespace std;
using namespace swirly;
using namespace swirly::enumops;

namespace {

constexpr auto Today = ymdToJd(2014, 2, 11);
constexpr auto SettlDay = Today + 2_jd;
constexpr auto ExpiryDay = Today + 1_jd;

constexpr auto Now = jdToMs(Today);

struct JournState {
  int reset{0};
  int createMarket{0};
  int updateMarket{0};
  int createTrader{0};
  int updateTrader{0};
  int createExec{0};
  int archiveOrder{0};
  int archiveTrade{0};
  int total{0};
};

struct TestJourn : Journ {
  bool pop(Msg& msg)
  {
    std::unique_lock<std::mutex> lock{mutex_};
    auto now = std::chrono::system_clock::now();
    auto pred = [this]() { return !this->msgs_.empty(); };
    if (!notEmpty_.wait_until(lock, now + std::chrono::seconds(2), pred)) {
      // Timeout.
      return false;
    }
    msg = msgs_.front();
    msgs_.pop();
    return true;
  }

 protected:
  void doUpdate(const Msg& msg) override
  {
    std::unique_lock<std::mutex> lock{mutex_};
    msgs_.push(msg);
    lock.unlock();
    notEmpty_.notify_one();
  }

 private:
  mutex mutex_;
  condition_variable notEmpty_;
  queue<Msg> msgs_;
};

struct AsyncJournFixture {
  AsyncJournFixture() : asyncJourn{journ, 1 << 10} {}
  TestJourn journ;
  AsyncJourn asyncJourn;
};

} // anonymous

SWIRLY_TEST_CASE(AsyncWindow)
{
  {
    detail::AsyncWindow<1> aw{3};
    // 1.
    SWIRLY_CHECK(aw.index() == 0);
    SWIRLY_CHECK(aw.size() == 1);
    SWIRLY_CHECK(aw.more() == More::Yes);
    SWIRLY_CHECK(aw.next());
    SWIRLY_CHECK(!aw.done());
    // 2.
    SWIRLY_CHECK(aw.index() == 1);
    SWIRLY_CHECK(aw.size() == 1);
    SWIRLY_CHECK(aw.more() == More::Yes);
    SWIRLY_CHECK(aw.next());
    SWIRLY_CHECK(!aw.done());
    // 3.
    SWIRLY_CHECK(aw.index() == 2);
    SWIRLY_CHECK(aw.size() == 1);
    SWIRLY_CHECK(aw.more() == More::No);
    SWIRLY_CHECK(!aw.next());
    SWIRLY_CHECK(aw.done());
  }
  {
    detail::AsyncWindow<8> aw{4};
    // 0-3.
    SWIRLY_CHECK(aw.index() == 0);
    SWIRLY_CHECK(aw.size() == 4);
    SWIRLY_CHECK(aw.more() == More::No);
    SWIRLY_CHECK(!aw.next());
    SWIRLY_CHECK(aw.done());
  }
  {
    detail::AsyncWindow<8> aw{16};
    // 0-7.
    SWIRLY_CHECK(aw.index() == 0);
    SWIRLY_CHECK(aw.size() == 8);
    SWIRLY_CHECK(aw.more() == More::Yes);
    SWIRLY_CHECK(aw.next());
    SWIRLY_CHECK(!aw.done());
    // 8-15
    SWIRLY_CHECK(aw.index() == 8);
    SWIRLY_CHECK(aw.size() == 8);
    SWIRLY_CHECK(aw.more() == More::No);
    SWIRLY_CHECK(!aw.next());
    SWIRLY_CHECK(aw.done());
  }
  {
    detail::AsyncWindow<8> aw{20};
    // 0-7.
    SWIRLY_CHECK(aw.index() == 0);
    SWIRLY_CHECK(aw.size() == 8);
    SWIRLY_CHECK(aw.more() == More::Yes);
    SWIRLY_CHECK(aw.next());
    SWIRLY_CHECK(!aw.done());
    // 8-15
    SWIRLY_CHECK(aw.index() == 8);
    SWIRLY_CHECK(aw.size() == 8);
    SWIRLY_CHECK(aw.more() == More::Yes);
    SWIRLY_CHECK(aw.next());
    SWIRLY_CHECK(!aw.done());
    // 16-19.
    SWIRLY_CHECK(aw.index() == 16);
    SWIRLY_CHECK(aw.size() == 4);
    SWIRLY_CHECK(aw.more() == More::No);
    SWIRLY_CHECK(!aw.next());
    SWIRLY_CHECK(aw.done());
  }
}

SWIRLY_FIXTURE_TEST_CASE(AsyncJournCreateMarket, AsyncJournFixture)
{
  asyncJourn.createMarket("EURUSD.MAR14"_sv, "EURUSD March 14"_sv, "EURUSD"_sv, SettlDay, ExpiryDay,
                          0x1);

  Msg msg;
  SWIRLY_CHECK(journ.pop(msg));
  SWIRLY_CHECK(msg.type == MsgType::CreateMarket);
  const auto& body = msg.createMarket;

  SWIRLY_CHECK(strncmp(body.mnem, "EURUSD.MAR14", sizeof(body.mnem)) == 0);
  SWIRLY_CHECK(strncmp(body.display, "EURUSD March 14", sizeof(body.display)) == 0);
  SWIRLY_CHECK(strncmp(body.contr, "EURUSD", sizeof(body.contr)) == 0);
  SWIRLY_CHECK(body.settlDay == body.settlDay);
  SWIRLY_CHECK(body.expiryDay == body.expiryDay);
  SWIRLY_CHECK(body.state == body.state);
}

SWIRLY_FIXTURE_TEST_CASE(AsyncJournUpdateMarket, AsyncJournFixture)
{
  asyncJourn.updateMarket("EURUSD.MAR14"_sv, "EURUSD March 14"_sv, 0x1);

  Msg msg;
  SWIRLY_CHECK(journ.pop(msg));
  SWIRLY_CHECK(msg.type == MsgType::UpdateMarket);
  const auto& body = msg.updateMarket;

  SWIRLY_CHECK(strncmp(body.mnem, "EURUSD.MAR14", sizeof(body.mnem)) == 0);
  SWIRLY_CHECK(strncmp(body.display, "EURUSD March 14", sizeof(body.display)) == 0);
  SWIRLY_CHECK(body.state == body.state);
}

SWIRLY_FIXTURE_TEST_CASE(AsyncJournCreateTrader, AsyncJournFixture)
{
  asyncJourn.createTrader("MARAYL"_sv, "Mark Aylett"_sv, "mark.aylett@swirlycloud.com"_sv);

  Msg msg;
  SWIRLY_CHECK(journ.pop(msg));
  SWIRLY_CHECK(msg.type == MsgType::CreateTrader);
  const auto& body = msg.createTrader;

  SWIRLY_CHECK(strncmp(body.mnem, "MARAYL", sizeof(body.mnem)) == 0);
  SWIRLY_CHECK(strncmp(body.display, "Mark Aylett", sizeof(body.display)) == 0);
  SWIRLY_CHECK(strncmp(body.email, "mark.aylett@swirlycloud.com", sizeof(body.email)) == 0);
}

SWIRLY_FIXTURE_TEST_CASE(AsyncJournUpdateTrader, AsyncJournFixture)
{
  asyncJourn.updateTrader("MARAYL"_sv, "Mark Aylett"_sv);

  Msg msg;
  SWIRLY_CHECK(journ.pop(msg));
  SWIRLY_CHECK(msg.type == MsgType::UpdateTrader);
  const auto& body = msg.updateTrader;

  SWIRLY_CHECK(strncmp(body.mnem, "MARAYL", sizeof(body.mnem)) == 0);
  SWIRLY_CHECK(strncmp(body.display, "Mark Aylett", sizeof(body.display)) == 0);
}

SWIRLY_FIXTURE_TEST_CASE(AsyncJournCreateExec, AsyncJournFixture)
{
  ConstExecPtr execs[2];
  execs[0] = makeRefCounted<Exec>("MARAYL"_sv, "EURUSD.MAR14"_sv, "EURUSD"_sv, SettlDay, 1_id,
                                  "REF"_sv, 2_id, State::New, Side::Buy, 10_lts, 12345_tks, 10_lts,
                                  0_lts, 0_cst, 0_lts, 0_tks, 1_lts, 0_id, Role::None, Mnem{}, Now);
  execs[1] = makeRefCounted<Exec>("MARAYL"_sv, "EURUSD.MAR14"_sv, "EURUSD"_sv, SettlDay, 3_id,
                                  "REF"_sv, 2_id, State::Trade, Side::Buy, 10_lts, 12345_tks, 5_lts,
                                  5_lts, 61725_cst, 5_lts, 12345_tks, 1_lts, 4_id, Role::Maker,
                                  "GOSAYL"_sv, Now + 1_ms);
  asyncJourn.createExec(execs);
  {
    Msg msg;
    SWIRLY_CHECK(journ.pop(msg));
    SWIRLY_CHECK(msg.type == MsgType::CreateExec);
    const auto& body = msg.createExec;

    SWIRLY_CHECK(strncmp(body.trader, "MARAYL", sizeof(body.trader)) == 0);
    SWIRLY_CHECK(strncmp(body.market, "EURUSD.MAR14", sizeof(body.market)) == 0);
    SWIRLY_CHECK(strncmp(body.contr, "EURUSD", sizeof(body.contr)) == 0);
    SWIRLY_CHECK(body.settlDay == SettlDay);
    SWIRLY_CHECK(body.id == 1_id);
    SWIRLY_CHECK(strncmp(body.ref, "REF", sizeof(body.ref)) == 0);
    SWIRLY_CHECK(body.orderId == 2_id);
    SWIRLY_CHECK(body.state == State::New);
    SWIRLY_CHECK(body.side == Side::Buy);
    SWIRLY_CHECK(body.lots == 10_lts);
    SWIRLY_CHECK(body.ticks == 12345_tks);
    SWIRLY_CHECK(body.resd == 10_lts);
    SWIRLY_CHECK(body.exec == 0_lts);
    SWIRLY_CHECK(body.cost == 0_cst);
    SWIRLY_CHECK(body.lastLots == 0_lts);
    SWIRLY_CHECK(body.lastTicks == 0_tks);
    SWIRLY_CHECK(body.minLots == 1_lts);
    SWIRLY_CHECK(body.matchId == 0_id);
    SWIRLY_CHECK(body.role == Role::None);
    SWIRLY_CHECK(strncmp(body.cpty, "", sizeof(body.cpty)) == 0);
    SWIRLY_CHECK(body.created == Now);
  }
  {
    Msg msg;
    SWIRLY_CHECK(journ.pop(msg));
    SWIRLY_CHECK(msg.type == MsgType::CreateExec);
    const auto& body = msg.createExec;

    SWIRLY_CHECK(strncmp(body.trader, "MARAYL", sizeof(body.trader)) == 0);
    SWIRLY_CHECK(strncmp(body.market, "EURUSD.MAR14", sizeof(body.market)) == 0);
    SWIRLY_CHECK(strncmp(body.contr, "EURUSD", sizeof(body.contr)) == 0);
    SWIRLY_CHECK(body.settlDay == SettlDay);
    SWIRLY_CHECK(body.id == 3_id);
    SWIRLY_CHECK(strncmp(body.ref, "REF", sizeof(body.ref)) == 0);
    SWIRLY_CHECK(body.orderId == 2_id);
    SWIRLY_CHECK(body.state == State::Trade);
    SWIRLY_CHECK(body.side == Side::Buy);
    SWIRLY_CHECK(body.lots == 10_lts);
    SWIRLY_CHECK(body.ticks == 12345_tks);
    SWIRLY_CHECK(body.resd == 5_lts);
    SWIRLY_CHECK(body.exec == 5_lts);
    SWIRLY_CHECK(body.cost == 61725_cst);
    SWIRLY_CHECK(body.lastLots == 5_lts);
    SWIRLY_CHECK(body.lastTicks == 12345_tks);
    SWIRLY_CHECK(body.minLots == 1_lts);
    SWIRLY_CHECK(body.matchId == 4_id);
    SWIRLY_CHECK(body.role == Role::Maker);
    SWIRLY_CHECK(strncmp(body.cpty, "GOSAYL", sizeof(body.cpty)) == 0);
    SWIRLY_CHECK(body.created == Now + 1_ms);
  }
}

SWIRLY_FIXTURE_TEST_CASE(AsyncJournArchiveOrder, AsyncJournFixture)
{
  vector<Iden> ids;
  ids.reserve(101);

  Iden id{};
  generate_n(back_insert_iterator<decltype(ids)>(ids), ids.capacity(), [&id]() { return ++id; });
  asyncJourn.archiveOrder("EURUSD.MAR14"_sv, ids, Now);

  auto it = ids.begin();
  while (it != ids.end()) {
    Msg msg;
    SWIRLY_CHECK(journ.pop(msg));
    SWIRLY_CHECK(msg.type == MsgType::ArchiveOrder);
    const auto& body = msg.archive;
    SWIRLY_CHECK(strncmp(body.market, "EURUSD.MAR14", sizeof(body.market)) == 0);
    SWIRLY_CHECK(body.modified == Now);
    for (const auto id : body.ids) {
      if (it != ids.end()) {
        SWIRLY_CHECK(id == *it++);
      } else {
        SWIRLY_CHECK(id == 0_id);
      }
    }
  }
}

SWIRLY_FIXTURE_TEST_CASE(AsyncJournArchiveTrade, AsyncJournFixture)
{
  vector<Iden> ids;
  ids.reserve(101);

  Iden id{};
  generate_n(back_insert_iterator<decltype(ids)>(ids), ids.capacity(), [&id]() { return ++id; });
  asyncJourn.archiveTrade("EURUSD.MAR14"_sv, ids, Now);

  auto it = ids.begin();
  while (it != ids.end()) {
    Msg msg;
    SWIRLY_CHECK(journ.pop(msg));
    SWIRLY_CHECK(msg.type == MsgType::ArchiveTrade);
    const auto& body = msg.archive;
    SWIRLY_CHECK(strncmp(body.market, "EURUSD.MAR14", sizeof(body.market)) == 0);
    SWIRLY_CHECK(body.modified == Now);
    for (const auto id : body.ids) {
      if (it != ids.end()) {
        SWIRLY_CHECK(id == *it++);
      } else {
        SWIRLY_CHECK(id == 0_id);
      }
    }
  }
}

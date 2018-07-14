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
#include <swirly/prof/HdrHistogram.hpp>
#include <swirly/prof/HdrRecorder.hpp>

#include <swirly/db/SqliteJourn.hpp>
#include <swirly/db/SqliteModel.hpp>

#include <swirly/lob/Response.hpp>
#include <swirly/lob/Serv.hpp>
#include <swirly/lob/Sess.hpp>
#include <swirly/lob/Test.hpp>

#include <swirly/fin/Date.hpp>
#include <swirly/fin/MsgQueue.hpp>

#include <swirly/app/MemCtx.hpp>
#include <swirly/app/Thread.hpp>

#include <swirly/sys/Signal.hpp>

#include <swirly/util/Config.hpp>
#include <swirly/util/Finally.hpp>
#include <swirly/util/Log.hpp>
#include <swirly/util/Time.hpp>

#include <thread>

using namespace std;
using namespace swirly;

namespace {

const Market& create_market(Serv& serv, Symbol instr_symbol, JDay settl_day, MarketState state,
                            Time now)
{
    const auto& instr = serv.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_day);
    auto it = serv.markets().find(market_id);
    if (it != serv.markets().end()) {
        return *it;
    }
    return serv.create_market(instr, settl_day, state, now);
}

class Archiver {
  public:
    explicit Archiver(Serv& serv) noexcept
    : serv_(serv)
    {
    }
    void operator()(const Sess& sess, Id64 market_id, Time now)
    {
        ids_.clear();
        for (const auto& trade : sess.trades()) {
            if (trade.market_id() == market_id) {
                ids_.push_back(trade.id());
            }
        }
        serv_.archive_trade(sess, market_id, ids_, now);
    }

  private:
    Serv& serv_;
    vector<Id64> ids_;
};

class NullJourn : public Journ {
  public:
    NullJourn() noexcept = default;
    ~NullJourn() override = default;

  protected:
    void do_begin() override {}
    void do_commit() override {}
    void do_rollback() noexcept override {}
    void do_write(const Msg& msg) override {}
};

MemCtx mem_ctx;

} // namespace

namespace swirly {

void* alloc(size_t size)
{
    return mem_ctx.alloc(size);
}

void* alloc(size_t size, align_val_t al)
{
    return mem_ctx.alloc(size, al);
}

void dealloc(void* ptr, size_t size) noexcept
{
    return mem_ctx.dealloc(ptr, size);
}

} // namespace swirly

int main(int argc, char* argv[])
{
    int ret = 1;
    try {

        mem_ctx = MemCtx{1 << 20};

        unique_ptr<Model> model;
        if (argc > 1) {
            Config config;
            config.set("db_type", "sqlite");
            config.set("db_name", argv[1]);
            model = make_unique<SqliteModel>(config);
        } else {
            model = make_unique<TestModel>();
        }

        const BusinessDay bus_day{MarketZone};
        const auto start_time = UnixClock::now();

        MsgQueue mq{1 << 10};
        Serv serv{mq, 1 << 4};
        serv.load(*model, start_time);
        model = nullptr;

        NullJourn journ;
        auto journ_agent = [&mq, &journ]() {
            int n{0};
            Msg msg;
            while (mq.pop(msg)) {
                journ.write(msg);
                ++n;
            }
            return n;
        };
        AgentThread journ_thread{journ_agent, ThreadConfig{"journ"s}};

        auto& market = create_market(serv, "EURUSD"sv, bus_day(start_time), 0, start_time);

        auto& eddayl = serv.sess("EDDAYL"sv);
        auto& gosayl = serv.sess("GOSAYL"sv);
        auto& marayl = serv.sess("MARAYL"sv);
        auto& pipayl = serv.sess("PIPAYL"sv);

        HdrHistogram maker{1, 1'000'000, 5};
        HdrHistogram taker{1, 1'000'000, 5};

        Archiver arch{serv};
        Response resp;
        for (int i = 0; i < 1'001'000; ++i) {

            // Reset profiles after warmup period.
            if (i == 1000) {
                maker.reset();
                taker.reset();
            }

            // Maker sell-side.
            {
                HdrRecorder tr{maker};
                resp.clear();
                serv.create_order(gosayl, market, ""sv, Side::Sell, 10_lts, 12348_tks, 1_lts,
                                  start_time, resp);
            }
            {
                HdrRecorder tr{maker};
                resp.clear();
                serv.create_order(marayl, market, ""sv, Side::Sell, 10_lts, 12348_tks, 1_lts,
                                  start_time, resp);
            }
            {
                HdrRecorder tr{maker};
                resp.clear();
                serv.create_order(gosayl, market, ""sv, Side::Sell, 10_lts, 12347_tks, 1_lts,
                                  start_time, resp);
            }
            {
                HdrRecorder tr{maker};
                resp.clear();
                serv.create_order(marayl, market, ""sv, Side::Sell, 5_lts, 12347_tks, 1_lts,
                                  start_time, resp);
            }
            {
                HdrRecorder tr{maker};
                resp.clear();
                serv.create_order(gosayl, market, ""sv, Side::Sell, 5_lts, 12346_tks, 1_lts,
                                  start_time, resp);
            }

            // Maker buy-side.
            {
                HdrRecorder tr{maker};
                resp.clear();
                serv.create_order(marayl, market, ""sv, Side::Buy, 5_lts, 12344_tks, 1_lts,
                                  start_time, resp);
            }
            {
                HdrRecorder tr{maker};
                resp.clear();
                serv.create_order(gosayl, market, ""sv, Side::Buy, 5_lts, 12343_tks, 1_lts,
                                  start_time, resp);
            }
            {
                HdrRecorder tr{maker};
                resp.clear();
                serv.create_order(marayl, market, ""sv, Side::Buy, 10_lts, 12343_tks, 1_lts,
                                  start_time, resp);
            }
            {
                HdrRecorder tr{maker};
                resp.clear();
                serv.create_order(gosayl, market, ""sv, Side::Buy, 10_lts, 12342_tks, 1_lts,
                                  start_time, resp);
            }
            {
                HdrRecorder tr{maker};
                resp.clear();
                serv.create_order(marayl, market, ""sv, Side::Buy, 10_lts, 12342_tks, 1_lts,
                                  start_time, resp);
            }

            // Taker sell-side.
            {
                HdrRecorder tr{taker};
                resp.clear();
                serv.create_order(eddayl, market, ""sv, Side::Sell, 40_lts, 12342_tks, 1_lts,
                                  start_time, resp);
            }

            // Taker buy-side.
            {
                HdrRecorder tr{taker};
                resp.clear();
                serv.create_order(pipayl, market, ""sv, Side::Buy, 40_lts, 12348_tks, 1_lts,
                                  start_time, resp);
            }

            arch(eddayl, market.id(), start_time);
            arch(gosayl, market.id(), start_time);
            arch(marayl, market.id(), start_time);
            arch(pipayl, market.id(), start_time);
        }

        fprintf(stderr, "Maker Percentile Report\n");
        fprintf(stderr, "-----------------------\n");
        maker.print(stderr, 5, 1000);

        fprintf(stderr, "Taker Percentile Report\n");
        fprintf(stderr, "-----------------------\n");
        taker.print(stderr, 5, 1000);

        fflush(stderr);
        ret = 0;
    } catch (const exception& e) {
        SWIRLY_ERROR << "exception: " << e.what();
    }
    return ret;
}

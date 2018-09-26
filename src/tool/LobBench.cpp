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
#include <swirly/hdr/Histogram.hpp>
#include <swirly/hdr/Recorder.hpp>

#include <swirly/db/SqliteJourn.hpp>
#include <swirly/db/SqliteModel.hpp>

#include <swirly/lob/App.hpp>
#include <swirly/lob/Response.hpp>
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

#include <thread>

using namespace std;
using namespace swirly;

namespace {

const Market& create_market(LobApp& app, Symbol instr_symbol, JDay settl_day, MarketState state,
                            WallTime now)
{
    const auto& instr = app.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_day);
    auto it = app.markets().find(market_id);
    if (it != app.markets().end()) {
        return *it;
    }
    return app.create_market(now, instr, settl_day, state);
}

class Archiver {
  public:
    explicit Archiver(LobApp& app) noexcept
    : app_(app)
    {
    }
    void operator()(WallTime now, const Sess& sess, Id64 market_id)
    {
        ids_.clear();
        for (const auto& trade : sess.trades()) {
            if (trade.market_id() == market_id) {
                ids_.push_back(trade.id());
            }
        }
        app_.archive_trade(now, sess, market_id, ids_);
    }

  private:
    LobApp& app_;
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
inline namespace app {

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

void dealloc(void* ptr, size_t size, align_val_t al) noexcept
{
    return mem_ctx.dealloc(ptr, size, al);
}

} // namespace app
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
        const auto start_time = WallClock::now();

        MsgQueue mq{1 << 12};
        LobApp app{mq, 1 << 4};
        app.load(start_time, *model);
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

        auto& market = create_market(app, "EURUSD"sv, bus_day(start_time), 0, start_time);

        auto& eddayl = app.sess("EDDAYL"sv);
        auto& gosayl = app.sess("GOSAYL"sv);
        auto& marayl = app.sess("MARAYL"sv);
        auto& pipayl = app.sess("PIPAYL"sv);

        Histogram maker_hist{1, 1'000'000, 5};
        Histogram taker_hist{1, 1'000'000, 5};

        Archiver arch{app};
        Response resp;

        for (int i = 0; i < 1'001'000; ++i) {

            // Reset profiles after warmup period.
            if (i == 1000) {
                maker_hist.clear();
                taker_hist.clear();
            }

            // Maker sell-side.
            {
                resp.clear();
                Recorder tr{maker_hist};
                app.create_order(start_time, gosayl, market, ""sv, Side::Sell, 10_lts, 12348_tks,
                                 1_lts, resp);
            }
            {
                resp.clear();
                Recorder tr{maker_hist};
                app.create_order(start_time, marayl, market, ""sv, Side::Sell, 10_lts, 12348_tks,
                                 1_lts, resp);
            }
            {
                resp.clear();
                Recorder tr{maker_hist};
                app.create_order(start_time, gosayl, market, ""sv, Side::Sell, 10_lts, 12347_tks,
                                 1_lts, resp);
            }
            {
                resp.clear();
                Recorder tr{maker_hist};
                app.create_order(start_time, marayl, market, ""sv, Side::Sell, 5_lts, 12347_tks,
                                 1_lts, resp);
            }
            {
                resp.clear();
                Recorder tr{maker_hist};
                app.create_order(start_time, gosayl, market, ""sv, Side::Sell, 5_lts, 12346_tks,
                                 1_lts, resp);
            }

            // Maker buy-side.
            {
                resp.clear();
                Recorder tr{maker_hist};
                app.create_order(start_time, marayl, market, ""sv, Side::Buy, 5_lts, 12344_tks,
                                 1_lts, resp);
            }
            {
                resp.clear();
                Recorder tr{maker_hist};
                app.create_order(start_time, gosayl, market, ""sv, Side::Buy, 5_lts, 12343_tks,
                                 1_lts, resp);
            }
            {
                resp.clear();
                Recorder tr{maker_hist};
                app.create_order(start_time, marayl, market, ""sv, Side::Buy, 10_lts, 12343_tks,
                                 1_lts, resp);
            }
            {
                resp.clear();
                Recorder tr{maker_hist};
                app.create_order(start_time, gosayl, market, ""sv, Side::Buy, 10_lts, 12342_tks,
                                 1_lts, resp);
            }
            {
                resp.clear();
                Recorder tr{maker_hist};
                app.create_order(start_time, marayl, market, ""sv, Side::Buy, 10_lts, 12342_tks,
                                 1_lts, resp);
            }

            // Taker sell-side.
            {
                resp.clear();
                Recorder tr{taker_hist};
                app.create_order(start_time, eddayl, market, ""sv, Side::Sell, 40_lts, 12342_tks,
                                 1_lts, resp);
            }

            // Taker buy-side.
            {
                resp.clear();
                Recorder tr{taker_hist};
                app.create_order(start_time, pipayl, market, ""sv, Side::Buy, 40_lts, 12348_tks,
                                 1_lts, resp);
            }

            arch(start_time, eddayl, market.id());
            arch(start_time, gosayl, market.id());
            arch(start_time, marayl, market.id());
            arch(start_time, pipayl, market.id());
        }

        maker_hist.percentiles_print("lob-maker.hdr", 5, 1000);
        taker_hist.percentiles_print("lob-taker.hdr", 5, 1000);
        ret = 0;

    } catch (const exception& e) {
        SWIRLY_ERROR << "exception: " << e.what();
    }
    return ret;
}

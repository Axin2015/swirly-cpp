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

#include <swirly/sqlite/Journ.hpp>
#include <swirly/sqlite/Model.hpp>

#include <swirly/lob/Accnt.hpp>
#include <swirly/lob/Response.hpp>
#include <swirly/lob/Serv.hpp>
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

const Market& createMarket(Serv& serv, Symbol instrSymbol, JDay settlDay, MarketState state,
                           Time now)
{
    const auto& instr = serv.instr(instrSymbol);
    const auto marketId = toMarketId(instr.id(), settlDay);
    auto it = serv.markets().find(marketId);
    if (it != serv.markets().end()) {
        return *it;
    }
    return serv.createMarket(instr, settlDay, state, now);
}

class Archiver {
  public:
    explicit Archiver(Serv& serv) noexcept
    : serv_(serv)
    {
    }
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

class NullJourn : public Journ {
  public:
    NullJourn() noexcept = default;
    ~NullJourn() override = default;

  protected:
    void doWrite(const Msg& msg) override {}
};

MemCtx memCtx;

} // namespace

namespace swirly {

void* alloc(size_t size)
{
    return memCtx.alloc(size);
}

void* alloc(size_t size, align_val_t al)
{
    return memCtx.alloc(size, al);
}

void dealloc(void* ptr, size_t size) noexcept
{
    return memCtx.dealloc(ptr, size);
}

} // namespace swirly

int main(int argc, char* argv[])
{
    int ret = 1;
    try {

        memCtx = MemCtx{1 << 20};

        unique_ptr<Model> model;
        if (argc > 1) {
            Config config;
            config.set("sqlite_model", argv[1]);
            model = make_unique<SqlModel>(config);
        } else {
            model = make_unique<TestModel>();
        }

        const BusinessDay busDay{MarketZone};
        const auto startTime = UnixClock::now();

        MsgQueue mq{1 << 10};
        Serv serv{mq, 1 << 4};
        serv.load(*model, startTime);
        model = nullptr;

        NullJourn journ;
        auto journAgent = [&mq, &journ]() {
            int n{0};
            Msg msg;
            while (mq.pop(msg)) {
                journ.write(msg);
                ++n;
            }
            return n;
        };
        AgentThread journThread{journAgent, ThreadConfig{"journ"s}};

        auto& market = createMarket(serv, "EURUSD"sv, busDay(startTime), 0, startTime);

        auto& eddayl = serv.accnt("EDDAYL"sv);
        auto& gosayl = serv.accnt("GOSAYL"sv);
        auto& marayl = serv.accnt("MARAYL"sv);
        auto& pipayl = serv.accnt("PIPAYL"sv);

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
                serv.createOrder(gosayl, market, ""sv, Side::Sell, 10_lts, 12348_tks, 1_lts,
                                 startTime, resp);
            }
            {
                HdrRecorder tr{maker};
                resp.clear();
                serv.createOrder(marayl, market, ""sv, Side::Sell, 10_lts, 12348_tks, 1_lts,
                                 startTime, resp);
            }
            {
                HdrRecorder tr{maker};
                resp.clear();
                serv.createOrder(gosayl, market, ""sv, Side::Sell, 10_lts, 12347_tks, 1_lts,
                                 startTime, resp);
            }
            {
                HdrRecorder tr{maker};
                resp.clear();
                serv.createOrder(marayl, market, ""sv, Side::Sell, 5_lts, 12347_tks, 1_lts,
                                 startTime, resp);
            }
            {
                HdrRecorder tr{maker};
                resp.clear();
                serv.createOrder(gosayl, market, ""sv, Side::Sell, 5_lts, 12346_tks, 1_lts,
                                 startTime, resp);
            }

            // Maker buy-side.
            {
                HdrRecorder tr{maker};
                resp.clear();
                serv.createOrder(marayl, market, ""sv, Side::Buy, 5_lts, 12344_tks, 1_lts,
                                 startTime, resp);
            }
            {
                HdrRecorder tr{maker};
                resp.clear();
                serv.createOrder(gosayl, market, ""sv, Side::Buy, 5_lts, 12343_tks, 1_lts,
                                 startTime, resp);
            }
            {
                HdrRecorder tr{maker};
                resp.clear();
                serv.createOrder(marayl, market, ""sv, Side::Buy, 10_lts, 12343_tks, 1_lts,
                                 startTime, resp);
            }
            {
                HdrRecorder tr{maker};
                resp.clear();
                serv.createOrder(gosayl, market, ""sv, Side::Buy, 10_lts, 12342_tks, 1_lts,
                                 startTime, resp);
            }
            {
                HdrRecorder tr{maker};
                resp.clear();
                serv.createOrder(marayl, market, ""sv, Side::Buy, 10_lts, 12342_tks, 1_lts,
                                 startTime, resp);
            }

            // Taker sell-side.
            {
                HdrRecorder tr{taker};
                resp.clear();
                serv.createOrder(eddayl, market, ""sv, Side::Sell, 40_lts, 12342_tks, 1_lts,
                                 startTime, resp);
            }

            // Taker buy-side.
            {
                HdrRecorder tr{taker};
                resp.clear();
                serv.createOrder(pipayl, market, ""sv, Side::Buy, 40_lts, 12348_tks, 1_lts,
                                 startTime, resp);
            }

            arch(eddayl, market.id(), startTime);
            arch(gosayl, market.id(), startTime);
            arch(marayl, market.id(), startTime);
            arch(pipayl, market.id(), startTime);
        }

        SWIRLY_NOTICE << "Maker percentile report"sv;
        maker.print(stdout, 1, 1000);

        SWIRLY_NOTICE << "Taker percentile report"sv;
        taker.print(stdout, 1, 1000);

        fflush(stdout);
        ret = 0;
    } catch (const exception& e) {
        SWIRLY_ERROR << "exception: "sv << e.what();
    }
    return ret;
}

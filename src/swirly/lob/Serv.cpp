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
#include "Serv.hpp"

#include <swirly/lob/Response.hpp>
#include <swirly/lob/Sess.hpp>

#include <swirly/fin/Date.hpp>
#include <swirly/fin/Exception.hpp>
#include <swirly/fin/Journ.hpp>
#include <swirly/fin/Model.hpp>
#include <swirly/fin/MsgQueue.hpp>

#include <swirly/util/Date.hpp>
#include <swirly/util/Finally.hpp>

#include "Match.hxx"

#include <regex>

namespace swirly {
inline namespace lob {
using namespace std;
namespace {

const regex SymbolPattern{R"(^[0-9A-Za-z-._]{3,16}$)"};

Ticks spread(const Order& taker_order, const Order& maker_order, Direct direct) noexcept
{
    return direct == Direct::Paid
        // Paid when the taker lifts the offer.
        ? maker_order.ticks() - taker_order.ticks()
        // Given when the taker hits the bid.
        : taker_order.ticks() - maker_order.ticks();
}

template <typename ValueT>
inline auto& remove_const(const ValueT& ref)
{
    return const_cast<ValueT&>(ref);
}

} // namespace

struct Serv::Impl {

    Impl(MsgQueue& mq, size_t max_execs)
    : mq_(mq)
    , max_execs_{max_execs}
    {
        matches_.reserve(8);
        execs_.reserve(1 + 16);
    }

    void load(const Model& model, Time now)
    {
        const auto bus_day = bus_day_(now);
        model.read_asset([& assets = assets_](auto ptr) { assets.insert(move(ptr)); });
        model.read_instr([& instrs = instrs_](auto ptr) { instrs.insert(move(ptr)); });
        model.read_market([& markets = markets_](MarketPtr ptr) { markets.insert(ptr); });
        model.read_order([this](auto ptr) {
            auto& sess = this->sess(ptr->accnt());
            sess.insert_order(ptr);
            bool success{false};
            const auto finally = make_finally([&]() noexcept {
                if (!success) {
                    sess.remove_order(*ptr);
                }
            });
            auto it = this->markets_.find(ptr->market_id());
            assert(it != this->markets_.end());
            it->insert_order(ptr);
            success = true;
        });
        // One week ago.
        model.read_exec(now - 604800000ms, [this](auto ptr) {
            auto& sess = this->sess(ptr->accnt());
            sess.push_exec_back(ptr);
        });
        model.read_trade([this](auto ptr) {
            auto& sess = this->sess(ptr->accnt());
            sess.insert_trade(ptr);
        });
        model.read_posn(bus_day, [this](auto ptr) {
            auto& sess = this->sess(ptr->accnt());
            sess.insert_posn(ptr);
        });
    }

    const AssetSet& assets() const noexcept { return assets_; }

    const Instr& instr(Symbol symbol) const
    {
        auto it = instrs_.find(symbol);
        if (it == instrs_.end()) {
            throw MarketNotFoundException{err_msg()
                                          << "instrument '" << symbol << "' does not exist"};
        }
        return *it;
    }

    const InstrSet& instrs() const noexcept { return instrs_; }

    const Sess& sess(Symbol accnt) const
    {
        SessSet::ConstIterator it;
        bool found;
        tie(it, found) = sesss_.find_hint(accnt);
        if (!found) {
            it = sesss_.insert_hint(it, Sess::make(accnt, max_execs_));
        }
        return *it;
    }

    const Market& market(Id64 id) const
    {
        auto it = markets_.find(id);
        if (it == markets_.end()) {
            throw MarketNotFoundException{err_msg() << "market '" << id << "' does not exist"};
        }
        return *it;
    }

    const MarketSet& markets() const noexcept { return markets_; }

    Sess& sess(Symbol accnt)
    {
        SessSet::Iterator it;
        bool found;
        tie(it, found) = sesss_.find_hint(accnt);
        if (!found) {
            it = sesss_.insert_hint(it, Sess::make(accnt, max_execs_));
        }
        return *it;
    }

    const Market& create_market(const Instr& instr, JDay settl_day, MarketState state, Time now)
    {
        if (settl_day != 0_jd) {
            // bus_day <= settl_day.
            const auto bus_day = bus_day_(now);
            if (settl_day < bus_day) {
                throw InvalidException{"settl-day before bus-day"sv};
            }
        }
        const auto id = to_market_id(instr.id(), settl_day);

        MarketSet::Iterator it;
        bool found;
        tie(it, found) = markets_.find_hint(id);
        if (found) {
            throw AlreadyExistsException{err_msg() << "market for '" << instr.symbol() << "' on "
                                                   << jd_to_iso(settl_day) << " already exists"};
        }
        {
            auto market = Market::make(id, instr.symbol(), settl_day, state);
            mq_.create_market(id, instr.symbol(), settl_day, state);
            it = markets_.insert_hint(it, market);
        }
        return *it;
    }

    void update_market(Market& market, MarketState state, Time now)
    {
        mq_.update_market(market.id(), state);
        market.set_state(state);
    }

    void create_order(Sess& sess, Market& market, string_view ref, Side side, Lots lots,
                      Ticks ticks, Lots min_lots, Time now, Response& resp)
    {
        // N.B. we only check for duplicates in the ref_idx; no unique constraint exists in the database,
        // and order-refs can be reused so long as only one order is live in the system at any given
        // time.
        if (!ref.empty() && sess.exists(ref)) {
            throw RefAlreadyExistsException{err_msg() << "order '" << ref << "' already exists"};
        }

        const auto bus_day = bus_day_(now);
        if (market.settl_day() != 0_jd && market.settl_day() < bus_day) {
            throw MarketClosedException{err_msg()
                                        << "market for '" << market.instr() << "' on "
                                        << jd_to_iso(market.settl_day()) << " has closed"};
        }
        if (lots == 0_lts || lots < min_lots) {
            throw InvalidLotsException{err_msg() << "invalid lots '" << lots << '\''};
        }
        const auto id = market.alloc_id();
        auto order = Order::make(sess.accnt(), market.id(), market.instr(), market.settl_day(), id,
                                 ref, side, lots, ticks, min_lots, now);
        auto exec = new_exec(*order, id, now);

        resp.insert_order(order);
        resp.insert_exec(exec);

        // Ensure that matches are cleared when scope exits.
        const auto finally = make_finally([this]() noexcept {
            this->matches_.clear();
            this->execs_.clear();
        });
        execs_.push_back(exec);
        // Order fields are updated on match.
        match_orders(sess, market, *order, now, resp);

        resp.set_market(&market);

        // Avoid allocating position when there are no matches.
        PosnPtr posn;
        const bool have_matches = !matches_.empty();
        if (have_matches) {
            // Avoid allocating position when there are no matches.
            // N.B. before commit phase, because this may fail.
            posn = sess.posn(market.id(), market.instr(), market.settl_day());
            resp.set_posn(posn);
        }

        // Place incomplete order in market.
        if (!order->done()) {
            // This may fail if level cannot be allocated.
            market.insert_order(order);
        }
        {
            // TODO: IOC orders would need an additional revision for the unsolicited cancellation
            // of any unfilled quantity.
            bool success{false};
            // clang-format off
            const auto finally = make_finally([&market, &order, &success]() noexcept {
                if (!success && !order->done()) {
                    // Undo market insertion.
                    market.remove_order(*order);
                }
            });
            // clang-format on

            mq_.create_exec(execs_);
            success = true;
        }

        // Commit phase.

        if (!order->done()) {
            sess.insert_order(order);
        }
        sess.push_exec_front(exec);

        // Commit matches.
        if (have_matches) {
            assert(posn);
            commit_matches(sess, market, *posn, now);
        }
    }

    void revise_order(Sess& sess, Market& market, Order& order, Lots lots, Time now, Response& resp)
    {
        if (order.done()) {
            throw TooLateException{err_msg() << "order '" << order.id() << "' is done"};
        }
        do_revise_order(sess, market, order, lots, now, resp);
    }

    void revise_order(Sess& sess, Market& market, Id64 id, Lots lots, Time now, Response& resp)
    {
        auto& order = sess.order(market.id(), id);
        if (order.done()) {
            throw TooLateException{err_msg() << "order '" << order.id() << "' is done"};
        }
        do_revise_order(sess, market, order, lots, now, resp);
    }

    void revise_order(Sess& sess, Market& market, string_view ref, Lots lots, Time now,
                      Response& resp)
    {
        auto& order = sess.order(ref);
        if (order.done()) {
            throw TooLateException{err_msg() << "order '" << order.id() << "' is done"};
        }
        do_revise_order(sess, market, order, lots, now, resp);
    }

    void revise_order(Sess& sess, Market& market, ArrayView<Id64> ids, Lots lots, Time now,
                      Response& resp)
    {
        resp.set_market(&market);
        for (const auto id : ids) {

            auto& order = sess.order(market.id(), id);
            if (order.done()) {
                throw TooLateException{err_msg() << "order '" << order.id() << "' is done"};
            }
            // Revised lots must not be:
            // 1. greater than original lots;
            // 2. less than executed lots;
            // 3. less than min lots.
            if (lots == 0_lts               //
                || lots > order.lots()      //
                || lots < order.exec_lots() //
                || lots < order.min_lots()) {
                throw InvalidLotsException{err_msg() << "invalid lots '" << lots << '\''};
            }
            auto exec = new_exec(order, market.alloc_id(), now);
            exec->revise(lots);

            resp.insert_order(&order);
            resp.insert_exec(exec);
        }

        mq_.create_exec(resp.execs());

        // Commit phase.

        for (const auto& exec : resp.execs()) {
            auto it = sess.orders().find(market.id(), exec->order_id());
            assert(it != sess.orders().end());
            market.revise_order(*it, lots, now);
            sess.push_exec_front(exec);
        }
    }

    void cancel_order(Sess& sess, Market& market, Order& order, Time now, Response& resp)
    {
        if (order.done()) {
            throw TooLateException{err_msg() << "order '" << order.id() << "' is done"};
        }
        do_cancel_order(sess, market, order, now, resp);
    }

    void cancel_order(Sess& sess, Market& market, Id64 id, Time now, Response& resp)
    {
        auto& order = sess.order(market.id(), id);
        if (order.done()) {
            throw TooLateException{err_msg() << "order '" << order.id() << "' is done"};
        }
        do_cancel_order(sess, market, order, now, resp);
    }

    void cancel_order(Sess& sess, Market& market, string_view ref, Time now, Response& resp)
    {
        auto& order = sess.order(ref);
        if (order.done()) {
            throw TooLateException{err_msg() << "order '" << order.id() << "' is done"};
        }
        do_cancel_order(sess, market, order, now, resp);
    }

    void cancel_order(Sess& sess, Market& market, ArrayView<Id64> ids, Time now, Response& resp)
    {
        resp.set_market(&market);
        for (const auto id : ids) {

            auto& order = sess.order(market.id(), id);
            if (order.done()) {
                throw TooLateException{err_msg() << "order '" << order.id() << "' is done"};
            }
            auto exec = new_exec(order, market.alloc_id(), now);
            exec->cancel();

            resp.insert_order(&order);
            resp.insert_exec(exec);
        }

        mq_.create_exec(resp.execs());

        // Commit phase.

        for (const auto& exec : resp.execs()) {
            auto it = sess.orders().find(market.id(), exec->order_id());
            assert(it != sess.orders().end());
            market.cancel_order(*it, now);
            sess.remove_order(*it);
            sess.push_exec_front(exec);
        }
    }

    void cancel_order(Sess& sess, Time now)
    {
        // FIXME: Not implemented.
    }

    void cancel_order(Market& market, Time now)
    {
        // FIXME: Not implemented.
    }

    TradePair create_trade(Sess& sess, Market& market, string_view ref, Side side, Lots lots,
                           Ticks ticks, LiqInd liq_ind, Symbol cpty, Time created)
    {
        auto posn = sess.posn(market.id(), market.instr(), market.settl_day());
        auto trade = new_manual(sess.accnt(), market, ref, side, lots, ticks, posn->net_lots(),
                                posn->net_cost(), liq_ind, cpty, created);
        decltype(trade) cpty_trade;

        if (!cpty.empty()) {

            // Create back-to-back trade if counter-party is specified.
            auto& cpty_sess = this->sess(cpty);
            auto cpty_posn = cpty_sess.posn(market.id(), market.instr(), market.settl_day());
            cpty_trade = trade->opposite(market.alloc_id());

            ConstExecPtr trades[] = {trade, cpty_trade};
            mq_.create_exec(trades);

            // Commit phase.

            cpty_sess.push_exec_front(cpty_trade);
            cpty_sess.insert_trade(cpty_trade);
            cpty_posn->add_trade(cpty_trade->side(), cpty_trade->last_lots(),
                                 cpty_trade->last_ticks());

        } else {

            mq_.create_exec(*trade);

            // Commit phase.
        }
        sess.push_exec_front(trade);
        sess.insert_trade(trade);
        posn->add_trade(trade->side(), trade->last_lots(), trade->last_ticks());

        return {trade, cpty_trade};
    }

    void archive_trade(Sess& sess, const Exec& trade, Time now)
    {
        if (trade.state() != State::Trade) {
            throw InvalidException{err_msg() << "exec '" << trade.id() << "' is not a trade"};
        }
        do_archive_trade(sess, trade, now);
    }

    void archive_trade(Sess& sess, Id64 market_id, Id64 id, Time now)
    {
        auto& trade = sess.trade(market_id, id);
        do_archive_trade(sess, trade, now);
    }

    void archive_trade(Sess& sess, Id64 market_id, ArrayView<Id64> ids, Time now)
    {
        // Validate.
        for (const auto id : ids) {
            sess.trade(market_id, id);
        }

        mq_.archive_trade(market_id, ids, now);

        // Commit phase.

        for (const auto id : ids) {

            auto it = sess.trades().find(market_id, id);
            assert(it != sess.trades().end());
            sess.remove_trade(*it);
        }
    }

    void expire_end_of_day(Time now)
    {
        // FIXME: Not implemented.
    }

    void settl_end_of_day(Time now)
    {
        // FIXME: Not implemented.
    }

  private:
    ExecPtr new_exec(const Order& order, Id64 id, Time created) const
    {
        return Exec::make(order.accnt(), order.market_id(), order.instr(), order.settl_day(), id,
                          order.id(), order.ref(), order.state(), order.side(), order.lots(),
                          order.ticks(), order.resd_lots(), order.exec_lots(), order.exec_cost(),
                          order.last_lots(), order.last_ticks(), order.min_lots(), 0_id64, 0_lts,
                          0_cst, LiqInd::None, Symbol{}, created);
    }

    /**
     * Special factory method for manual trades.
     */
    ExecPtr new_manual(Id64 market_id, Symbol instr, JDay settl_day, Id64 id, Symbol accnt,
                       string_view ref, Side side, Lots lots, Ticks ticks, Lots posn_lots,
                       Cost posn_cost, LiqInd liq_ind, Symbol cpty, Time created) const
    {
        const auto order_id = 0_id64;
        const auto state = State::Trade;
        const auto resd = 0_lts;
        const auto exec = lots;
        const auto cost = swirly::cost(lots, ticks);
        const auto last_lots = lots;
        const auto last_ticks = ticks;
        const auto min_lots = 1_lts;
        const auto match_id = 0_id64;
        return Exec::make(accnt, market_id, instr, settl_day, id, order_id, ref, state, side, lots,
                          ticks, resd, exec, cost, last_lots, last_ticks, min_lots, match_id,
                          posn_lots, posn_cost, liq_ind, cpty, created);
    }

    ExecPtr new_manual(Symbol accnt, Market& market, string_view ref, Side side, Lots lots,
                       Ticks ticks, Lots posn_lots, Cost posn_cost, LiqInd liq_ind, Symbol cpty,
                       Time created) const
    {
        return new_manual(market.id(), market.instr(), market.settl_day(), market.alloc_id(), accnt,
                          ref, side, lots, ticks, posn_lots, posn_cost, liq_ind, cpty, created);
    }

    Match new_match(Market& market, const Order& taker_order, const OrderPtr& maker_order,
                    Lots lots, Lots sum_lots, Cost sum_cost, Time created)
    {
        const auto maker_id = market.alloc_id();
        const auto taker_id = market.alloc_id();

        auto it = sesss_.find(maker_order->accnt());
        assert(it != sesss_.end());
        auto& maker_sess = *it;
        auto maker_posn = maker_sess.posn(market.id(), market.instr(), market.settl_day());

        const auto ticks = maker_order->ticks();

        auto maker_trade = new_exec(*maker_order, maker_id, created);
        maker_trade->trade(lots, ticks, taker_id, LiqInd::Maker, taker_order.accnt());

        auto taker_trade = new_exec(taker_order, taker_id, created);
        taker_trade->trade(sum_lots, sum_cost, lots, ticks, maker_id, LiqInd::Taker,
                           maker_order->accnt());

        return {lots, maker_order, maker_trade, maker_posn, taker_trade};
    }

    void match_orders(const Sess& taker_sess, Market& market, Order& taker_order, MarketSide& side,
                      Direct direct, Time now, Response& resp)
    {
        auto sum_lots = 0_lts;
        auto sum_cost = 0_cst;
        auto last_lots = 0_lts;
        auto last_ticks = 0_tks;

        for (auto& maker_order : side.orders()) {
            // Break if order is fully filled.
            if (sum_lots == taker_order.resd_lots()) {
                break;
            }
            // Only consider orders while prices cross.
            if (spread(taker_order, maker_order, direct) > 0_tks) {
                break;
            }

            const auto lots = min(taker_order.resd_lots() - sum_lots, maker_order.resd_lots());
            const auto ticks = maker_order.ticks();

            sum_lots += lots;
            sum_cost += cost(lots, ticks);
            last_lots = lots;
            last_ticks = ticks;

            auto match
                = new_match(market, taker_order, &maker_order, lots, sum_lots, sum_cost, now);

            // Insert order if trade crossed with self.
            if (maker_order.accnt() == taker_sess.accnt()) {
                // Maker updated first because this is consistent with last-look semantics.
                // N.B. the reference count is not incremented here.
                resp.insert_order(&maker_order);
                resp.insert_exec(match.maker_trade);
            }
            resp.insert_exec(match.taker_trade);

            execs_.push_back(match.maker_trade);
            execs_.push_back(match.taker_trade);
            matches_.push_back(move(match));
        }

        if (!matches_.empty()) {
            taker_order.trade(sum_lots, sum_cost, last_lots, last_ticks, now);
        }
    }

    void match_orders(const Sess& taker_sess, Market& market, Order& taker_order, Time now,
                      Response& resp)
    {
        MarketSide* market_side;
        Direct direct;
        if (taker_order.side() == Side::Buy) {
            // Paid when the taker lifts the offer.
            market_side = &market.offer_side();
            direct = Direct::Paid;
        } else {
            assert(taker_order.side() == Side::Sell);
            // Given when the taker hits the bid.
            market_side = &market.bid_side();
            direct = Direct::Given;
        }
        match_orders(taker_sess, market, taker_order, *market_side, direct, now, resp);
    }

    // Assumes that maker lots have not been reduced since matching took place. N.B. this function is
    // responsible for committing a transaction, so it is particularly important that it does not
    // throw.
    void commit_matches(Sess& taker_sess, Market& market, Posn& taker_posn, Time now) noexcept
    {
        for (const auto& match : matches_) {

            const auto maker_order = match.maker_order;
            assert(maker_order);

            // Reduce maker.
            market.take_order(*maker_order, match.lots, now);

            // Must succeed because maker order exists.
            auto it = sesss_.find(maker_order->accnt());
            assert(it != sesss_.end());
            auto& maker_sess = *it;

            // Maker updated first because this is consistent with last-look semantics.

            // Update maker position.
            const auto maker_trade = match.maker_trade;
            assert(maker_trade);
            maker_trade->posn(match.maker_posn->net_lots(), match.maker_posn->net_cost());
            match.maker_posn->add_trade(maker_trade->side(), maker_trade->last_lots(),
                                        maker_trade->last_ticks());

            // Update maker account.
            maker_sess.push_exec_front(maker_trade);
            maker_sess.insert_trade(maker_trade);
            if (maker_order->done()) {
                maker_sess.remove_order(*maker_order);
            }

            // Update taker position.
            const auto taker_trade = match.taker_trade;
            assert(taker_trade);
            taker_trade->posn(taker_posn.net_lots(), taker_posn.net_cost());
            taker_posn.add_trade(taker_trade->side(), taker_trade->last_lots(),
                                 taker_trade->last_ticks());

            // Update taker account.
            taker_sess.push_exec_front(taker_trade);
            taker_sess.insert_trade(taker_trade);
        }
    }

    void do_revise_order(Sess& sess, Market& market, Order& order, Lots lots, Time now,
                         Response& resp)
    {
        // Revised lots must not be:
        // 1. greater than original lots;
        // 2. less than executed lots;
        // 3. less than min lots.
        if (lots == 0_lts               //
            || lots > order.lots()      //
            || lots < order.exec_lots() //
            || lots < order.min_lots()) {
            throw InvalidLotsException{err_msg() << "invalid lots '" << lots << '\''};
        }
        auto exec = new_exec(order, market.alloc_id(), now);
        exec->revise(lots);

        resp.set_market(&market);
        resp.insert_order(&order);
        resp.insert_exec(exec);

        mq_.create_exec(*exec);

        // Commit phase.

        market.revise_order(order, lots, now);
        sess.push_exec_front(exec);
    }
    void do_cancel_order(Sess& sess, Market& market, Order& order, Time now, Response& resp)
    {
        auto exec = new_exec(order, market.alloc_id(), now);
        exec->cancel();

        resp.set_market(&market);
        resp.insert_order(&order);
        resp.insert_exec(exec);

        mq_.create_exec(*exec);

        // Commit phase.

        market.cancel_order(order, now);
        sess.remove_order(order);
        sess.push_exec_front(exec);
    }

    void do_archive_trade(Sess& sess, const Exec& trade, Time now)
    {
        mq_.archive_trade(trade.market_id(), trade.id(), now);

        // Commit phase.

        sess.remove_trade(trade);
    }

    MsgQueue& mq_;
    const BusinessDay bus_day_{MarketZone};
    const size_t max_execs_;
    AssetSet assets_;
    InstrSet instrs_;
    MarketSet markets_;
    mutable SessSet sesss_;
    vector<Match> matches_;
    vector<ConstExecPtr> execs_;
};

Serv::Serv(MsgQueue& mq, size_t max_execs)
: impl_{make_unique<Impl>(mq, max_execs)}
{
}

Serv::~Serv() = default;

Serv::Serv(Serv&&) = default;

Serv& Serv::operator=(Serv&&) = default;

void Serv::load(const Model& model, Time now)
{
    impl_->load(model, now);
}

const AssetSet& Serv::assets() const noexcept
{
    return impl_->assets();
}

const InstrSet& Serv::instrs() const noexcept
{
    return impl_->instrs();
}

const MarketSet& Serv::markets() const noexcept
{
    return impl_->markets();
}

const Instr& Serv::instr(Symbol symbol) const
{
    return impl_->instr(symbol);
}

const Market& Serv::market(Id64 id) const
{
    return impl_->market(id);
}

const Sess& Serv::sess(Symbol accnt) const
{
    return impl_->sess(accnt);
}

const Market& Serv::create_market(const Instr& instr, JDay settl_day, MarketState state, Time now)
{
    return impl_->create_market(instr, settl_day, state, now);
}

void Serv::update_market(const Market& market, MarketState state, Time now)
{
    return impl_->update_market(remove_const(market), state, now);
}

void Serv::create_order(const Sess& sess, const Market& market, string_view ref, Side side,
                        Lots lots, Ticks ticks, Lots min_lots, Time now, Response& resp)
{
    impl_->create_order(remove_const(sess), remove_const(market), ref, side, lots, ticks, min_lots,
                        now, resp);
}

void Serv::revise_order(const Sess& sess, const Market& market, const Order& order, Lots lots,
                        Time now, Response& resp)
{
    impl_->revise_order(remove_const(sess), remove_const(market), remove_const(order), lots, now,
                        resp);
}

void Serv::revise_order(const Sess& sess, const Market& market, Id64 id, Lots lots, Time now,
                        Response& resp)
{
    impl_->revise_order(remove_const(sess), remove_const(market), id, lots, now, resp);
}

void Serv::revise_order(const Sess& sess, const Market& market, string_view ref, Lots lots,
                        Time now, Response& resp)
{
    impl_->revise_order(remove_const(sess), remove_const(market), ref, lots, now, resp);
}

void Serv::revise_order(const Sess& sess, const Market& market, ArrayView<Id64> ids, Lots lots,
                        Time now, Response& resp)
{
    impl_->revise_order(remove_const(sess), remove_const(market), ids, lots, now, resp);
}

void Serv::cancel_order(const Sess& sess, const Market& market, const Order& order, Time now,
                        Response& resp)
{
    impl_->cancel_order(remove_const(sess), remove_const(market), remove_const(order), now, resp);
}

void Serv::cancel_order(const Sess& sess, const Market& market, Id64 id, Time now, Response& resp)
{
    impl_->cancel_order(remove_const(sess), remove_const(market), id, now, resp);
}

void Serv::cancel_order(const Sess& sess, const Market& market, string_view ref, Time now,
                        Response& resp)
{
    impl_->cancel_order(remove_const(sess), remove_const(market), ref, now, resp);
}

void Serv::cancel_order(const Sess& sess, const Market& market, ArrayView<Id64> ids, Time now,
                        Response& resp)
{
    impl_->cancel_order(remove_const(sess), remove_const(market), ids, now, resp);
}

void Serv::cancel_order(const Sess& sess, Time now)
{
    impl_->cancel_order(remove_const(sess), now);
}

void Serv::cancel_order(const Market& market, Time now)
{
    impl_->cancel_order(remove_const(market), now);
}

TradePair Serv::create_trade(const Sess& sess, const Market& market, string_view ref, Side side,
                             Lots lots, Ticks ticks, LiqInd liq_ind, Symbol cpty, Time created)
{
    return impl_->create_trade(remove_const(sess), remove_const(market), ref, side, lots, ticks,
                               liq_ind, cpty, created);
}

void Serv::archive_trade(const Sess& sess, const Exec& trade, Time now)
{
    impl_->archive_trade(remove_const(sess), trade, now);
}

void Serv::archive_trade(const Sess& sess, Id64 market_id, Id64 id, Time now)
{
    impl_->archive_trade(remove_const(sess), market_id, id, now);
}

void Serv::archive_trade(const Sess& sess, Id64 market_id, ArrayView<Id64> ids, Time now)
{
    impl_->archive_trade(remove_const(sess), market_id, ids, now);
}

void Serv::expire_end_of_day(Time now)
{
    impl_->expire_end_of_day(now);
}

void Serv::settl_end_of_day(Time now)
{
    impl_->settl_end_of_day(now);
}

} // namespace lob
} // namespace swirly

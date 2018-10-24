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
#ifndef SWIRLY_LOB_SESS_HPP
#define SWIRLY_LOB_SESS_HPP

#include <swirly/lob/Types.hpp>

#include <swirly/fin/Exec.hpp>
#include <swirly/fin/MarketId.hpp>
#include <swirly/fin/Order.hpp>
#include <swirly/fin/Posn.hpp>

#include <swirly/app/Exception.hpp>

#include <swirly/util/Set.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <boost/circular_buffer.hpp>
#pragma GCC diagnostic pop

namespace swirly {
inline namespace lob {

using SessPtr = std::unique_ptr<Sess>;
using ConstSessPtr = std::unique_ptr<const Sess>;

class SWIRLY_API Sess : protected Comparable<Sess> {
  public:
    Sess(Symbol accnt, std::size_t max_execs) noexcept
    : accnt_{accnt}
    , execs_{max_execs}
    {
    }
    ~Sess();

    // Copy.
    Sess(const Sess&) = delete;
    Sess& operator=(const Sess&) = delete;

    // Move.
    Sess(Sess&&);
    Sess& operator=(Sess&&) = delete;

    template <typename... ArgsT>
    static SessPtr make(ArgsT&&... args)
    {
        return std::make_unique<Sess>(std::forward<ArgsT>(args)...);
    }

    int compare(const Sess& rhs) const noexcept { return accnt_.compare(rhs.accnt_); }
    bool exists(std::string_view ref) const noexcept
    {
        return ref_idx_.find(ref) != ref_idx_.end();
    }
    auto accnt() const noexcept { return accnt_; }
    const auto& orders() const noexcept { return orders_; }
    const Order& order(Id64 market_id, Id64 id) const
    {
        auto it = orders_.find(market_id, id);
        if (it == orders_.end()) {
            throw OrderNotFoundException{err_msg() << "order '" << id << "' does not exist"};
        }
        return *it;
    }
    const Order& order(std::string_view ref) const
    {
        auto it = ref_idx_.find(ref);
        if (it == ref_idx_.end()) {
            throw OrderNotFoundException{err_msg() << "order '" << ref << "' does not exist"};
        }
        return *it;
    }
    const auto& execs() const noexcept { return execs_; }
    const auto& trades() const noexcept { return trades_; }
    const Exec& trade(Id64 market_id, Id64 id) const
    {
        auto it = trades_.find(market_id, id);
        if (it == trades_.end()) {
            throw NotFoundException{err_msg() << "trade '" << id << "' does not exist"};
        }
        return *it;
    }
    const auto& posns() const noexcept { return posns_; }

    void set_trade_slot(TradeSlot slot) noexcept { trade_slot_ = slot; }
    auto& orders() noexcept { return orders_; }
    Order& order(Id64 market_id, Id64 id)
    {
        auto it = orders_.find(market_id, id);
        if (it == orders_.end()) {
            throw OrderNotFoundException{err_msg() << "order '" << id << "' does not exist"};
        }
        return *it;
    }
    Order& order(std::string_view ref)
    {
        auto it = ref_idx_.find(ref);
        if (it == ref_idx_.end()) {
            throw OrderNotFoundException{err_msg() << "order '" << ref << "' does not exist"};
        }
        return *it;
    }
    void insert_order(const OrderPtr& order) noexcept
    {
        assert(order->accnt() == accnt_);
        orders_.insert(order);
        if (!order->ref().empty()) {
            ref_idx_.insert(order);
        }
    }
    OrderPtr remove_order(const Order& order) noexcept
    {
        assert(order.accnt() == accnt_);
        if (!order.ref().empty()) {
            ref_idx_.remove(order);
        }
        return orders_.remove(order);
    }
    void push_exec_back(const ConstExecPtr& exec) noexcept
    {
        assert(exec->accnt() == accnt_);
        execs_.push_back(exec);
    }
    void push_exec_front(const ConstExecPtr& exec) noexcept
    {
        assert(exec->accnt() == accnt_);
        execs_.push_front(exec);
    }
    void insert_trade(const ExecPtr& trade) noexcept
    {
        assert(trade->accnt() == accnt_);
        assert(trade->state() == State::Trade);
        trades_.insert(trade);
    }
    void insert_trade_and_notify(CyclTime now, const ExecPtr& trade) noexcept;

    ConstExecPtr remove_trade(const Exec& trade) noexcept
    {
        assert(trade.accnt() == accnt_);
        return trades_.remove(trade);
    }
    /**
     * Throws std::bad_alloc.
     */
    PosnPtr posn(Id64 market_id);

    void insert_posn(const PosnPtr& posn) noexcept
    {
        assert(posn->accnt() == accnt_);
        posns_.insert(posn);
    }
    boost::intrusive::set_member_hook<> symbol_hook;
    using PosnSet = IdSet<Posn, MarketIdTraits<Posn>>;

  private:
    const Symbol accnt_;
    TradeSlot trade_slot_;
    OrderIdSet orders_;
    boost::circular_buffer<ConstExecPtr> execs_;
    ExecIdSet trades_;
    PosnSet posns_;
    OrderRefSet ref_idx_;
};

namespace detail {
struct SessTraits {
    static Symbol symbol(const Sess& sess) noexcept { return sess.accnt(); }
};
} // namespace detail

using SessSet = SymbolSet<Sess, detail::SessTraits>;

} // namespace lob
} // namespace swirly

#endif // SWIRLY_LOB_SESS_HPP

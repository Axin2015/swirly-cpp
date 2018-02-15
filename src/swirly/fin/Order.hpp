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
#ifndef SWIRLY_FIN_ORDER_HPP
#define SWIRLY_FIN_ORDER_HPP

#include <swirly/fin/Conv.hpp>
#include <swirly/fin/Request.hpp>

#include <swirly/sys/MemAlloc.hpp>

#include <boost/intrusive/list.hpp>

namespace swirly {

class Level;

/**
 * An instruction to buy or sell goods or services.
 */
class SWIRLY_API Order
  : public RefCount<Order, ThreadUnsafePolicy>
  , public Request
  , public MemAlloc {
  public:
    Order(Symbol accnt, Id64 marketId, Symbol instr, JDay settlDay, Id64 id, std::string_view ref,
          State state, Side side, Lots lots, Ticks ticks, Lots resdLots, Lots execLots,
          Cost execCost, Lots lastLots, Ticks lastTicks, Lots minLots, Time created,
          Time modified) noexcept
      : Request{accnt, marketId, instr, settlDay, id, ref, side, lots, created}
      , state_{state}
      , ticks_{ticks}
      , resdLots_{resdLots}
      , execLots_{execLots}
      , execCost_{execCost}
      , lastLots_{lastLots}
      , lastTicks_{lastTicks}
      , minLots_{minLots}
      , modified_{modified}
    {
    }
    Order(Symbol accnt, Id64 marketId, Symbol instr, JDay settlDay, Id64 id, std::string_view ref,
          Side side, Lots lots, Ticks ticks, Lots minLots, Time created) noexcept
      : Order{accnt, marketId, instr, settlDay, id,    ref,   State::New, side,    lots,
              ticks, lots,     0_lts, 0_cst,    0_lts, 0_tks, minLots,    created, created}
    {
    }
    ~Order() noexcept;

    // Copy.
    Order(const Order&) = delete;
    Order& operator=(const Order&) = delete;

    // Move.
    Order(Order&&);
    Order& operator=(Order&&) = delete;

    template <typename... ArgsT>
    static OrderPtr make(ArgsT&&... args)
    {
        return makeIntrusive<Order>(std::forward<ArgsT>(args)...);
    }

    void toCsv(std::ostream& os, char delim = ',') const;
    void toJson(std::ostream& os) const;

    auto* level() const noexcept { return level_; }
    auto state() const noexcept { return state_; }
    auto ticks() const noexcept { return ticks_; }
    auto resdLots() const noexcept { return resdLots_; }
    auto execLots() const noexcept { return execLots_; }
    auto execCost() const noexcept { return execCost_; }
    auto lastLots() const noexcept { return lastLots_; }
    auto lastTicks() const noexcept { return lastTicks_; }
    auto minLots() const noexcept { return minLots_; }
    auto done() const noexcept { return resdLots_ == 0_lts; }
    auto modified() const noexcept { return modified_; }
    void setLevel(Level* level) const noexcept { level_ = level; }
    void create(Time now) noexcept
    {
        assert(lots_ > 0_lts);
        assert(lots_ >= minLots_);
        state_ = State::New;
        resdLots_ = lots_;
        execLots_ = 0_lts;
        execCost_ = 0_cst;
        modified_ = now;
    }
    void revise(Lots lots, Time now) noexcept
    {
        assert(lots > 0_lts);
        assert(lots >= execLots_);
        assert(lots >= minLots_);
        assert(lots <= lots_);
        const auto delta = lots_ - lots;
        assert(delta >= 0_lts);
        state_ = State::Revise;
        lots_ = lots;
        resdLots_ -= delta;
        modified_ = now;
    }
    void cancel(Time now) noexcept
    {
        state_ = State::Cancel;
        // Note that executed lots is not affected.
        resdLots_ = 0_lts;
        modified_ = now;
    }
    void trade(Lots takenLots, Cost takenCost, Lots lastLots, Ticks lastTicks, Time now) noexcept
    {
        state_ = State::Trade;
        resdLots_ -= takenLots;
        execLots_ += takenLots;
        execCost_ += takenCost;
        lastLots_ = lastLots;
        lastTicks_ = lastTicks;
        modified_ = now;
    }
    void trade(Lots lastLots, Ticks lastTicks, Time now) noexcept
    {
        trade(lastLots, swirly::cost(lastLots, lastTicks), lastLots, lastTicks, now);
    }
    boost::intrusive::set_member_hook<> idHook_;
    boost::intrusive::set_member_hook<> refHook_;
    boost::intrusive::list_member_hook<> listHook_;

  private:
    // Internals.
    mutable Level* level_{nullptr};

    State state_;
    const Ticks ticks_;
    /**
     * Must be greater than zero.
     */
    Lots resdLots_;
    /**
     * Must not be greater that lots.
     */
    Lots execLots_;
    Cost execCost_;
    Lots lastLots_;
    Ticks lastTicks_;
    /**
     * Minimum to be filled by this order.
     */
    const Lots minLots_;
    Time modified_;
};

static_assert(sizeof(Order) <= 5 * 64, "no greater than specified cache-lines");

using OrderIdSet = RequestIdSet<Order>;

class SWIRLY_API OrderRefSet {
    struct ValueCompare {
        int compare(const Order& lhs, const Order& rhs) const noexcept
        {
            return lhs.ref().compare(rhs.ref());
        }
        bool operator()(const Order& lhs, const Order& rhs) const noexcept
        {
            return compare(lhs, rhs) < 0;
        }
    };
    struct KeyValueCompare {
        bool operator()(std::string_view lhs, const Order& rhs) const noexcept
        {
            return lhs.compare(rhs.ref()) < 0;
        }
        bool operator()(const Order& lhs, std::string_view rhs) const noexcept
        {
            return lhs.ref().compare(rhs) < 0;
        }
    };
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using CompareOption = boost::intrusive::compare<ValueCompare>;
    using MemberHookOption
        = boost::intrusive::member_hook<Order, decltype(Order::refHook_), &Order::refHook_>;
    using Set
        = boost::intrusive::set<Order, ConstantTimeSizeOption, CompareOption, MemberHookOption>;
    using ValuePtr = boost::intrusive_ptr<Order>;

  public:
    using Iterator = typename Set::iterator;
    using ConstIterator = typename Set::const_iterator;

    OrderRefSet() = default;

    ~OrderRefSet() noexcept;

    // Copy.
    OrderRefSet(const OrderRefSet&) = delete;
    OrderRefSet& operator=(const OrderRefSet&) = delete;

    // Move.
    OrderRefSet(OrderRefSet&&);
    OrderRefSet& operator=(OrderRefSet&&);

    // Begin.
    ConstIterator begin() const noexcept { return set_.begin(); }
    ConstIterator cbegin() const noexcept { return set_.cbegin(); }
    Iterator begin() noexcept { return set_.begin(); }

    // End.
    ConstIterator end() const noexcept { return set_.end(); }
    ConstIterator cend() const noexcept { return set_.cend(); }
    Iterator end() noexcept { return set_.end(); }

    // Find.
    ConstIterator find(std::string_view ref) const noexcept
    {
        return set_.find(ref, KeyValueCompare());
    }
    Iterator find(std::string_view ref) noexcept { return set_.find(ref, KeyValueCompare()); }
    std::pair<ConstIterator, bool> findHint(std::string_view ref) const noexcept
    {
        const auto comp = KeyValueCompare();
        auto it = set_.lower_bound(ref, comp);
        return std::make_pair(it, it != set_.end() && !comp(ref, *it));
    }
    std::pair<Iterator, bool> findHint(std::string_view ref) noexcept
    {
        const auto comp = KeyValueCompare();
        auto it = set_.lower_bound(ref, comp);
        return std::make_pair(it, it != set_.end() && !comp(ref, *it));
    }
    Iterator insert(const ValuePtr& value) noexcept;

    Iterator insertHint(ConstIterator hint, const ValuePtr& value) noexcept;

    Iterator insertOrReplace(const ValuePtr& value) noexcept;

    template <typename... ArgsT>
    Iterator emplace(ArgsT&&... args)
    {
        return insert(makeIntrusive<Order>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplaceHint(ConstIterator hint, ArgsT&&... args)
    {
        return insertHint(hint, makeIntrusive<Order>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplaceOrReplace(ArgsT&&... args)
    {
        return insertOrReplace(makeIntrusive<Order>(std::forward<ArgsT>(args)...));
    }
    ValuePtr remove(const Order& ref) noexcept
    {
        ValuePtr value;
        set_.erase_and_dispose(ref, [&value](Order* ptr) { value = ValuePtr{ptr, false}; });
        return value;
    }

  private:
    Set set_;
};

class SWIRLY_API OrderList {
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using MemberHookOption
        = boost::intrusive::member_hook<Order, decltype(Order::listHook_), &Order::listHook_>;
    using List = boost::intrusive::list<Order, ConstantTimeSizeOption, MemberHookOption>;
    using ValuePtr = boost::intrusive_ptr<Order>;

  public:
    using Iterator = typename List::iterator;
    using ConstIterator = typename List::const_iterator;

    OrderList() = default;

    ~OrderList() noexcept;

    // Copy.
    OrderList(const OrderList&) = delete;
    OrderList& operator=(const OrderList&) = delete;

    // Move.
    OrderList(OrderList&&);
    OrderList& operator=(OrderList&&);

    static ConstIterator toIterator(const Order& order) noexcept
    {
        return List::s_iterator_to(order);
    }

    // Begin.
    ConstIterator begin() const noexcept { return list_.begin(); }
    ConstIterator cbegin() const noexcept { return list_.cbegin(); }
    Iterator begin() noexcept { return list_.begin(); }

    // End.
    ConstIterator end() const noexcept { return list_.end(); }
    ConstIterator cend() const noexcept { return list_.cend(); }
    Iterator end() noexcept { return list_.end(); }
    Iterator insertBack(const OrderPtr& value) noexcept;

    Iterator insertBefore(const OrderPtr& value, const Order& next) noexcept;

    ValuePtr remove(const Order& ref) noexcept;

  private:
    List list_;
};

inline std::ostream& operator<<(std::ostream& os, const Order& order)
{
    order.toJson(os);
    return os;
}

} // namespace swirly

#endif // SWIRLY_FIN_ORDER_HPP

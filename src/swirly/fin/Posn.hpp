/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
#ifndef SWIRLY_FIN_POSN_HPP
#define SWIRLY_FIN_POSN_HPP

#include <swirly/fin/Conv.hpp>
#include <swirly/fin/Types.hpp>

#include <swirly/util/BasicTypes.hpp>
#include <swirly/util/Date.hpp>
#include <swirly/util/Symbol.hpp>

#include <swirly/sys/RefCount.hpp>

#include <boost/intrusive/set.hpp>

namespace swirly {

class SWIRLY_API Posn : public RefCount<Posn, ThreadUnsafePolicy> {
  public:
    Posn(Symbol accnt, Id64 marketId, Symbol instr, JDay settlDay, Lots buyLots, Cost buyCost,
         Lots sellLots, Cost sellCost) noexcept
      : accnt_{accnt}
      , marketId_{marketId}
      , instr_{instr}
      , settlDay_{settlDay}
      , buyLots_{buyLots}
      , buyCost_{buyCost}
      , sellLots_{sellLots}
      , sellCost_{sellCost}
    {
    }
    Posn(Symbol accnt, Id64 marketId, Symbol instr, JDay settlDay) noexcept
      : Posn{accnt, marketId, instr, settlDay, 0_lts, 0_cst, 0_lts, 0_cst}
    {
    }
    ~Posn() noexcept;

    // Copy.
    Posn(const Posn&) = delete;
    Posn& operator=(const Posn&) = delete;

    // Move.
    Posn(Posn&&);
    Posn& operator=(Posn&&) = delete;

    template <typename... ArgsT>
    static PosnPtr make(ArgsT&&... args)
    {
        return makeIntrusive<Posn>(std::forward<ArgsT>(args)...);
    }

    void toJson(std::ostream& os) const;

    auto accnt() const noexcept { return accnt_; }
    auto marketId() const noexcept { return marketId_; }
    auto instr() const noexcept { return instr_; }
    auto settlDay() const noexcept { return settlDay_; }
    auto buyLots() const noexcept { return buyLots_; }
    auto buyCost() const noexcept { return buyCost_; }
    auto sellLots() const noexcept { return sellLots_; }
    auto sellCost() const noexcept { return sellCost_; }

    void add(const Posn& rhs) noexcept
    {
        addBuy(rhs.buyLots_, rhs.buyCost_);
        addSell(rhs.sellLots_, rhs.sellCost_);
    }
    void addBuy(Lots lots, Cost cost) noexcept
    {
        buyLots_ += lots;
        buyCost_ += cost;
    }
    void addSell(Lots lots, Cost cost) noexcept
    {
        sellLots_ += lots;
        sellCost_ += cost;
    }
    void addTrade(Side side, Lots lots, Cost cost) noexcept
    {
        if (side == Side::Buy) {
            addBuy(lots, cost);
        } else {
            assert(side == Side::Sell);
            addSell(lots, cost);
        }
    }
    void addTrade(Side side, Lots lots, Ticks ticks) noexcept
    {
        addTrade(side, lots, swirly::cost(lots, ticks));
    }
    void setBuyLots(Lots buyLots) noexcept { buyLots_ = buyLots; }
    void setBuyCost(Cost buyCost) noexcept { buyCost_ = buyCost; }
    void setSellLots(Lots sellLots) noexcept { sellLots_ = sellLots; }
    void setSellCost(Cost sellCost) noexcept { sellCost_ = sellCost; }

    boost::intrusive::set_member_hook<> idHook_;

  private:
    const Symbol accnt_;
    const Id64 marketId_;
    const Symbol instr_;
    JDay settlDay_;
    Lots buyLots_;
    Cost buyCost_;
    Lots sellLots_;
    Cost sellCost_;
};

inline std::ostream& operator<<(std::ostream& os, const Posn& posn)
{
    posn.toJson(os);
    return os;
}

class SWIRLY_API PosnSet {
    using Key = std::tuple<Symbol, Id64>;
    struct ValueCompare {
        int compare(const Posn& lhs, const Posn& rhs) const noexcept
        {
            int result{lhs.accnt().compare(rhs.accnt())};
            if (result == 0) {
                result = swirly::compare(lhs.marketId(), rhs.marketId());
            }
            return result;
        }
        bool operator()(const Posn& lhs, const Posn& rhs) const noexcept
        {
            return compare(lhs, rhs) < 0;
        }
    };
    struct KeyValueCompare {
        bool operator()(const Key& lhs, const Posn& rhs) const noexcept
        {
            int result{std::get<0>(lhs).compare(rhs.accnt())};
            if (result == 0) {
                result = swirly::compare(std::get<1>(lhs), rhs.marketId());
            }
            return result < 0;
        }
        bool operator()(const Posn& lhs, const Key& rhs) const noexcept
        {
            int result{lhs.accnt().compare(std::get<0>(rhs))};
            if (result == 0) {
                result = swirly::compare(lhs.marketId(), std::get<1>(rhs));
            }
            return result < 0;
        }
    };
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using CompareOption = boost::intrusive::compare<ValueCompare>;
    using MemberHookOption
        = boost::intrusive::member_hook<Posn, decltype(Posn::idHook_), &Posn::idHook_>;
    using Set
        = boost::intrusive::set<Posn, ConstantTimeSizeOption, CompareOption, MemberHookOption>;
    using ValuePtr = boost::intrusive_ptr<Posn>;

  public:
    using Iterator = typename Set::iterator;
    using ConstIterator = typename Set::const_iterator;

    PosnSet() = default;

    ~PosnSet() noexcept;

    // Copy.
    PosnSet(const PosnSet&) = delete;
    PosnSet& operator=(const PosnSet&) = delete;

    // Move.
    PosnSet(PosnSet&&);
    PosnSet& operator=(PosnSet&&);

    // Begin.
    ConstIterator begin() const noexcept { return set_.begin(); }
    ConstIterator cbegin() const noexcept { return set_.cbegin(); }
    Iterator begin() noexcept { return set_.begin(); }

    // End.
    ConstIterator end() const noexcept { return set_.end(); }
    ConstIterator cend() const noexcept { return set_.cend(); }
    Iterator end() noexcept { return set_.end(); }

    // Find.
    ConstIterator find(Symbol accnt, Id64 marketId) const noexcept
    {
        return set_.find(std::make_tuple(accnt, marketId), KeyValueCompare());
    }
    Iterator find(Symbol accnt, Id64 marketId) noexcept
    {
        return set_.find(std::make_tuple(accnt, marketId), KeyValueCompare());
    }
    std::pair<ConstIterator, bool> findHint(Symbol accnt, Id64 marketId) const noexcept
    {
        const auto key = std::make_tuple(accnt, marketId);
        const auto comp = KeyValueCompare();
        auto it = set_.lower_bound(key, comp);
        return std::make_pair(it, it != set_.end() && !comp(key, *it));
    }
    std::pair<Iterator, bool> findHint(Symbol accnt, Id64 marketId) noexcept
    {
        const auto key = std::make_tuple(accnt, marketId);
        const auto comp = KeyValueCompare();
        auto it = set_.lower_bound(key, comp);
        return std::make_pair(it, it != set_.end() && !comp(key, *it));
    }
    Iterator insert(const ValuePtr& value) noexcept;

    Iterator insertHint(ConstIterator hint, const ValuePtr& value) noexcept;

    Iterator insertOrReplace(const ValuePtr& value) noexcept;

    template <typename... ArgsT>
    Iterator emplace(ArgsT&&... args)
    {
        return insert(makeIntrusive<Posn>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplaceHint(ConstIterator hint, ArgsT&&... args)
    {
        return insertHint(hint, makeIntrusive<Posn>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplaceOrReplace(ArgsT&&... args)
    {
        return insertOrReplace(makeIntrusive<Posn>(std::forward<ArgsT>(args)...));
    }
    ValuePtr remove(Iterator it) noexcept
    {
        ValuePtr ptr{&*it, false};
        set_.erase(it);
        return ptr;
    }

  private:
    Set set_;
};

} // namespace swirly

#endif // SWIRLY_FIN_POSN_HPP

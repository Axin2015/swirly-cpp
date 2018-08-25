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
#ifndef SWIRLY_FIN_POSN_HPP
#define SWIRLY_FIN_POSN_HPP

#include <swirly/fin/Conv.hpp>
#include <swirly/fin/Types.hpp>

#include <swirly/util/Date.hpp>
#include <swirly/util/RefCount.hpp>
#include <swirly/util/Symbol.hpp>

#include <boost/intrusive/set.hpp>

namespace swirly {
inline namespace fin {

class SWIRLY_API Posn : public RefCount<Posn, ThreadUnsafePolicy> {
  public:
    Posn(Symbol accnt, Id64 market_id, Symbol instr, JDay settl_day, Lots buy_lots, Cost buy_cost,
         Lots sell_lots, Cost sell_cost) noexcept
    : accnt_{accnt}
    , market_id_{market_id}
    , instr_{instr}
    , settl_day_{settl_day}
    , buy_lots_{buy_lots}
    , buy_cost_{buy_cost}
    , sell_lots_{sell_lots}
    , sell_cost_{sell_cost}
    , net_cost_{buy_cost - sell_cost}
    {
    }
    Posn(Symbol accnt, Id64 market_id, Symbol instr, JDay settl_day) noexcept
    : Posn{accnt, market_id, instr, settl_day, 0_lts, 0_cst, 0_lts, 0_cst}
    {
    }
    ~Posn();

    // Copy.
    Posn(const Posn&) = delete;
    Posn& operator=(const Posn&) = delete;

    // Move.
    Posn(Posn&&);
    Posn& operator=(Posn&&) = delete;

    template <typename... ArgsT>
    static PosnPtr make(ArgsT&&... args)
    {
        return make_intrusive<Posn>(std::forward<ArgsT>(args)...);
    }

    void to_dsv(std::ostream& os, char delim = ',') const;
    void to_json(std::ostream& os) const;

    auto accnt() const noexcept { return accnt_; }
    auto market_id() const noexcept { return market_id_; }
    auto instr() const noexcept { return instr_; }
    auto settl_day() const noexcept { return settl_day_; }
    auto buy_lots() const noexcept { return buy_lots_; }
    auto buy_cost() const noexcept { return buy_cost_; }
    auto sell_lots() const noexcept { return sell_lots_; }
    auto sell_cost() const noexcept { return sell_cost_; }
    auto net_lots() const noexcept { return buy_lots_ - sell_lots_; }
    auto net_cost() const noexcept { return net_cost_; }

    void add_buy(Lots lots, Cost cost) noexcept
    {
        buy_lots_ += lots;
        buy_cost_ += cost;
        net_cost_ = buy_cost_ - sell_cost_;
    }
    void add_sell(Lots lots, Cost cost) noexcept
    {
        sell_lots_ += lots;
        sell_cost_ += cost;
        net_cost_ = buy_cost_ - sell_cost_;
    }
    void add_buy(Lots lots, Ticks ticks) noexcept;
    void add_sell(Lots lots, Ticks ticks) noexcept;
    void add_posn(const Posn& rhs) noexcept
    {
        add_buy(rhs.buy_lots_, rhs.buy_cost_);
        add_sell(rhs.sell_lots_, rhs.sell_cost_);
    }
    void add_trade(Side side, Lots lots, Ticks ticks) noexcept
    {
        if (side == Side::Buy) {
            add_buy(lots, ticks);
        } else {
            assert(side == Side::Sell);
            add_sell(lots, ticks);
        }
    }

    boost::intrusive::set_member_hook<> id_hook;

  private:
    const Symbol accnt_;
    const Id64 market_id_;
    const Symbol instr_;
    JDay settl_day_;
    Lots buy_lots_;
    Cost buy_cost_;
    Lots sell_lots_;
    Cost sell_cost_;
    Cost net_cost_;
};

inline std::ostream& operator<<(std::ostream& os, const Posn& posn)
{
    posn.to_json(os);
    return os;
}

class SWIRLY_API PosnSet {
    using Key = std::tuple<Symbol, Id64>;
    struct ValueCompare {
        int compare(const Posn& lhs, const Posn& rhs) const noexcept
        {
            int result{lhs.accnt().compare(rhs.accnt())};
            if (result == 0) {
                result = swirly::compare(lhs.market_id(), rhs.market_id());
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
                result = swirly::compare(std::get<1>(lhs), rhs.market_id());
            }
            return result < 0;
        }
        bool operator()(const Posn& lhs, const Key& rhs) const noexcept
        {
            int result{lhs.accnt().compare(std::get<0>(rhs))};
            if (result == 0) {
                result = swirly::compare(lhs.market_id(), std::get<1>(rhs));
            }
            return result < 0;
        }
    };
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using CompareOption = boost::intrusive::compare<ValueCompare>;
    using MemberHookOption
        = boost::intrusive::member_hook<Posn, decltype(Posn::id_hook), &Posn::id_hook>;
    using Set
        = boost::intrusive::set<Posn, ConstantTimeSizeOption, CompareOption, MemberHookOption>;
    using ValuePtr = boost::intrusive_ptr<Posn>;

  public:
    using Iterator = typename Set::iterator;
    using ConstIterator = typename Set::const_iterator;

    PosnSet() = default;

    ~PosnSet();

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
    ConstIterator find(Symbol accnt, Id64 market_id) const noexcept
    {
        return set_.find(std::make_tuple(accnt, market_id), KeyValueCompare());
    }
    Iterator find(Symbol accnt, Id64 market_id) noexcept
    {
        return set_.find(std::make_tuple(accnt, market_id), KeyValueCompare());
    }
    std::pair<ConstIterator, bool> find_hint(Symbol accnt, Id64 market_id) const noexcept
    {
        const auto key = std::make_tuple(accnt, market_id);
        const auto comp = KeyValueCompare();
        auto it = set_.lower_bound(key, comp);
        return std::make_pair(it, it != set_.end() && !comp(key, *it));
    }
    std::pair<Iterator, bool> find_hint(Symbol accnt, Id64 market_id) noexcept
    {
        const auto key = std::make_tuple(accnt, market_id);
        const auto comp = KeyValueCompare();
        auto it = set_.lower_bound(key, comp);
        return std::make_pair(it, it != set_.end() && !comp(key, *it));
    }
    Iterator insert(const ValuePtr& value) noexcept;

    Iterator insert_hint(ConstIterator hint, const ValuePtr& value) noexcept;

    Iterator insert_or_replace(const ValuePtr& value) noexcept;

    template <typename... ArgsT>
    Iterator emplace(ArgsT&&... args)
    {
        return insert(make_intrusive<Posn>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplace_hint(ConstIterator hint, ArgsT&&... args)
    {
        return insert_hint(hint, make_intrusive<Posn>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplace_or_replace(ArgsT&&... args)
    {
        return insert_or_replace(make_intrusive<Posn>(std::forward<ArgsT>(args)...));
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

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_POSN_HPP

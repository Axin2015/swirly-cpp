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
#ifndef SWIRLY_ELM_POSN_HPP
#define SWIRLY_ELM_POSN_HPP

#include <swirly/elm/Conv.hpp>
#include <swirly/elm/Exec.hpp>
#include <swirly/elm/Types.hpp>

#include <swirly/ash/RefCounted.hpp>
#include <swirly/ash/Types.hpp>

#include <boost/intrusive/set.hpp>

/**
 * @addtogroup Entity
 * @{
 */

namespace swirly {

class SWIRLY_API Posn : public RefCounted {
 public:
  Posn(std::string_view trader, std::string_view contr, Jday settlDay, Lots buyLots, Cost buyCost,
       Lots sellLots, Cost sellCost) noexcept : trader_{trader},
                                                contr_{contr},
                                                settlDay_{settlDay},
                                                buyLots_{buyLots},
                                                buyCost_{buyCost},
                                                sellLots_{sellLots},
                                                sellCost_{sellCost}
  {
  }
  ~Posn() noexcept override;

  // Copy.
  Posn(const Posn&) = delete;
  Posn& operator=(const Posn&) = delete;

  // Move.
  Posn(Posn&&);
  Posn& operator=(Posn&&) = delete;

  void toJson(std::ostream& os) const;

  auto trader() const noexcept { return +trader_; }
  auto contr() const noexcept { return +contr_; }
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
    using namespace enumops;
    buyLots_ += lots;
    buyCost_ += cost;
  }
  void addSell(Lots lots, Cost cost) noexcept
  {
    using namespace enumops;
    sellLots_ += lots;
    sellCost_ += cost;
  }
  void addTrade(Side side, Lots lastLots, Ticks lastTicks) noexcept
  {
    const auto cost = swirly::cost(lastLots, lastTicks);
    if (side == Side::Buy) {
      addBuy(lastLots, cost);
    } else {
      assert(side == Side::Sell);
      addSell(lastLots, cost);
    }
  }
  void addTrade(const Exec& trade) noexcept
  {
    addTrade(trade.side(), trade.lastLots(), trade.lastTicks());
  }
  /**
   * This function is typically used to change the settlement-day to zero during settlement.
   *
   * @param settlDay
   *        The new settlement-day.
   */
  void setSettlDay(Jday settlDay) noexcept { settlDay_ = settlDay; }
  void setBuyLots(Lots buyLots) noexcept { buyLots_ = buyLots; }
  void setBuyCost(Cost buyCost) noexcept { buyCost_ = buyCost; }
  void setSellLots(Lots sellLots) noexcept { sellLots_ = sellLots; }
  void setSellCost(Cost sellCost) noexcept { sellCost_ = sellCost; }

  boost::intrusive::set_member_hook<> traderHook_;

 private:
  const Mnem trader_;
  const Mnem contr_;
  Jday settlDay_;
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

using PosnPtr = boost::intrusive_ptr<Posn>;

class SWIRLY_API TraderPosnSet {
  using Key = std::tuple<std::string_view, Jday>;
  struct ValueCompare {
    int compare(const Posn& lhs, const Posn& rhs) const noexcept
    {
      int result{lhs.contr().compare(rhs.contr())};
      if (result == 0) {
        result = swirly::compare(lhs.settlDay(), rhs.settlDay());
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
      int result{std::get<0>(lhs).compare(rhs.contr())};
      if (result == 0) {
        result = swirly::compare(std::get<1>(lhs), rhs.settlDay());
      }
      return result < 0;
    }
    bool operator()(const Posn& lhs, const Key& rhs) const noexcept
    {
      int result{lhs.contr().compare(std::get<0>(rhs))};
      if (result == 0) {
        result = swirly::compare(lhs.settlDay(), std::get<1>(rhs));
      }
      return result < 0;
    }
  };
  using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
  using CompareOption = boost::intrusive::compare<ValueCompare>;
  using MemberHookOption
    = boost::intrusive::member_hook<Posn, decltype(Posn::traderHook_), &Posn::traderHook_>;
  using Set = boost::intrusive::set<Posn, ConstantTimeSizeOption, CompareOption, MemberHookOption>;
  using ValuePtr = boost::intrusive_ptr<Posn>;

 public:
  using Iterator = typename Set::iterator;
  using ConstIterator = typename Set::const_iterator;

  TraderPosnSet() = default;

  ~TraderPosnSet() noexcept;

  // Copy.
  TraderPosnSet(const TraderPosnSet&) = delete;
  TraderPosnSet& operator=(const TraderPosnSet&) = delete;

  // Move.
  TraderPosnSet(TraderPosnSet&&);
  TraderPosnSet& operator=(TraderPosnSet&&);

  // Begin.
  ConstIterator begin() const noexcept { return set_.begin(); }
  ConstIterator cbegin() const noexcept { return set_.cbegin(); }
  Iterator begin() noexcept { return set_.begin(); }

  // End.
  ConstIterator end() const noexcept { return set_.end(); }
  ConstIterator cend() const noexcept { return set_.cend(); }
  Iterator end() noexcept { return set_.end(); }

  // Find.
  ConstIterator find(std::string_view contr, Jday settlDay) const noexcept
  {
    return set_.find(std::make_tuple(contr, settlDay), KeyValueCompare());
  }
  Iterator find(std::string_view contr, Jday settlDay) noexcept
  {
    return set_.find(std::make_tuple(contr, settlDay), KeyValueCompare());
  }
  std::pair<ConstIterator, bool> findHint(std::string_view contr, Jday settlDay) const noexcept
  {
    const auto key = std::make_tuple(contr, settlDay);
    const auto comp = KeyValueCompare();
    auto it = set_.lower_bound(key, comp);
    return std::make_pair(it, it != set_.end() && !comp(key, *it));
  }
  std::pair<Iterator, bool> findHint(std::string_view contr, Jday settlDay) noexcept
  {
    const auto key = std::make_tuple(contr, settlDay);
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
    return insert(makeRefCounted<Posn>(std::forward<ArgsT>(args)...));
  }
  template <typename... ArgsT>
  Iterator emplaceHint(ConstIterator hint, ArgsT&&... args)
  {
    return insertHint(hint, makeRefCounted<Posn>(std::forward<ArgsT>(args)...));
  }
  template <typename... ArgsT>
  Iterator emplaceOrReplace(ArgsT&&... args)
  {
    return insertOrReplace(makeRefCounted<Posn>(std::forward<ArgsT>(args)...));
  }

 private:
  Set set_;
};

} // swirly

/** @} */

#endif // SWIRLY_ELM_POSN_HPP

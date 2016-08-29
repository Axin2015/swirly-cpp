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
#ifndef SWIRLY_FIG_ACCNT_HPP
#define SWIRLY_FIG_ACCNT_HPP

#include <swirly/elm/Exception.hpp>
#include <swirly/elm/Exec.hpp>
#include <swirly/elm/Order.hpp>
#include <swirly/elm/Posn.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <boost/circular_buffer.hpp>
#pragma GCC diagnostic pop

namespace swirly {

class Accnt;

using AccntPtr = std::unique_ptr<Accnt>;
using ConstAccntPtr = std::unique_ptr<const Accnt>;

class SWIRLY_API Accnt : public Comparable<Accnt> {
 public:
  Accnt(Mnem mnem, std::size_t maxExecs) noexcept : mnem_{mnem}, execs_{maxExecs} {}
  ~Accnt() noexcept;

  // Copy.
  Accnt(const Accnt&) = delete;
  Accnt& operator=(const Accnt&) = delete;

  // Move.
  Accnt(Accnt&&);
  Accnt& operator=(Accnt&&) = delete;

  template <typename... ArgsT>
  static AccntPtr make(ArgsT&&... args)
  {
    return std::make_unique<Accnt>(std::forward<ArgsT>(args)...);
  }

  int compare(const Accnt& rhs) const noexcept { return mnem_.compare(rhs.mnem_); }
  bool exists(std::string_view ref) const noexcept { return refIdx_.find(ref) != refIdx_.end(); }
  auto mnem() const noexcept { return mnem_; }
  const auto& orders() const noexcept { return orders_; }
  const auto& execs() const noexcept { return execs_; }
  const auto& trades() const noexcept { return trades_; }
  const Exec& trade(Mnem market, Id64 id) const
  {
    auto it = trades_.find(market, id);
    if (it == trades_.end()) {
      throw NotFoundException{errMsg() << "trade '" << id << "' does not exist"};
    }
    return *it;
  }
  const auto& posns() const noexcept { return posns_; }

  auto& orders() noexcept { return orders_; }
  Order& order(Mnem market, Id64 id)
  {
    auto it = orders_.find(market, id);
    if (it == orders_.end()) {
      throw OrderNotFoundException{errMsg() << "order '" << id << "' does not exist"};
    }
    return *it;
  }
  Order& order(std::string_view ref)
  {
    auto it = refIdx_.find(ref);
    if (it == refIdx_.end()) {
      throw OrderNotFoundException{errMsg() << "order '" << ref << "' does not exist"};
    }
    return *it;
  }
  void insertOrder(const OrderPtr& order) noexcept
  {
    assert(order->accnt() == mnem_);
    orders_.insert(order);
    if (!order->ref().empty()) {
      refIdx_.insert(order);
    }
  }
  OrderPtr removeOrder(const Order& order) noexcept
  {
    assert(order.accnt() == mnem_);
    if (!order.ref().empty()) {
      refIdx_.remove(order);
    }
    return orders_.remove(order);
  }
  void pushExecBack(const ConstExecPtr& exec) noexcept
  {
    assert(exec->accnt() == mnem_);
    execs_.push_back(exec);
  }
  void pushExecFront(const ConstExecPtr& exec) noexcept
  {
    assert(exec->accnt() == mnem_);
    execs_.push_front(exec);
  }
  void insertTrade(const ExecPtr& trade) noexcept
  {
    assert(trade->accnt() == mnem_);
    assert(trade->state() == State::Trade);
    trades_.insert(trade);
  }
  ConstExecPtr removeTrade(const Exec& trade) noexcept
  {
    assert(trade.accnt() == mnem_);
    return trades_.remove(trade);
  }
  PosnPtr posn(Mnem contr, Jday settlDay) throw(std::bad_alloc);

  void insertPosn(const PosnPtr& posn) noexcept
  {
    assert(posn->accnt() == mnem_);
    posns_.insert(posn);
  }
  boost::intrusive::set_member_hook<> mnemHook_;

 private:
  const Mnem mnem_;
  OrderIdSet orders_;
  boost::circular_buffer<ConstExecPtr> execs_;
  ExecIdSet trades_;
  AccntPosnSet posns_;
  OrderRefSet refIdx_;
};

using AccntSet = MnemSet<Accnt>;

} // swirly

#endif // SWIRLY_FIG_ACCNT_HPP

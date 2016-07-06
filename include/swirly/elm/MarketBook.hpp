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
#ifndef SWIRLY_ELM_MARKETBOOK_HPP
#define SWIRLY_ELM_MARKETBOOK_HPP

#include <swirly/elm/BookSide.hpp>
#include <swirly/elm/Market.hpp>

namespace swirly {

class MarketBook;

/**
 * @addtogroup Book
 * @{
 */

/**
 * Adapter that provides stream insertion operator for MarketBook::toJsonView().
 */
class SWIRLY_API MarketView {
 public:
  MarketView(const MarketBook& book) noexcept : book_{&book} {}
  ~MarketView() noexcept = default;

  // Copy.
  MarketView(const MarketView&) noexcept = default;
  MarketView& operator=(const MarketView&) noexcept = default;

  // Move.
  MarketView(MarketView&&) noexcept = default;
  MarketView& operator=(MarketView&&) noexcept = default;

  void toJson(std::ostream& os) const;

 private:
  const MarketBook* book_;
};

inline std::ostream& operator<<(std::ostream& os, const MarketView& view)
{
  view.toJson(os);
  return os;
}

using MarketBookPtr = std::unique_ptr<MarketBook>;
using ConstMarketBookPtr = std::unique_ptr<const MarketBook>;

class SWIRLY_API MarketBook : public Market {
 public:
  MarketBook(Mnem mnem, std::string_view display, Mnem contr, Jday settlDay, Jday expiryDay,
             MarketState state, Lots lastLots = 0_lts, Ticks lastTicks = 0_tks,
             Millis lastTime = 0_ms, Iden maxOrderId = 0_id, Iden maxExecId = 0_id) noexcept
    : Market{mnem, display, contr, settlDay, expiryDay, state},
      lastLots_{lastLots},
      lastTicks_{lastTicks},
      lastTime_{lastTime},
      maxOrderId_{maxOrderId},
      maxExecId_{maxExecId}
  {
  }
  ~MarketBook() noexcept override;

  // Copy.
  MarketBook(const MarketBook&) = default;
  MarketBook& operator=(const MarketBook&) = default;

  // Move.
  MarketBook(MarketBook&&) = default;
  MarketBook& operator=(MarketBook&&) = default;

  template <typename... ArgsT>
  static MarketBookPtr make(ArgsT&&... args)
  {
    return std::make_unique<MarketBook>(std::forward<ArgsT>(args)...);
  }

  void toJsonView(std::ostream& os) const;

  Lots lastLots() const noexcept { return lastLots_; }
  Ticks lastTicks() const noexcept { return lastTicks_; }
  Millis lastTime() const noexcept { return lastTime_; }
  const BookSide& bidSide() const noexcept { return bidSide_; }
  const BookSide& offerSide() const noexcept { return offerSide_; }
  Iden maxOrderId() const noexcept { return maxOrderId_; }
  Iden maxExecId() const noexcept { return maxExecId_; }
  MarketView view() const noexcept { return *this; }
  BookSide& bidSide() noexcept { return bidSide_; }
  BookSide& offerSide() noexcept { return offerSide_; }
  void insertOrder(const OrderPtr& order) throw(std::bad_alloc)
  {
    side(order->side()).insertOrder(order);
  }
  void removeOrder(const Order& order) noexcept { side(order.side()).removeOrder(order); }
  void createOrder(const OrderPtr& order, Millis now) throw(std::bad_alloc)
  {
    side(order->side()).createOrder(order, now);
  }
  void reviseOrder(Order& order, Lots lots, Millis now) noexcept
  {
    side(order.side()).reviseOrder(order, lots, now);
  }
  void cancelOrder(Order& order, Millis now) noexcept
  {
    side(order.side()).cancelOrder(order, now);
  }
  void takeOrder(Order& order, Lots lots, Millis now) noexcept
  {
    side(order.side()).takeOrder(order, lots, now);
    lastLots_ = lots;
    lastTicks_ = order.ticks();
    lastTime_ = now;
  }
  Iden allocOrderId() noexcept
  {
    using namespace enumops;
    return ++maxOrderId_;
  }
  Iden allocExecId() noexcept
  {
    using namespace enumops;
    return ++maxExecId_;
  }
  boost::intrusive::set_member_hook<> mnemHook_;

 private:
  BookSide& side(Side side) noexcept { return side == Side::Buy ? bidSide_ : offerSide_; }
  const BookSide& side(Side side) const noexcept
  {
    return side == Side::Buy ? bidSide_ : offerSide_;
  }

  Lots lastLots_;
  Ticks lastTicks_;
  Millis lastTime_;
  // Two sides constitute the book.
  BookSide bidSide_;
  BookSide offerSide_;
  Iden maxOrderId_;
  Iden maxExecId_;
};

inline void MarketView::toJson(std::ostream& os) const
{
  book_->toJsonView(os);
}

using MarketSet = MnemSet<MarketBook>;

/** @} */

} // swirly

#endif // SWIRLY_ELM_MARKETBOOK_HPP

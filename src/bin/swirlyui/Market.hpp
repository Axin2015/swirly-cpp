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
#ifndef SWIRLYUI_MARKET_HPP
#define SWIRLYUI_MARKET_HPP

#include "Level.hpp"

#include <QDate>
#include <QMetaType>
#include <QString>

#ifndef SWIRLY_DEPTH
#define SWIRLY_DEPTH 3
#endif // SWIRLY_DEPTH

class QJsonObject;

namespace swirly {
namespace ui {

class Market {
 public:
  static constexpr std::size_t Depth{SWIRLY_DEPTH};
  using Levels = std::array<Level, Depth>;

  Market(const QString& contr, QDate settlDate, MarketState state, Lots lastLots, Ticks lastTicks,
         const QDateTime& lastTime)
    : contr_{contr},
      settlDate_{settlDate},
      state_{state},
      lastLots_{lastLots},
      lastTicks_{lastTicks},
      lastTime_{lastTime}
  {
  }
  Market() = default;
  ~Market() noexcept = default;

  static Market fromJson(const QJsonObject& obj);

  const QString& contr() const noexcept { return contr_; }
  QDate settlDate() const noexcept { return settlDate_; }
  MarketState state() const noexcept { return state_; }
  Lots lastLots() const noexcept { return lastLots_; }
  Ticks lastTicks() const noexcept { return lastTicks_; }
  const QDateTime& lastTime() const noexcept { return lastTime_; }
  const Levels& bids() const noexcept { return bids_; }
  const Levels& offers() const noexcept { return offers_; }
  const Level& bestBid() const noexcept { return bids_.front(); }
  const Level& bestOffer() const noexcept { return offers_.front(); }

 private:
  QString contr_{};
  QDate settlDate_{};
  MarketState state_{};
  Lots lastLots_{};
  Ticks lastTicks_{};
  QDateTime lastTime_{};
  Levels bids_;
  Levels offers_;
};

QDebug operator<<(QDebug debug, const Market& market);

} // ui
} // swirly

Q_DECLARE_METATYPE(swirly::ui::Market)

#endif // SWIRLYUI_MARKET_HPP

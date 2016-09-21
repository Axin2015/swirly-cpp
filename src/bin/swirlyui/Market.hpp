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

#include <QDate>
#include <QMetaType>
#include <QString>

#include "Types.hpp"

class QJsonObject;

namespace swirly {
namespace ui {

class Market {
 public:
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

  static Market parse(const QJsonObject& obj);

  const QString& contr() const noexcept { return contr_; }
  QDate settlDate() const noexcept { return settlDate_; }
  MarketState state() const noexcept { return state_; }
  Lots lastLots() const noexcept { return lastLots_; }
  Ticks lastTicks() const noexcept { return lastTicks_; }
  const QDateTime& lastTime() const noexcept { return lastTime_; }

 private:
  QString contr_{};
  QDate settlDate_{};
  MarketState state_{};
  Lots lastLots_{};
  Ticks lastTicks_{};
  QDateTime lastTime_{};
};

QDebug operator<<(QDebug debug, const Market& market);

} // ui
} // swirly

Q_DECLARE_METATYPE(swirly::ui::Market)

#endif // SWIRLYUI_MARKET_HPP

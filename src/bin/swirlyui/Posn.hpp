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
#ifndef SWIRLYUI_POSN_HPP
#define SWIRLYUI_POSN_HPP

#include "Types.hpp"

#include <QDate>
#include <QMetaType>
#include <QString>

class QJsonObject;

namespace swirly {
namespace ui {

class Posn {
 public:
  Posn(Id64 marketId, const QString& contr, QDate settlDate, const QString& accnt, Lots buyLots,
       Cost buyCost, Lots sellLots, Cost sellCost)
    : marketId_{marketId},
      contr_{contr},
      settlDate_{settlDate},
      accnt_{accnt},
      buyLots_{buyLots},
      buyCost_{buyCost},
      sellLots_{sellLots},
      sellCost_{sellCost}
  {
  }
  Posn() = default;
  ~Posn() noexcept = default;

  static Posn fromJson(const QJsonObject& obj);

  Id64 marketId() const noexcept { return marketId_; }
  const QString& contr() const noexcept { return contr_; }
  QDate settlDate() const noexcept { return settlDate_; }
  const QString& accnt() const noexcept { return accnt_; }
  Lots buyLots() const noexcept { return buyLots_; }
  Cost buyCost() const noexcept { return buyCost_; }
  Lots sellLots() const noexcept { return sellLots_; }
  Cost sellCost() const noexcept { return sellCost_; }

 private:
  Id64 marketId_{};
  QString contr_;
  QDate settlDate_;
  QString accnt_;
  Lots buyLots_;
  Cost buyCost_;
  Lots sellLots_;
  Cost sellCost_;
};

QDebug operator<<(QDebug debug, const Posn& posn);

} // ui
} // swirly

Q_DECLARE_METATYPE(swirly::ui::Posn)

#endif // SWIRLYUI_POSN_HPP

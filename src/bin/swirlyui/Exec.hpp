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
#ifndef SWIRLYUI_EXEC_HPP
#define SWIRLYUI_EXEC_HPP

#include "Types.hpp"

#include <QDate>
#include <QMetaType>
#include <QString>

class QJsonObject;

namespace swirly {
namespace ui {

class Exec {
 public:
  Exec(const QString& accnt, const QString& contr, QDate settlDate, Id64 id, const QString& ref,
       Id64 orderId, State state, Side side, Lots lots, Ticks ticks, Lots resd, Lots exec,
       Cost cost, Lots lastLots, Ticks lastTicks, Lots minLots, Id64 matchId, LiqInd liqInd,
       const QString& cpty, const QDateTime& created)
    : accnt_{accnt},
      contr_{contr},
      settlDate_{settlDate},
      id_{id},
      ref_{ref},
      orderId_{orderId},
      state_{state},
      side_{side},
      lots_{lots},
      ticks_{ticks},
      resd_{resd},
      exec_{exec},
      cost_{cost},
      lastLots_{lastLots},
      lastTicks_{lastTicks},
      minLots_{minLots},
      matchId_{matchId},
      liqInd_{liqInd},
      cpty_{cpty},
      created_{created}
  {
  }
  Exec() = default;
  ~Exec() noexcept = default;

  static Exec fromJson(const QJsonObject& obj);

  const QString& accnt() const noexcept { return accnt_; }
  const QString& contr() const noexcept { return contr_; }
  QDate settlDate() const noexcept { return settlDate_; }
  Id64 id() const noexcept { return id_; }
  const QString& ref() const noexcept { return ref_; }
  Id64 orderId() const noexcept { return orderId_; }
  State state() const noexcept { return state_; }
  Side side() const noexcept { return side_; }
  Lots lots() const noexcept { return lots_; }
  Ticks ticks() const noexcept { return ticks_; }
  Lots resd() const noexcept { return resd_; }
  Lots exec() const noexcept { return exec_; }
  Cost cost() const noexcept { return cost_; }
  Lots lastLots() const noexcept { return lastLots_; }
  Ticks lastTicks() const noexcept { return lastTicks_; }
  Lots minLots() const noexcept { return minLots_; }
  Id64 matchId() const noexcept { return matchId_; }
  LiqInd liqInd() const noexcept { return liqInd_; }
  const QString& cpty() const noexcept { return cpty_; }
  const QDateTime& created() const noexcept { return created_; }

 private:
  QString accnt_;
  QString contr_;
  QDate settlDate_;
  Id64 id_;
  QString ref_;
  Id64 orderId_;
  State state_;
  Side side_;
  Lots lots_;
  Ticks ticks_;
  Lots resd_;
  Lots exec_;
  Cost cost_;
  Lots lastLots_;
  Ticks lastTicks_;
  Lots minLots_;
  Id64 matchId_;
  LiqInd liqInd_;
  QString cpty_;
  QDateTime created_;
};

QDebug operator<<(QDebug debug, const Exec& exec);

} // ui
} // swirly

Q_DECLARE_METATYPE(swirly::ui::Exec)

#endif // SWIRLYUI_EXEC_HPP

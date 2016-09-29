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
#ifndef SWIRLYUI_CONTR_HPP
#define SWIRLYUI_CONTR_HPP

#include "Types.hpp"

#include <QMetaType>
#include <QString>

class QJsonObject;

namespace swirly {
namespace ui {
namespace contr {
namespace column {

enum { //
  Mnem, //
  Display, //
  Asset, //
  Ccy, //
  LotNumer, //
  LotDenom, //
  TickNumer, //
  TickDenom, //
  PipDp, //
  MinLots, //
  MaxLots, //
  Count
};

} // column
} // contr

class Contr {
 public:
  Contr(const QString& mnem, const QString& display, const QString& asset, const QString& ccy,
        int lotNumer, int lotDenom, int tickNumer, int tickDenom, int pipDp, Lots minLots,
        Lots maxLots);
  Contr() = default;
  ~Contr() noexcept = default;

  static Contr fromJson(const QJsonObject& obj);

  const QString& mnem() const noexcept { return mnem_; }
  const QString& display() const noexcept { return display_; }
  const QString& asset() const noexcept { return asset_; }
  const QString& ccy() const noexcept { return ccy_; }
  int lotNumer() const noexcept { return lotNumer_; }
  int lotDenom() const noexcept { return lotDenom_; }
  double qtyInc() const noexcept { return qtyInc_; }
  int tickNumer() const noexcept { return tickNumer_; }
  int tickDenom() const noexcept { return tickDenom_; }
  double priceInc() const noexcept { return priceInc_; }
  int pipDp() const noexcept { return pipDp_; }
  int qtyDp() const noexcept { return qtyDp_; }
  int priceDp() const noexcept { return priceDp_; }
  Lots minLots() const noexcept { return minLots_; }
  Lots maxLots() const noexcept { return maxLots_; }

 private:
  QString mnem_{};
  QString display_{};
  QString asset_{};
  QString ccy_{};
  int lotNumer_{};
  int lotDenom_{};
  // Transient.
  double qtyInc_{};
  int tickNumer_{};
  int tickDenom_{};
  // Transient.
  double priceInc_{};
  int pipDp_{};
  // Transient.
  int qtyDp_{};
  // Transient.
  int priceDp_{};
  Lots minLots_{};
  Lots maxLots_{};
};

QDebug operator<<(QDebug debug, const Contr& contr);

} // ui
} // swirly

Q_DECLARE_METATYPE(swirly::ui::Contr)

#endif // SWIRLYUI_CONTR_HPP

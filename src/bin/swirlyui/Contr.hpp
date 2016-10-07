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

#include <memory>

class QJsonObject;

namespace swirly {
namespace ui {
namespace contr {

enum class Column : int { //
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
  MaxLots
};
constexpr int ColumnCount{unbox(Column::MaxLots) + 1};

} // contr

// Cheap copies.
class Contr {
 public:
  Contr(const QString& mnem, const QString& display, const QString& asset, const QString& ccy,
        int lotNumer, int lotDenom, int tickNumer, int tickDenom, int pipDp, Lots minLots,
        Lots maxLots)
    : impl_{std::make_shared<const Impl>(mnem, display, asset, ccy, lotNumer, lotDenom, tickNumer,
                                         tickDenom, pipDp, minLots, maxLots)}
  {
  }
  Contr() = default;
  ~Contr() noexcept = default;

  static Contr fromJson(const QJsonObject& obj);

  const QString& mnem() const noexcept { return impl_->mnem; }
  const QString& display() const noexcept { return impl_->display; }
  const QString& asset() const noexcept { return impl_->asset; }
  const QString& ccy() const noexcept { return impl_->ccy; }
  int lotNumer() const noexcept { return impl_->lotNumer; }
  int lotDenom() const noexcept { return impl_->lotDenom; }
  double qtyInc() const noexcept { return impl_->qtyInc; }
  int tickNumer() const noexcept { return impl_->tickNumer; }
  int tickDenom() const noexcept { return impl_->tickDenom; }
  double priceInc() const noexcept { return impl_->priceInc; }
  int pipDp() const noexcept { return impl_->pipDp; }
  int qtyDp() const noexcept { return impl_->qtyDp; }
  int priceDp() const noexcept { return impl_->priceDp; }
  Lots minLots() const noexcept { return impl_->minLots; }
  Lots maxLots() const noexcept { return impl_->maxLots; }

 private:
  struct Impl {
    Impl(const QString& mnem, const QString& display, const QString& asset, const QString& ccy,
         int lotNumer, int lotDenom, int tickNumer, int tickDenom, int pipDp, Lots minLots,
         Lots maxLots);
    Impl() = default;
    ~Impl() noexcept = default;
    QString mnem{};
    QString display{};
    QString asset{};
    QString ccy{};
    int lotNumer{};
    int lotDenom{};
    // Transient.
    double qtyInc{};
    int tickNumer{};
    int tickDenom{};
    // Transient.
    double priceInc{};
    int pipDp{};
    // Transient.
    int qtyDp{};
    // Transient.
    int priceDp{};
    Lots minLots{};
    Lots maxLots{};
  };
  static std::shared_ptr<const Impl> empty();
  std::shared_ptr<const Impl> impl_{empty()};
};

QDebug operator<<(QDebug debug, const Contr& contr);

inline Lots qtyToLots(double qty, const Contr& contr) noexcept
{
  return swirly::qtyToLots(qty, contr.qtyInc());
}

inline double lotsToQty(Lots lots, const Contr& contr) noexcept
{
  return swirly::lotsToQty(lots, contr.qtyInc());
}

inline QString lotsToQtyString(Lots lots, const Contr& contr)
{
  const auto qty = lotsToQty(lots, contr);
  return QString::number(qty, 'f', contr.qtyDp());
}

inline Ticks priceToTicks(double price, const Contr& contr) noexcept
{
  return swirly::priceToTicks(price, contr.priceInc());
}

inline double ticksToPrice(Ticks ticks, const Contr& contr) noexcept
{
  return swirly::ticksToPrice(ticks, contr.priceInc());
}

inline QString ticksToPriceString(Ticks ticks, const Contr& contr)
{
  const auto price = ticksToPrice(ticks, contr);
  return QString::number(price, 'f', contr.priceDp());
}

inline double ticksToAvgPrice(Lots lots, Cost cost, const Contr& contr) noexcept
{
  double ticks = 0;
  if (lots != 0_lts) {
    ticks = fractToReal(unbox(cost), unbox(lots));
  }
  return ticks * contr.priceInc();
}

inline QString ticksToAvgPriceString(Lots lots, Cost cost, const Contr& contr)
{
  const auto price = ticksToAvgPrice(lots, cost, contr);
  return QString::number(price, 'f', contr.priceDp() + 1);
}

} // ui
} // swirly

Q_DECLARE_METATYPE(swirly::ui::Contr)

#endif // SWIRLYUI_CONTR_HPP

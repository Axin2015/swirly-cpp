/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
#ifndef SWIRLYUI_INSTR_HXX
#define SWIRLYUI_INSTR_HXX

#include "Types.hxx"

#include <QMetaType>
#include <QString>

#include <memory>

class QJsonObject;

namespace swirly {
namespace ui {
namespace instr {

enum class Column : int { //
    CheckState, //
    Symbol, //
    Display, //
    BaseAsset, //
    TermCcy, //
    LotNumer, //
    LotDenom, //
    TickNumer, //
    TickDenom, //
    PipDp, //
    MinLots, //
    MaxLots
};
constexpr int ColumnCount{unbox(Column::MaxLots) + 1};

} // namespace instr

// Cheap copies.
class Instr {
  public:
    Instr(const QString& symbol, const QString& display, const QString& baseAsset,
          const QString& termCcy, int lotNumer, int lotDenom, int tickNumer, int tickDenom,
          int pipDp, Lots minLots, Lots maxLots)
        : impl_{std::make_shared<const Impl>(symbol, display, baseAsset, termCcy, lotNumer,
                                             lotDenom, tickNumer, tickDenom, pipDp, minLots,
                                             maxLots)}
    {
    }
    Instr() = default;
    ~Instr() noexcept = default;

    static Instr fromJson(const QJsonObject& obj);

    const QString& symbol() const noexcept { return impl_->symbol; }
    const QString& display() const noexcept { return impl_->display; }
    const QString& baseAsset() const noexcept { return impl_->baseAsset; }
    const QString& termCcy() const noexcept { return impl_->termCcy; }
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
        Impl(const QString& symbol, const QString& display, const QString& baseAsset,
             const QString& termCcy, int lotNumer, int lotDenom, int tickNumer, int tickDenom,
             int pipDp, Lots minLots, Lots maxLots);
        Impl() = default;
        ~Impl() noexcept = default;
        QString symbol{};
        QString display{};
        QString baseAsset{};
        QString termCcy{};
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

QDebug operator<<(QDebug debug, const Instr& instr);

inline bool isModified(const Instr& prev, const Instr& next) noexcept
{
    // Immutable.
    return false;
}

inline Lots qtyToLots(double qty, const Instr& instr) noexcept
{
    return swirly::qtyToLots(qty, instr.qtyInc());
}

inline double lotsToQty(Lots lots, const Instr& instr) noexcept
{
    return swirly::lotsToQty(lots, instr.qtyInc());
}

inline QString lotsToQtyString(Lots lots, const Instr& instr)
{
    const auto qty = lotsToQty(lots, instr);
    return QString::number(qty, 'f', instr.qtyDp());
}

inline Ticks priceToTicks(double price, const Instr& instr) noexcept
{
    return swirly::priceToTicks(price, instr.priceInc());
}

inline double ticksToPrice(Ticks ticks, const Instr& instr) noexcept
{
    return swirly::ticksToPrice(ticks, instr.priceInc());
}

inline QString ticksToPriceString(Ticks ticks, const Instr& instr)
{
    const auto price = ticksToPrice(ticks, instr);
    return QString::number(price, 'f', instr.priceDp());
}

inline double ticksToAvgPrice(Lots lots, Cost cost, const Instr& instr) noexcept
{
    double ticks = 0;
    if (lots != 0_lts) {
        ticks = fractToReal(cost.count(), lots.count());
    }
    return ticks * instr.priceInc();
}

inline QString ticksToAvgPriceString(Lots lots, Cost cost, const Instr& instr)
{
    const auto price = ticksToAvgPrice(lots, cost, instr);
    return QString::number(price, 'f', instr.priceDp() + 1);
}

} // namespace ui
} // namespace swirly

Q_DECLARE_METATYPE(swirly::ui::Instr)

#endif // SWIRLYUI_INSTR_HXX

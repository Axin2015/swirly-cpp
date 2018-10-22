/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
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
    CheckState,           //
    Id,                   //
    Symbol,               //
    Display,              //
    BaseAsset,            //
    TermCcy,              //
    LotNumer,             //
    LotDenom,             //
    TickNumer,            //
    TickDenom,            //
    PipDp,                //
    MinLots,              //
    MaxLots
};
constexpr int ColumnCount{unbox(Column::MaxLots) + 1};

} // namespace instr

// Cheap copies.
class Instr {
  public:
    Instr(Id32 id, const QString& symbol, const QString& display, const QString& base_asset,
          const QString& term_ccy, int lot_numer, int lot_denom, int tick_numer, int tick_denom,
          int pip_dp, Lots min_lots, Lots max_lots)
    : impl_{std::make_shared<const Impl>(id, symbol, display, base_asset, term_ccy, lot_numer,
                                         lot_denom, tick_numer, tick_denom, pip_dp, min_lots,
                                         max_lots)}
    {
    }
    Instr() = default;
    ~Instr() = default;

    static Instr from_json(const QJsonObject& obj);

    const Id32 id() const noexcept { return impl_->id; }
    const QString& symbol() const noexcept { return impl_->symbol; }
    const QString& display() const noexcept { return impl_->display; }
    const QString& base_asset() const noexcept { return impl_->base_asset; }
    const QString& term_ccy() const noexcept { return impl_->term_ccy; }
    int lot_numer() const noexcept { return impl_->lot_numer; }
    int lot_denom() const noexcept { return impl_->lot_denom; }
    double qty_inc() const noexcept { return impl_->qty_inc; }
    int tick_numer() const noexcept { return impl_->tick_numer; }
    int tick_denom() const noexcept { return impl_->tick_denom; }
    double price_inc() const noexcept { return impl_->price_inc; }
    int pip_dp() const noexcept { return impl_->pip_dp; }
    int qty_dp() const noexcept { return impl_->qty_dp; }
    int price_dp() const noexcept { return impl_->price_dp; }
    Lots min_lots() const noexcept { return impl_->min_lots; }
    Lots max_lots() const noexcept { return impl_->max_lots; }

  private:
    struct Impl {
        Impl(Id32 id, const QString& symbol, const QString& display, const QString& base_asset,
             const QString& term_ccy, int lot_numer, int lot_denom, int tick_numer, int tick_denom,
             int pip_dp, Lots min_lots, Lots max_lots);
        Impl() = default;
        ~Impl() = default;
        Id32 id{};
        QString symbol{};
        QString display{};
        QString base_asset{};
        QString term_ccy{};
        int lot_numer{};
        int lot_denom{};
        // Transient.
        double qty_inc{};
        int tick_numer{};
        int tick_denom{};
        // Transient.
        double price_inc{};
        int pip_dp{};
        // Transient.
        int qty_dp{};
        // Transient.
        int price_dp{};
        Lots min_lots{};
        Lots max_lots{};
    };
    static std::shared_ptr<const Impl> empty();
    std::shared_ptr<const Impl> impl_{empty()};
};

QDebug operator<<(QDebug debug, const Instr& instr);

inline bool is_modified(const Instr& prev, const Instr& next) noexcept
{
    // Immutable.
    return false;
}

inline Lots qty_to_lots(double qty, const Instr& instr) noexcept
{
    return swirly::qty_to_lots(qty, instr.qty_inc());
}

inline double lots_to_qty(Lots lots, const Instr& instr) noexcept
{
    return swirly::lots_to_qty(lots, instr.qty_inc());
}

inline QString lots_to_qty_string(Lots lots, const Instr& instr)
{
    const auto qty = lots_to_qty(lots, instr);
    return QString::number(qty, 'f', instr.qty_dp());
}

inline Ticks price_to_ticks(double price, const Instr& instr) noexcept
{
    return swirly::price_to_ticks(price, instr.price_inc());
}

inline double ticks_to_price(Ticks ticks, const Instr& instr) noexcept
{
    return swirly::ticks_to_price(ticks, instr.price_inc());
}

inline QString ticks_to_price_string(Ticks ticks, const Instr& instr)
{
    const auto price = ticks_to_price(ticks, instr);
    return QString::number(price, 'f', instr.price_dp());
}

inline double ticks_to_avg_price(Lots lots, Cost cost, const Instr& instr) noexcept
{
    double ticks = 0;
    if (lots != 0_lts) {
        ticks = fract_to_real(cost.count(), lots.count());
    }
    return ticks * instr.price_inc();
}

inline QString ticks_to_avg_price_string(Lots lots, Cost cost, const Instr& instr)
{
    const auto price = ticks_to_avg_price(lots, cost, instr);
    return QString::number(price, 'f', instr.price_dp() + 1);
}

} // namespace ui
} // namespace swirly

Q_DECLARE_METATYPE(swirly::ui::Instr)

#endif // SWIRLYUI_INSTR_HXX

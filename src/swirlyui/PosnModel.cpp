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
#include "PosnModel.hxx"

#include <algorithm>

namespace swirly {
namespace ui {
using namespace posn;
using namespace std;

PosnModel::PosnModel(QObject* parent)
: TableModel{parent}
{
    header_[unbox(Column::CheckState)] = tr("");
    header_[unbox(Column::Accnt)] = tr("Accnt");
    header_[unbox(Column::MarketId)] = tr("Market Id");
    header_[unbox(Column::Instr)] = tr("Instr");
    header_[unbox(Column::SettlDate)] = tr("Settl Date");
    header_[unbox(Column::BuyLots)] = tr("Buy Lots");
    header_[unbox(Column::BuyAvgPrice)] = tr("Buy Avg");
    header_[unbox(Column::SellLots)] = tr("Sell Lots");
    header_[unbox(Column::SellAvgPrice)] = tr("Sell Avg");
}

PosnModel::~PosnModel() = default;

QVariant PosnModel::data(const QModelIndex& index, int role) const
{
    QVariant var{};
    if (!index.isValid()) {
        // No-op.
    } else if (role == Qt::CheckStateRole) {
        const auto& row = row_at(index.row());
        switch (box<Column>(index.column())) {
        case Column::CheckState:
            var = row.checked() ? Qt::Checked : Qt::Unchecked;
            break;
        default:
            break;
        }
    } else if (role == Qt::DisplayRole) {
        const auto& posn = value_at(index.row());
        switch (box<Column>(index.column())) {
        case Column::CheckState:
            break;
        case Column::Accnt:
            var = posn.accnt();
            break;
        case Column::MarketId:
            var = to_variant(posn.market().id());
            break;
        case Column::Instr:
            var = posn.instr().symbol();
            break;
        case Column::SettlDate:
            var = posn.settl_date();
            break;
        case Column::BuyLots:
            var = to_variant(posn.buy_lots());
            break;
        case Column::BuyAvgPrice:
            var = ticks_to_avg_price_string(posn.buy_lots(), posn.buy_cost(), posn.instr());
            break;
        case Column::SellLots:
            var = to_variant(posn.sell_lots());
            break;
        case Column::SellAvgPrice:
            var = ticks_to_avg_price_string(posn.sell_lots(), posn.sell_cost(), posn.instr());
            break;
        }
    } else if (role == Qt::TextAlignmentRole) {
        switch (box<Column>(index.column())) {
        case Column::CheckState:
            break;
        case Column::Accnt:
            var = QVariant{Qt::AlignLeft | Qt::AlignVCenter};
            break;
        case Column::MarketId:
        case Column::Instr:
        case Column::SettlDate:
        case Column::BuyLots:
        case Column::BuyAvgPrice:
        case Column::SellLots:
        case Column::SellAvgPrice:
            var = QVariant{Qt::AlignRight | Qt::AlignVCenter};
            break;
        }
    } else if (role == Qt::UserRole) {
        var = QVariant::fromValue(value_at(index.row()));
    }
    return var;
}

QVariant PosnModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant var{};
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        var = header_[section];
    }
    return var;
}

} // namespace ui
} // namespace swirly

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
#include "OrderModel.hxx"

using namespace std;

namespace swirly {
namespace ui {
using namespace order;

OrderModel::OrderModel(QObject* parent) : TableModel{parent}
{
    header_[unbox(Column::CheckState)] = tr("");
    header_[unbox(Column::Accnt)] = tr("Accnt");
    header_[unbox(Column::MarketId)] = tr("Market Id");
    header_[unbox(Column::Contr)] = tr("Contr");
    header_[unbox(Column::SettlDate)] = tr("Settl Date");
    header_[unbox(Column::Id)] = tr("Id");
    header_[unbox(Column::Ref)] = tr("Ref");
    header_[unbox(Column::State)] = tr("State");
    header_[unbox(Column::Side)] = tr("Side");
    header_[unbox(Column::Lots)] = tr("Lots");
    header_[unbox(Column::Price)] = tr("Price");
    header_[unbox(Column::Resd)] = tr("Resd");
    header_[unbox(Column::Exec)] = tr("Exec");
    header_[unbox(Column::AvgPrice)] = tr("Avg Price");
    header_[unbox(Column::LastLots)] = tr("Last Lots");
    header_[unbox(Column::LastPrice)] = tr("Last Price");
    header_[unbox(Column::MinLots)] = tr("Min Lots");
    header_[unbox(Column::Created)] = tr("Created");
    header_[unbox(Column::Modified)] = tr("Modified");
}

OrderModel::~OrderModel() noexcept = default;

QVariant OrderModel::data(const QModelIndex& index, int role) const
{
    QVariant var{};
    if (!index.isValid()) {
        // No-op.
    } else if (role == Qt::CheckStateRole) {
        const auto& row = rowAt(index.row());
        switch (box<Column>(index.column())) {
        case Column::CheckState:
            var = row.checked() ? Qt::Checked : Qt::Unchecked;
            break;
        default:
            break;
        }
    } else if (role == Qt::DisplayRole) {
        const auto& order = valueAt(index.row());
        switch (box<Column>(index.column())) {
        case Column::CheckState:
            break;
        case Column::Accnt:
            var = order.accnt();
            break;
        case Column::MarketId:
            var = toVariant(order.marketId());
            break;
        case Column::Contr:
            var = order.contr().mnem();
            break;
        case Column::SettlDate:
            var = order.settlDate();
            break;
        case Column::Id:
            var = toVariant(order.id());
            break;
        case Column::Ref:
            var = order.ref();
            break;
        case Column::State:
            var = enumString(order.state());
            break;
        case Column::Side:
            var = enumString(order.side());
            break;
        case Column::Lots:
            var = toVariant(order.lots());
            break;
        case Column::Price:
            if (order.lots() != 0_lts) {
                var = ticksToPriceString(order.ticks(), order.contr());
            }
            break;
        case Column::Resd:
            var = toVariant(order.resd());
            break;
        case Column::Exec:
            var = toVariant(order.exec());
            break;
        case Column::AvgPrice:
            var = ticksToAvgPriceString(order.exec(), order.cost(), order.contr());
            break;
        case Column::LastLots:
            var = toVariant(order.lastLots());
            break;
        case Column::LastPrice:
            if (order.lastLots() != 0_lts) {
                var = ticksToPriceString(order.lastTicks(), order.contr());
            }
            break;
        case Column::MinLots:
            var = toVariant(order.minLots());
            break;
        case Column::Created:
            var = order.created();
            break;
        case Column::Modified:
            var = order.modified();
            break;
        }
    } else if (role == Qt::TextAlignmentRole) {
        switch (box<Column>(index.column())) {
        case Column::CheckState:
            break;
        case Column::Accnt:
        case Column::Contr:
        case Column::Ref:
        case Column::State:
        case Column::Side:
            var = QVariant{Qt::AlignLeft | Qt::AlignVCenter};
            break;
        case Column::MarketId:
        case Column::SettlDate:
        case Column::Id:
        case Column::Lots:
        case Column::Price:
        case Column::Resd:
        case Column::Exec:
        case Column::AvgPrice:
        case Column::LastLots:
        case Column::LastPrice:
        case Column::MinLots:
        case Column::Created:
        case Column::Modified:
            var = QVariant{Qt::AlignRight | Qt::AlignVCenter};
            break;
        }
    } else if (role == Qt::UserRole) {
        var = QVariant::fromValue(valueAt(index.row()));
    }
    return var;
}

QVariant OrderModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant var{};
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        var = header_[section];
    }
    return var;
}

OrderKeys OrderModel::checked() const
{
    OrderKeys keys;
    for (const auto& row : rows_) {
        if (row.second.checked()) {
            keys.push_back(row.first);
        }
    }
    std::sort(keys.begin(), keys.end());
    return keys;
}

} // ui
} // swirly

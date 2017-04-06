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
#include "TradeModel.hxx"

using namespace std;

namespace swirly {
namespace ui {
using namespace exec;

TradeModel::TradeModel(QObject* parent) : TableModel{parent}
{
    header_[unbox(Column::CheckState)] = tr("");
    header_[unbox(Column::Accnt)] = tr("Accnt");
    header_[unbox(Column::MarketId)] = tr("Market Id");
    header_[unbox(Column::Contr)] = tr("Contr");
    header_[unbox(Column::SettlDate)] = tr("Settl Date");
    header_[unbox(Column::Id)] = tr("Id");
    header_[unbox(Column::OrderId)] = tr("Order Id");
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
    header_[unbox(Column::MatchId)] = tr("Match Id");
    header_[unbox(Column::LiqInd)] = tr("Liq Ind");
    header_[unbox(Column::Cpty)] = tr("Cpty");
    header_[unbox(Column::Created)] = tr("Created");
}

TradeModel::~TradeModel() noexcept = default;

QVariant TradeModel::data(const QModelIndex& index, int role) const
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
        const auto& trade = valueAt(index.row());
        switch (box<Column>(index.column())) {
        case Column::CheckState:
            break;
        case Column::Accnt:
            var = trade.accnt();
            break;
        case Column::MarketId:
            var = toVariant(trade.marketId());
            break;
        case Column::Contr:
            var = trade.contr().symbol();
            break;
        case Column::SettlDate:
            var = trade.settlDate();
            break;
        case Column::Id:
            var = toVariant(trade.id());
            break;
        case Column::OrderId:
            var = toVariant(trade.orderId());
            break;
        case Column::Ref:
            var = trade.ref();
            break;
        case Column::State:
            var = enumString(trade.state(), trade.resd());
            break;
        case Column::Side:
            var = enumString(trade.side());
            break;
        case Column::Lots:
            var = toVariant(trade.lots());
            break;
        case Column::Price:
            if (trade.lots() != 0_lts) {
                var = ticksToPriceString(trade.ticks(), trade.contr());
            }
            break;
        case Column::Resd:
            var = toVariant(trade.resd());
            break;
        case Column::Exec:
            var = toVariant(trade.exec());
            break;
        case Column::AvgPrice:
            var = ticksToAvgPriceString(trade.exec(), trade.cost(), trade.contr());
            break;
        case Column::LastLots:
            var = toVariant(trade.lastLots());
            break;
        case Column::LastPrice:
            if (trade.lastLots() != 0_lts) {
                var = ticksToPriceString(trade.lastTicks(), trade.contr());
            }
            break;
        case Column::MinLots:
            var = toVariant(trade.minLots());
            break;
        case Column::MatchId:
            var = toVariant(trade.matchId());
            break;
        case Column::LiqInd:
            var = enumString(trade.liqInd());
            break;
        case Column::Cpty:
            var = trade.cpty();
            break;
        case Column::Created:
            var = trade.created();
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
        case Column::LiqInd:
        case Column::Cpty:
            var = QVariant{Qt::AlignLeft | Qt::AlignVCenter};
            break;
        case Column::MarketId:
        case Column::SettlDate:
        case Column::Id:
        case Column::OrderId:
        case Column::Lots:
        case Column::Price:
        case Column::Resd:
        case Column::Exec:
        case Column::AvgPrice:
        case Column::LastLots:
        case Column::LastPrice:
        case Column::MinLots:
        case Column::MatchId:
        case Column::Created:
            var = QVariant{Qt::AlignRight | Qt::AlignVCenter};
            break;
        }
    } else if (role == Qt::UserRole) {
        var = QVariant::fromValue(valueAt(index.row()));
    }
    return var;
}

QVariant TradeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant var{};
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        var = header_[section];
    }
    return var;
}

} // ui
} // swirly

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
#include "TradeModel.hxx"

namespace swirly {
namespace ui {
using namespace exec;
using namespace std;

TradeModel::TradeModel(QObject* parent)
: TableModel{parent}
{
    header_[unbox(Column::CheckState)] = tr("");
    header_[unbox(Column::Accnt)] = tr("Accnt");
    header_[unbox(Column::MarketId)] = tr("Market Id");
    header_[unbox(Column::Instr)] = tr("Instr");
    header_[unbox(Column::SettlDate)] = tr("Settl Date");
    header_[unbox(Column::Id)] = tr("Id");
    header_[unbox(Column::OrderId)] = tr("Order Id");
    header_[unbox(Column::Ref)] = tr("Ref");
    header_[unbox(Column::State)] = tr("State");
    header_[unbox(Column::Side)] = tr("Side");
    header_[unbox(Column::Lots)] = tr("Lots");
    header_[unbox(Column::Price)] = tr("Price");
    header_[unbox(Column::ResdLots)] = tr("Resd Lots");
    header_[unbox(Column::ExecLots)] = tr("Exec Lots");
    header_[unbox(Column::AvgPrice)] = tr("Avg Price");
    header_[unbox(Column::LastLots)] = tr("Last Lots");
    header_[unbox(Column::LastPrice)] = tr("Last Price");
    header_[unbox(Column::MinLots)] = tr("Min Lots");
    header_[unbox(Column::MatchId)] = tr("Match Id");
    header_[unbox(Column::LiqInd)] = tr("Liq Ind");
    header_[unbox(Column::Cpty)] = tr("Cpty");
    header_[unbox(Column::Created)] = tr("Created");
}

TradeModel::~TradeModel() = default;

QVariant TradeModel::data(const QModelIndex& index, int role) const
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
        const auto& trade = value_at(index.row());
        switch (box<Column>(index.column())) {
        case Column::CheckState:
            break;
        case Column::Accnt:
            var = trade.accnt();
            break;
        case Column::MarketId:
            var = to_variant(trade.market_id());
            break;
        case Column::Instr:
            var = trade.instr().symbol();
            break;
        case Column::SettlDate:
            var = trade.settl_date();
            break;
        case Column::Id:
            var = to_variant(trade.id());
            break;
        case Column::OrderId:
            var = to_variant(trade.order_id());
            break;
        case Column::Ref:
            var = trade.ref();
            break;
        case Column::State:
            var = enum_string(trade.state(), trade.resd_lots());
            break;
        case Column::Side:
            var = enum_string(trade.side());
            break;
        case Column::Lots:
            var = to_variant(trade.lots());
            break;
        case Column::Price:
            if (trade.lots() != 0_lts) {
                var = ticks_to_price_string(trade.ticks(), trade.instr());
            }
            break;
        case Column::ResdLots:
            var = to_variant(trade.resd_lots());
            break;
        case Column::ExecLots:
            var = to_variant(trade.exec_lots());
            break;
        case Column::AvgPrice:
            var = ticks_to_avg_price_string(trade.exec_lots(), trade.exec_cost(), trade.instr());
            break;
        case Column::LastLots:
            var = to_variant(trade.last_lots());
            break;
        case Column::LastPrice:
            if (trade.last_lots() != 0_lts) {
                var = ticks_to_price_string(trade.last_ticks(), trade.instr());
            }
            break;
        case Column::MinLots:
            var = to_variant(trade.min_lots());
            break;
        case Column::MatchId:
            var = to_variant(trade.match_id());
            break;
        case Column::LiqInd:
            var = enum_string(trade.liq_ind());
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
        case Column::Instr:
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
        case Column::ResdLots:
        case Column::ExecLots:
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
        var = QVariant::fromValue(value_at(index.row()));
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

} // namespace ui
} // namespace swirly

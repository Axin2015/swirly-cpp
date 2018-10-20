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
#include "ExecModel.hxx"

#include <algorithm>

namespace swirly {
namespace ui {
using namespace exec;
using namespace std;

ExecModel::ExecModel(QObject* parent)
: QAbstractTableModel{parent}
{
    header_[unbox(Column::CheckState)] = tr("");
    header_[unbox(Column::Created)] = tr("Created");
    header_[unbox(Column::Accnt)] = tr("Accnt");
    header_[unbox(Column::MarketId)] = tr("Market Id");
    header_[unbox(Column::Product)] = tr("Product");
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
}

ExecModel::~ExecModel() = default;

int ExecModel::rowCount(const QModelIndex& parent) const
{
    return rows_.size();
}

int ExecModel::columnCount(const QModelIndex& parent) const
{
    return ColumnCount;
}

QVariant ExecModel::data(const QModelIndex& index, int role) const
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
        const auto& exec = value_at(index.row());
        switch (box<Column>(index.column())) {
        case Column::CheckState:
            break;
        case Column::Created:
            var = exec.created();
            break;
        case Column::Accnt:
            var = exec.accnt();
            break;
        case Column::MarketId:
            var = to_variant(exec.market_id());
            break;
        case Column::Product:
            var = exec.product().symbol();
            break;
        case Column::SettlDate:
            var = exec.settl_date();
            break;
        case Column::Id:
            var = to_variant(exec.id());
            break;
        case Column::OrderId:
            var = to_variant(exec.order_id());
            break;
        case Column::Ref:
            var = exec.ref();
            break;
        case Column::State:
            var = enum_string(exec.state(), exec.resd_lots());
            break;
        case Column::Side:
            var = enum_string(exec.side());
            break;
        case Column::Lots:
            var = to_variant(exec.lots());
            break;
        case Column::Price:
            if (exec.lots() != 0_lts) {
                var = ticks_to_price_string(exec.ticks(), exec.product());
            }
            break;
        case Column::ResdLots:
            var = to_variant(exec.resd_lots());
            break;
        case Column::ExecLots:
            var = to_variant(exec.exec_lots());
            break;
        case Column::AvgPrice:
            var = ticks_to_avg_price_string(exec.exec_lots(), exec.exec_cost(), exec.product());
            break;
        case Column::LastLots:
            var = to_variant(exec.last_lots());
            break;
        case Column::LastPrice:
            if (exec.last_lots() != 0_lts) {
                var = ticks_to_price_string(exec.last_ticks(), exec.product());
            }
            break;
        case Column::MinLots:
            var = to_variant(exec.min_lots());
            break;
        case Column::MatchId:
            if (exec.match_id() != 0_id64) {
                var = to_variant(exec.match_id());
            }
            break;
        case Column::LiqInd:
            if (exec.match_id() != 0_id64) {
                var = enum_string(exec.liq_ind());
            }
            break;
        case Column::Cpty:
            if (exec.match_id() != 0_id64) {
                var = exec.cpty();
            }
            break;
        }
    } else if (role == Qt::TextAlignmentRole) {
        switch (box<Column>(index.column())) {
        case Column::CheckState:
            break;
        case Column::Created:
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
            var = QVariant{Qt::AlignRight | Qt::AlignVCenter};
            break;
        case Column::Accnt:
        case Column::Product:
        case Column::Ref:
        case Column::State:
        case Column::Side:
        case Column::LiqInd:
        case Column::Cpty:
            var = QVariant{Qt::AlignLeft | Qt::AlignVCenter};
            break;
        }
    } else if (role == Qt::UserRole) {
        var = QVariant::fromValue(value_at(index.row()));
    }
    return var;
}

QVariant ExecModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant var{};
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        var = header_[section];
    }
    return var;
}

void ExecModel::update_row(uint64_t tag, const Exec& exec)
{
    // Linear search is acceptable on small circular buffer.
    auto it = find_if(rows_.begin(), rows_.end(), [&exec](const auto& row) {
        return row.value().market_id() == exec.market_id() && row.value().id() == exec.id();
    });

    if (it != rows_.end()) {
        // Update tag for completeness. Note that this is not strictly necessary because the sweep
        // operation in not supported.
        it->set_tag(tag);
        if (is_modified(it->value(), exec)) {
            it->set_value(exec);
            const int row = distance(rows_.begin(), it);
            emit dataChanged(index(row, 0), index(row, ColumnCount - 1));
        }
    } else {
        if (rows_.full()) {
            rows_.push_front(Row<Exec>{tag, exec});
            emit dataChanged(index(0, 0), index(rows_.size() - 1, ColumnCount - 1));
        } else {
            beginInsertRows(QModelIndex{}, 0, 0);
            rows_.push_front(Row<Exec>{tag, exec});
            endInsertRows();
        }
    }
}

} // namespace ui
} // namespace swirly

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
#include "InstrModel.hxx"

using namespace std;

namespace swirly {
namespace ui {
using namespace instr;

InstrModel::InstrModel(QObject* parent)
  : TableModel{parent}
{
    header_[unbox(Column::CheckState)] = tr("");
    header_[unbox(Column::Symbol)] = tr("Symbol");
    header_[unbox(Column::Display)] = tr("Display");
    header_[unbox(Column::BaseAsset)] = tr("Base Asset");
    header_[unbox(Column::TermCcy)] = tr("Term Ccy");
    header_[unbox(Column::LotNumer)] = tr("Lot Numer");
    header_[unbox(Column::LotDenom)] = tr("Lot Denom");
    header_[unbox(Column::TickNumer)] = tr("Tick Numer");
    header_[unbox(Column::TickDenom)] = tr("Tick Denom");
    header_[unbox(Column::PipDp)] = tr("Pip Dp");
    header_[unbox(Column::MinLots)] = tr("Min Lots");
    header_[unbox(Column::MaxLots)] = tr("Max Lots");
}

InstrModel::~InstrModel() noexcept = default;

QVariant InstrModel::data(const QModelIndex& index, int role) const
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
        const auto& instr = valueAt(index.row());
        switch (box<Column>(index.column())) {
        case Column::CheckState:
            break;
        case Column::Symbol:
            var = instr.symbol();
            break;
        case Column::Display:
            var = instr.display();
            break;
        case Column::BaseAsset:
            var = instr.baseAsset();
            break;
        case Column::TermCcy:
            var = instr.termCcy();
            break;
        case Column::LotNumer:
            var = instr.lotNumer();
            break;
        case Column::LotDenom:
            var = instr.lotDenom();
            break;
        case Column::TickNumer:
            var = instr.tickNumer();
            break;
        case Column::TickDenom:
            var = instr.tickDenom();
            break;
        case Column::PipDp:
            var = instr.pipDp();
            break;
        case Column::MinLots:
            var = toVariant(instr.minLots());
            break;
        case Column::MaxLots:
            var = toVariant(instr.maxLots());
            break;
        }
    } else if (role == Qt::TextAlignmentRole) {
        switch (box<Column>(index.column())) {
        case Column::CheckState:
            break;
        case Column::Symbol:
        case Column::Display:
        case Column::BaseAsset:
        case Column::TermCcy:
            var = QVariant{Qt::AlignLeft | Qt::AlignVCenter};
            break;
        case Column::LotNumer:
        case Column::LotDenom:
        case Column::TickNumer:
        case Column::TickDenom:
        case Column::PipDp:
        case Column::MinLots:
        case Column::MaxLots:
            var = QVariant{Qt::AlignRight | Qt::AlignVCenter};
            break;
        }
    } else if (role == Qt::UserRole) {
        var = QVariant::fromValue(valueAt(index.row()));
    }
    return var;
}

QVariant InstrModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant var{};
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        var = header_[section];
    }
    return var;
}

Instr InstrModel::find(const QString& symbol) const
{
    Instr instr;
    auto it = rows_.find(symbol);
    if (it != rows_.end()) {
        instr = it->second.value();
    }
    return instr;
}

int InstrModel::indexOf(const QString& symbol) const
{
    int i{-1};
    auto it = rows_.find(symbol);
    if (it != rows_.end()) {
        i = distance(rows_.begin(), it);
    }
    return i;
}

} // namespace ui
} // namespace swirly

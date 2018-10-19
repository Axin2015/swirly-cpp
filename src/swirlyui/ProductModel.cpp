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
#include "ProductModel.hxx"

namespace swirly {
namespace ui {
using namespace product;
using namespace std;

ProductModel::ProductModel(QObject* parent)
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

ProductModel::~ProductModel() = default;

QVariant ProductModel::data(const QModelIndex& index, int role) const
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
        const auto& product = value_at(index.row());
        switch (box<Column>(index.column())) {
        case Column::CheckState:
            break;
        case Column::Symbol:
            var = product.symbol();
            break;
        case Column::Display:
            var = product.display();
            break;
        case Column::BaseAsset:
            var = product.base_asset();
            break;
        case Column::TermCcy:
            var = product.term_ccy();
            break;
        case Column::LotNumer:
            var = product.lot_numer();
            break;
        case Column::LotDenom:
            var = product.lot_denom();
            break;
        case Column::TickNumer:
            var = product.tick_numer();
            break;
        case Column::TickDenom:
            var = product.tick_denom();
            break;
        case Column::PipDp:
            var = product.pip_dp();
            break;
        case Column::MinLots:
            var = to_variant(product.min_lots());
            break;
        case Column::MaxLots:
            var = to_variant(product.max_lots());
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
        var = QVariant::fromValue(value_at(index.row()));
    }
    return var;
}

QVariant ProductModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant var{};
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        var = header_[section];
    }
    return var;
}

Product ProductModel::find(const QString& symbol) const
{
    Product product;
    auto it = rows_.find(symbol);
    if (it != rows_.end()) {
        product = it->second.value();
    }
    return product;
}

int ProductModel::index_of(const QString& symbol) const
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

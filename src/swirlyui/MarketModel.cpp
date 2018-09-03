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
#include "MarketModel.hxx"

namespace swirly {
namespace ui {
using namespace market;
using namespace std;

MarketModel::MarketModel(QObject* parent)
: TableModel{parent}
{
    header_[unbox(Column::CheckState)] = tr("");
    header_[unbox(Column::Id)] = tr("Id");
    header_[unbox(Column::Instr)] = tr("Instr");
    header_[unbox(Column::SettlDate)] = tr("Settl Date");
    header_[unbox(Column::State)] = tr("State");
    header_[unbox(Column::LastLots)] = tr("Last Lots");
    header_[unbox(Column::LastPrice)] = tr("Last Price");
    header_[unbox(Column::LastTime)] = tr("Last Time");
    header_[unbox(Column::BidPrice)] = tr("Bid Price");
    header_[unbox(Column::BidLots)] = tr("Bid Resd");
    header_[unbox(Column::BidCount)] = tr("Bid Count");
    header_[unbox(Column::OfferPrice)] = tr("Offer Price");
    header_[unbox(Column::OfferLots)] = tr("Offer Resd");
    header_[unbox(Column::OfferCount)] = tr("Offer Count");
}

MarketModel::~MarketModel() = default;

QVariant MarketModel::data(const QModelIndex& index, int role) const
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
        const auto& market = value_at(index.row());
        switch (box<Column>(index.column())) {
        case Column::CheckState:
            break;
        case Column::Id:
            var = to_variant(market.id());
            break;
        case Column::Instr:
            var = market.instr().symbol();
            break;
        case Column::SettlDate:
            var = market.settl_date();
            break;
        case Column::State:
            var = market.state();
            break;
        case Column::LastTime:
            var = market.last_time();
            break;
        case Column::LastLots:
            var = to_variant(market.last_lots());
            break;
        case Column::LastPrice:
            if (market.last_lots() != 0_lts) {
                var = ticks_to_price_string(market.last_ticks(), market.instr());
            }
            break;
        case Column::BidPrice:
            if (market.best_bid().lots() != 0_lts) {
                var = ticks_to_price_string(market.best_bid().ticks(), market.instr());
            }
            break;
        case Column::BidLots:
            var = to_variant(market.best_bid().lots());
            break;
        case Column::BidCount:
            var = market.best_bid().count();
            break;
        case Column::OfferPrice:
            if (market.best_offer().lots() != 0_lts) {
                var = ticks_to_price_string(market.best_offer().ticks(), market.instr());
            }
            break;
        case Column::OfferLots:
            var = to_variant(market.best_offer().lots());
            break;
        case Column::OfferCount:
            var = market.best_offer().count();
            break;
        }
    } else if (role == Qt::TextAlignmentRole) {
        switch (box<Column>(index.column())) {
        case Column::CheckState:
            break;
        case Column::Instr:
        case Column::State:
            var = QVariant{Qt::AlignLeft | Qt::AlignVCenter};
            break;
        case Column::Id:
        case Column::SettlDate:
        case Column::LastLots:
        case Column::LastPrice:
        case Column::LastTime:
        case Column::BidPrice:
        case Column::BidLots:
        case Column::BidCount:
        case Column::OfferPrice:
        case Column::OfferLots:
        case Column::OfferCount:
            var = QVariant{Qt::AlignRight | Qt::AlignVCenter};
            break;
        }
    } else if (role == Qt::UserRole) {
        var = QVariant::fromValue(value_at(index.row()));
    }
    return var;
}

QVariant MarketModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant var{};
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        var = header_[section];
    }
    return var;
}

Market MarketModel::find(Id64 id) const
{
    Market market;
    auto it = rows_.find(id);
    if (it != rows_.end()) {
        market = it->second.value();
    }
    return market;
}

int MarketModel::index_of(Id64 id) const
{
    int i{-1};
    auto it = rows_.find(id);
    if (it != rows_.end()) {
        i = distance(rows_.begin(), it);
    }
    return i;
}

} // namespace ui
} // namespace swirly

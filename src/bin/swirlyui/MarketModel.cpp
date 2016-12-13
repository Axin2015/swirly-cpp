/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
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
#include "MarketModel.hpp"

using namespace std;

namespace swirly {
namespace ui {
using namespace market;

MarketModel::MarketModel(QObject* parent) : TableModel{parent}
{
  header_[unbox(Column::CheckState)] = tr("");
  header_[unbox(Column::Id)] = tr("Id");
  header_[unbox(Column::Contr)] = tr("Contr");
  header_[unbox(Column::SettlDate)] = tr("Settl Date");
  header_[unbox(Column::State)] = tr("State");
  header_[unbox(Column::LastLots)] = tr("Last Lots");
  header_[unbox(Column::LastPrice)] = tr("Last Price");
  header_[unbox(Column::LastTime)] = tr("Last Time");
  header_[unbox(Column::BidPrice)] = tr("Bid Price");
  header_[unbox(Column::BidResd)] = tr("Bid Resd");
  header_[unbox(Column::BidCount)] = tr("Bid Count");
  header_[unbox(Column::OfferPrice)] = tr("Offer Price");
  header_[unbox(Column::OfferResd)] = tr("Offer Resd");
  header_[unbox(Column::OfferCount)] = tr("Offer Count");
}

MarketModel::~MarketModel() noexcept = default;

QVariant MarketModel::data(const QModelIndex& index, int role) const
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
    const auto& market = valueAt(index.row());
    switch (box<Column>(index.column())) {
    case Column::CheckState:
      break;
    case Column::Id:
      var = toVariant(market.id());
      break;
    case Column::Contr:
      var = market.contr().mnem();
      break;
    case Column::SettlDate:
      var = market.settlDate();
      break;
    case Column::State:
      var = market.state();
      break;
    case Column::LastLots:
      var = toVariant(market.lastLots());
      break;
    case Column::LastPrice:
      if (market.lastLots() != 0_lts) {
        var = ticksToPriceString(market.lastTicks(), market.contr());
      }
      break;
    case Column::LastTime:
      var = market.lastTime();
      break;
    case Column::BidPrice:
      if (market.bestBid().resd() != 0_lts) {
        var = ticksToPriceString(market.bestBid().ticks(), market.contr());
      }
      break;
    case Column::BidResd:
      var = toVariant(market.bestBid().resd());
      break;
    case Column::BidCount:
      var = market.bestBid().count();
      break;
    case Column::OfferPrice:
      if (market.bestOffer().resd() != 0_lts) {
        var = ticksToPriceString(market.bestOffer().ticks(), market.contr());
      }
      break;
    case Column::OfferResd:
      var = toVariant(market.bestOffer().resd());
      break;
    case Column::OfferCount:
      var = market.bestOffer().count();
      break;
    }
  } else if (role == Qt::TextAlignmentRole) {
    switch (box<Column>(index.column())) {
    case Column::CheckState:
      break;
    case Column::Contr:
    case Column::State:
      var = QVariant{Qt::AlignLeft | Qt::AlignVCenter};
      break;
    case Column::Id:
    case Column::SettlDate:
    case Column::LastLots:
    case Column::LastPrice:
    case Column::LastTime:
    case Column::BidPrice:
    case Column::BidResd:
    case Column::BidCount:
    case Column::OfferPrice:
    case Column::OfferResd:
    case Column::OfferCount:
      var = QVariant{Qt::AlignRight | Qt::AlignVCenter};
      break;
    }
  } else if (role == Qt::UserRole) {
    var = QVariant::fromValue(valueAt(index.row()));
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

int MarketModel::indexOf(Id64 id) const
{
  int i{-1};
  auto it = rows_.find(id);
  if (it != rows_.end()) {
    i = distance(rows_.begin(), it);
  }
  return i;
}

} // ui
} // swirly

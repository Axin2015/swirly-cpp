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

#include <algorithm>

using namespace std;

namespace swirly {
namespace ui {
using namespace market;

MarketModel::MarketModel(QObject* parent) : QAbstractTableModel{parent}
{
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

int MarketModel::rowCount(const QModelIndex& parent) const
{
  return rows_.size();
}

int MarketModel::columnCount(const QModelIndex& parent) const
{
  return ColumnCount;
}

QVariant MarketModel::data(const QModelIndex& index, int role) const
{
  QVariant var{};
  if (!index.isValid()) {
    // No-op.
  } else if (role == Qt::DisplayRole) {
    const auto& market = rows_.nth(index.row())->second;
    switch (box<Column>(index.column())) {
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
      var = ticksToPriceString(market.lastTicks(), market.contr());
      break;
    case Column::LastTime:
      var = market.lastTime();
      break;
    case Column::BidPrice:
      var = ticksToPriceString(market.bestBid().ticks(), market.contr());
      break;
    case Column::BidResd:
      var = toVariant(market.bestBid().resd());
      break;
    case Column::BidCount:
      var = market.bestBid().count();
      break;
    case Column::OfferPrice:
      var = ticksToPriceString(market.bestOffer().ticks(), market.contr());
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
    var = QVariant::fromValue(rows_.nth(index.row())->second);
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

void MarketModel::updateRow(const Market& market)
{
  auto it = rows_.lower_bound(market.id());
  const int i = distance(rows_.begin(), it);

  const bool found{it != rows_.end() && !rows_.key_comp()(market.id(), it->first)};
  if (found) {
    it->second = market;
    emit dataChanged(index(i, 0), index(i, ColumnCount - 1));
  } else {
    // If not found then insert.
    beginInsertRows(QModelIndex{}, i, i);
    rows_.emplace_hint(it, market.id(), market);
    endInsertRows();
  }
}

} // ui
} // swirly

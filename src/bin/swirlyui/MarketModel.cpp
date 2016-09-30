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
  header_[column::Id] = tr("Id");
  header_[column::Contr] = tr("Contr");
  header_[column::SettlDate] = tr("Settl Date");
  header_[column::State] = tr("State");
  header_[column::LastLots] = tr("Last Lots");
  header_[column::LastPrice] = tr("Last Price");
  header_[column::LastTime] = tr("Last Time");
  header_[column::BidPrice] = tr("Bid Price");
  header_[column::BidResd] = tr("Bid Resd");
  header_[column::BidCount] = tr("Bid Count");
  header_[column::OfferPrice] = tr("Offer Price");
  header_[column::OfferResd] = tr("Offer Resd");
  header_[column::OfferCount] = tr("Offer Count");
}

MarketModel::~MarketModel() noexcept = default;

int MarketModel::rowCount(const QModelIndex& parent) const
{
  return rows_.size();
}

int MarketModel::columnCount(const QModelIndex& parent) const
{
  return column::Count;
}

QVariant MarketModel::data(const QModelIndex& index, int role) const
{
  QVariant var{};
  if (!index.isValid()) {
    // No-op.
  } else if (role == Qt::DisplayRole) {
    const auto& market = rows_.nth(index.row())->second;
    switch (index.column()) {
    case column::Id:
      var = market.id();
      break;
    case column::Contr:
      var = market.contr().mnem();
      break;
    case column::SettlDate:
      var = market.settlDate();
      break;
    case column::State:
      var = market.state();
      break;
    case column::LastLots:
      var = market.lastLots();
      break;
    case column::LastPrice:
      var = ticksToPriceString(market.lastTicks(), market.contr());
      break;
    case column::LastTime:
      var = market.lastTime();
      break;
    case column::BidPrice:
      var = ticksToPriceString(market.bestBid().ticks(), market.contr());
      break;
    case column::BidResd:
      var = market.bestBid().resd();
      break;
    case column::BidCount:
      var = market.bestBid().count();
      break;
    case column::OfferPrice:
      var = ticksToPriceString(market.bestOffer().ticks(), market.contr());
      break;
    case column::OfferResd:
      var = market.bestOffer().resd();
      break;
    case column::OfferCount:
      var = market.bestOffer().count();
      break;
    }
  } else if (role == Qt::TextAlignmentRole) {
    var = QVariant{Qt::AlignLeft | Qt::AlignVCenter};
  } else if (role == Qt::UserRole) {
    var = QVariant::fromValue(rows_.nth(index.row())->second);
  }
  return var;
}

QVariant MarketModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
    return QVariant{};
  }
  return header_[section];
}

void MarketModel::updateRow(const Market& market)
{
  auto it = rows_.lower_bound(market.id());
  const int i = distance(rows_.begin(), it);

  const bool found{it != rows_.end() && !rows_.key_comp()(market.id(), it->first)};
  if (found) {
    it->second = market;
    emit dataChanged(index(i, 0), index(i, column::Count - 1));
  } else {
    // If not found then insert.
    beginInsertRows(QModelIndex{}, i, i);
    rows_.emplace_hint(it, market.id(), market);
    endInsertRows();
  }
}

} // ui
} // swirly

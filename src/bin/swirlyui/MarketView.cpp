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
#include "MarketView.hpp"

#include "MarketForm.hpp"
#include "MarketModel.hpp"
#include "Utility.hpp"

#include <QModelIndex>
#include <QTableView>
#include <QVBoxLayout>

#include <memory>

using namespace std;

namespace swirly {
namespace ui {
using namespace market;

MarketView::MarketView(ContrModel& contrModel, MarketModel& model, QWidget* parent,
                       Qt::WindowFlags f)
  : QWidget{parent, f}, model_(model)
{
  auto form = make_unique<MarketForm>(contrModel);
  connect(form.get(), &MarketForm::createMarket, this, &MarketView::createMarket);
  connect(form.get(), &MarketForm::createOrder, this, &MarketView::createOrder);

  auto table = make_unique<QTableView>();
  {
    auto del = makeDeleter(table->model());
    table->setModel(&model);
  }
  table->resizeColumnToContents(unbox(Column::CheckState));

  table->setColumnHidden(unbox(Column::CheckState), true);
  table->setColumnHidden(unbox(Column::Id), true);
  table->setColumnHidden(unbox(Column::State), true);
  table->setColumnHidden(unbox(Column::LastLots), true);
  table->setColumnHidden(unbox(Column::LastTime), true);

  table->setFocusPolicy(Qt::NoFocus);
  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  table->setSelectionMode(QAbstractItemView::NoSelection);

  connect(table.get(), &QTableView::clicked, this, &MarketView::slotClicked);

  auto layout = make_unique<QVBoxLayout>();
  layout->addWidget(marketForm_ = form.release());
  layout->addWidget(table.release());
  setLayout(layout.release());
}

MarketView::~MarketView() noexcept = default;

void MarketView::setFields(const QString& contrMnem, QDate settlDate, optional<Lots> lots,
                           optional<Ticks> ticks)
{
  marketForm_->setFields(contrMnem, settlDate, lots, ticks);
}

void MarketView::slotClicked(const QModelIndex& index)
{
  if (index.isValid()) {
    const auto& market = model_.valueAt(index.row());
    optional<Lots> lots;
    optional<Ticks> ticks;
    switch (box<Column>(index.column())) {
    case Column::CheckState:
      model_.toggleCheckState(index.row());
    // Fall-through.
    case Column::Id:
    case Column::Contr:
    case Column::SettlDate:
    case Column::State:
    case Column::LastLots:
    case Column::LastPrice:
    case Column::LastTime:
      if (market.lastLots() != 0_lts) {
        lots = market.lastLots();
        ticks = market.lastTicks();
      }
      break;
    case Column::BidCount:
    case Column::BidResd:
    case Column::BidPrice:
      if (market.bestBid().resd() != 0_lts) {
        lots = market.bestBid().resd();
        ticks = market.bestBid().ticks();
      } else if (market.lastLots() != 0_lts) {
        lots = market.lastLots();
        ticks = market.lastTicks();
      }
      break;
    case Column::OfferPrice:
    case Column::OfferResd:
    case Column::OfferCount:
      if (market.bestOffer().resd() != 0_lts) {
        lots = market.bestOffer().resd();
        ticks = market.bestOffer().ticks();
      } else if (market.lastLots() != 0_lts) {
        lots = market.lastLots();
        ticks = market.lastTicks();
      }
      break;
    }
    setFields(market.contr().mnem(), market.settlDate(), lots, ticks);
  }
}

} // ui
} // swirly

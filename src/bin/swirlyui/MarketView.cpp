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

#include <QGridLayout>
#include <QModelIndex>
#include <QTableView>

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
  connect(form.get(), &MarketForm::createOrder, this, &MarketView::createOrder);

  auto table = make_unique<QTableView>();
  {
    auto del = makeDeleter(table->model());
    table->setModel(&model);
  }
  table->setColumnHidden(unbox(Column::Id), true);
  table->setColumnHidden(unbox(Column::State), true);
  table->setColumnHidden(unbox(Column::LastLots), true);
  table->setColumnHidden(unbox(Column::LastTime), true);

  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  table->setSelectionMode(QAbstractItemView::SingleSelection);

  connect(table.get(), &QTableView::clicked, this, &MarketView::slotClicked);

  auto layout = make_unique<QVBoxLayout>();
  layout->addWidget(form.release());
  layout->addWidget(table.release());
  setLayout(layout.release());
}

MarketView::~MarketView() noexcept = default;

void MarketView::slotClicked(const QModelIndex& index)
{
}

} // ui
} // swirly

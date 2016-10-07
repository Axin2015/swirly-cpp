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
#include "TradeView.hpp"

#include "TradeModel.hpp"
#include "Utility.hpp"

#include <QGridLayout>
#include <QModelIndex>
#include <QTableView>

#include <memory>

using namespace std;

namespace swirly {
namespace ui {
using namespace exec;

TradeView::TradeView(TradeModel& model, QWidget* parent, Qt::WindowFlags f)
  : QWidget{parent, f}, model_(model)
{
  auto table = make_unique<QTableView>();
  {
    auto del = makeDeleter(table->model());
    table->setModel(&model);
  }
  table->resizeColumnToContents(unbox(Column::CheckState));

  table->setColumnHidden(unbox(Column::MarketId), true);
  table->setColumnHidden(unbox(Column::Accnt), true);
  table->setColumnHidden(unbox(Column::State), true);
  table->setColumnHidden(unbox(Column::MinLots), true);

  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  table->setSelectionMode(QAbstractItemView::SingleSelection);

  connect(table.get(), &QTableView::clicked, this, &TradeView::slotClicked);

  auto layout = make_unique<QGridLayout>();
  layout->addWidget(table.release(), 0, 0);
  setLayout(layout.release());
}

TradeView::~TradeView() noexcept = default;

void TradeView::slotClicked(const QModelIndex& index)
{
  if (index.isValid() && box<Column>(index.column()) == Column::CheckState) {
    model_.toggleCheckState(index.row());
  }
}

} // ui
} // swirly

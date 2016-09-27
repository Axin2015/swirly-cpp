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
#include "OrderView.hpp"

#include "OrderModel.hpp"

#include <QGridLayout>
#include <QModelIndex>
#include <QTableView>

#include <memory>

using namespace std;

namespace swirly {
namespace ui {

void OrderView::slotCurrentChanged(const QModelIndex& current, const QModelIndex& previous)
{
  if (current.isValid()) {
    QVariant var{model_.data(current, Qt::UserRole)};
    emit currentChanged(var.value<Order>());
  }
}

void OrderView::slotDoubleClicked(const QModelIndex& index)
{
  if (index.isValid()) {
    QVariant var{model_.data(index, Qt::UserRole)};
    emit doubleClicked(var.value<Order>());
  }
}

OrderView::OrderView(OrderModel& model, QWidget* parent, Qt::WindowFlags f)
  : QWidget{parent, f}, model_{model}
{
  QTableView* const table{new QTableView(this)};
  unique_ptr<QAbstractItemModel> prev(table->model());
  table->setModel(&model);

  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  table->setSelectionMode(QAbstractItemView::SingleSelection);

  QGridLayout* const layout{new QGridLayout(this)};
  layout->addWidget(table, 0, 0);
}

} // ui
} // swirly

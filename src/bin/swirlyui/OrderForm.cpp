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
#include "OrderForm.hpp"

#include "OrderModel.hpp"

#include <QHBoxLayout>
#include <QPushButton>

#include <memory>

using namespace std;

namespace swirly {
namespace ui {

OrderForm::OrderForm(OrderModel& orderModel, QWidget* parent, Qt::WindowFlags f)
  : QWidget{parent, f}, orderModel_(orderModel)
{
  auto selectAllButton = make_unique<QPushButton>(tr("Select All"));
  auto selectNoneButton = make_unique<QPushButton>(tr("Select None"));
  auto cancelButton = make_unique<QPushButton>(tr("Cancel"));

  connect(selectAllButton.get(), &QPushButton::clicked, this, &OrderForm::slotSelectAllClicked);
  connect(selectNoneButton.get(), &QPushButton::clicked, this, &OrderForm::slotSelectNoneClicked);
  connect(cancelButton.get(), &QPushButton::clicked, this, &OrderForm::slotCancelOrdersClicked);

  auto layout = make_unique<QHBoxLayout>();
  layout->addWidget(selectAllButton.release());
  layout->addWidget(selectNoneButton.release());
  layout->addWidget(cancelButton.release());
  layout->addStretch(1);

  setLayout(layout.release());
}

OrderForm::~OrderForm() noexcept = default;

void OrderForm::slotSelectAllClicked()
{
  orderModel_.setChecked(true);
}

void OrderForm::slotSelectNoneClicked()
{
  orderModel_.setChecked(false);
}

void OrderForm::slotCancelOrdersClicked()
{
  emit cancelOrders(orderModel_.checked());
}

} // ui
} // swirly

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
#include "OrderForm.hxx"

#include "OrderModel.hxx"

#include <QHBoxLayout>
#include <QPushButton>

#include <memory>

namespace swirly {
namespace ui {
using namespace std;

OrderForm::OrderForm(OrderModel& order_model, QWidget* parent, Qt::WindowFlags f)
: QWidget{parent, f}
, order_model_(order_model)
{
    auto select_all_button = make_unique<QPushButton>(tr("Select All"));
    auto select_none_button = make_unique<QPushButton>(tr("Select None"));
    auto cancel_button = make_unique<QPushButton>(tr("Cancel"));

    connect(select_all_button.get(), &QPushButton::clicked, this,
            &OrderForm::slot_select_all_clicked);
    connect(select_none_button.get(), &QPushButton::clicked, this,
            &OrderForm::slot_select_none_clicked);
    connect(cancel_button.get(), &QPushButton::clicked, this,
            &OrderForm::slot_cancel_orders_clicked);

    auto layout = make_unique<QHBoxLayout>();
    layout->addWidget(select_all_button.release());
    layout->addWidget(select_none_button.release());
    layout->addWidget(cancel_button.release());
    layout->addStretch(1);

    setLayout(layout.release());
}

OrderForm::~OrderForm() = default;

void OrderForm::slot_select_all_clicked()
{
    order_model_.set_checked(true);
}

void OrderForm::slot_select_none_clicked()
{
    order_model_.set_checked(false);
}

void OrderForm::slot_cancel_orders_clicked()
{
    emit cancel_orders(order_model_.checked());
}

} // namespace ui
} // namespace swirly

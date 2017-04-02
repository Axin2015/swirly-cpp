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
#include "OrderView.hxx"

#include "OrderForm.hxx"
#include "OrderModel.hxx"
#include "Utility.hxx"

#include <QModelIndex>
#include <QTableView>
#include <QVBoxLayout>

#include <memory>

using namespace std;

namespace swirly {
namespace ui {
using namespace order;

OrderView::OrderView(OrderModel& model, QWidget* parent, Qt::WindowFlags f)
    : QWidget{parent, f}, model_(model)
{
    auto form = make_unique<OrderForm>(model);
    connect(form.get(), &OrderForm::cancelOrders, this, &OrderView::cancelOrders);

    auto table = make_unique<QTableView>();
    {
        auto del = makeDeleter(table->model());
        table->setModel(&model);
    }
    table->resizeColumnToContents(unbox(Column::CheckState));

    table->setColumnHidden(unbox(Column::Accnt), true);
    table->setColumnHidden(unbox(Column::MarketId), true);
    table->setColumnHidden(unbox(Column::MinLots), true);

    table->setFocusPolicy(Qt::NoFocus);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::NoSelection);

    connect(table.get(), &QTableView::clicked, this, &OrderView::slotClicked);

    auto layout = make_unique<QVBoxLayout>();
    layout->addWidget(form.release(), 0, 0);
    layout->addWidget(table.release(), 0, 0);
    setLayout(layout.release());
}

OrderView::~OrderView() noexcept = default;

void OrderView::slotClicked(const QModelIndex& index)
{
    if (index.isValid() && box<Column>(index.column()) == Column::CheckState) {
        model_.toggleCheckState(index.row());
    }
    const auto& order = model_.valueAt(index.row());
    const auto lots = order.resd() > 0_lts ? order.resd() : order.lots();
    const auto ticks = order.ticks();
    emit setFields(order.contr().mnem(), order.settlDate(), lots, ticks);
}

} // ui
} // swirly

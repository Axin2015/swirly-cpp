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
#include "TradeView.hxx"

#include "TradeModel.hxx"
#include "Utility.hxx"

#include <QGridLayout>
#include <QModelIndex>
#include <QTableView>

#include <memory>

namespace swirly {
namespace ui {
using namespace exec;
using namespace std;

TradeView::TradeView(TradeModel& model, QWidget* parent, Qt::WindowFlags f)
: QWidget{parent, f}
, model_(model)
{
    auto table = make_unique<QTableView>();
    {
        auto del = make_deleter(table->model());
        table->setModel(&model);
    }
    table->resizeColumnToContents(unbox(Column::CheckState));

    table->setColumnHidden(unbox(Column::Accnt), true);
    table->setColumnHidden(unbox(Column::MarketId), true);
    table->setColumnHidden(unbox(Column::MinLots), true);

    table->setFocusPolicy(Qt::NoFocus);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::NoSelection);

    connect(table.get(), &QTableView::clicked, this, &TradeView::slot_clicked);

    auto layout = make_unique<QGridLayout>();
    layout->addWidget(table.release(), 0, 0);
    setLayout(layout.release());
}

TradeView::~TradeView() = default;

void TradeView::slot_clicked(const QModelIndex& index)
{
    if (index.isValid() && box<Column>(index.column()) == Column::CheckState) {
        model_.toggle_check_state(index.row());
    }
    const auto& trade = model_.value_at(index.row());
    const auto lots = trade.resd_lots() > 0_lts ? trade.resd_lots() : trade.lots();
    const auto ticks = trade.ticks();
    emit set_fields(trade.instr().symbol(), trade.settl_date(), lots, ticks);
}

} // namespace ui
} // namespace swirly

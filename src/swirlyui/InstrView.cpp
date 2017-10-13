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
#include "InstrView.hxx"

#include "InstrModel.hxx"
#include "Utility.hxx"

#include <QGridLayout>
#include <QModelIndex>
#include <QTableView>

#include <memory>

using namespace std;

namespace swirly {
namespace ui {
using namespace instr;

InstrView::InstrView(InstrModel& model, QWidget* parent, Qt::WindowFlags f)
    : QWidget{parent, f}, model_(model)
{
    auto table = make_unique<QTableView>();
    {
        auto del = makeDeleter(table->model());
        table->setModel(&model);
    }
    table->resizeColumnToContents(unbox(Column::CheckState));

    table->setColumnHidden(unbox(Column::CheckState), true);

    table->setFocusPolicy(Qt::NoFocus);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::NoSelection);

    connect(table.get(), &QTableView::clicked, this, &InstrView::slotClicked);

    auto layout = make_unique<QGridLayout>();
    layout->addWidget(table_ = table.release(), 0, 0);
    setLayout(layout.release());
}

InstrView::~InstrView() noexcept = default;

void InstrView::resizeColumnsToContents()
{
    table_->resizeColumnsToContents();
}

void InstrView::slotClicked(const QModelIndex& index)
{
    if (index.isValid() && box<Column>(index.column()) == Column::CheckState) {
        model_.toggleCheckState(index.row());
    }
}

} // namespace ui
} // namespace swirly

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
#include "MarketView.hxx"

#include "MarketForm.hxx"
#include "MarketModel.hxx"
#include "Utility.hxx"

#include <QModelIndex>
#include <QTableView>
#include <QVBoxLayout>

#include <memory>

namespace swirly {
namespace ui {
using namespace market;
using namespace std;

MarketView::MarketView(InstrModel& instr_model, MarketModel& model, QWidget* parent,
                       Qt::WindowFlags f)
: QWidget{parent, f}
, model_(model)
{
    auto form = make_unique<MarketForm>(instr_model);
    connect(form.get(), &MarketForm::create_market, this, &MarketView::create_market);
    connect(form.get(), &MarketForm::create_order, this, &MarketView::create_order);

    auto table = make_unique<QTableView>();
    {
        auto del = make_deleter(table->model());
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

    connect(table.get(), &QTableView::clicked, this, &MarketView::slot_clicked);

    auto layout = make_unique<QVBoxLayout>();
    layout->addWidget(market_form_ = form.release());
    layout->addWidget(table.release());
    setLayout(layout.release());
}

MarketView::~MarketView() = default;

void MarketView::set_fields(const QString& instr_symbol, QDate settl_date, optional<Lots> lots,
                            optional<Ticks> ticks)
{
    market_form_->set_fields(instr_symbol, settl_date, lots, ticks);
}

void MarketView::slot_clicked(const QModelIndex& index)
{
    if (index.isValid()) {
        const auto& market = model_.value_at(index.row());
        optional<Lots> lots;
        optional<Ticks> ticks;
        switch (box<Column>(index.column())) {
        case Column::CheckState:
            model_.toggle_check_state(index.row());
        // Fall-through.
        case Column::Id:
        case Column::Instr:
        case Column::SettlDate:
        case Column::State:
        case Column::LastLots:
        case Column::LastPrice:
        case Column::LastTime:
            if (market.last_lots() != 0_lts) {
                lots = market.last_lots();
                ticks = market.last_ticks();
            }
            break;
        case Column::BidCount:
        case Column::BidLots:
        case Column::BidPrice:
            if (market.best_bid().lots() != 0_lts) {
                lots = market.best_bid().lots();
                ticks = market.best_bid().ticks();
            } else if (market.last_lots() != 0_lts) {
                lots = market.last_lots();
                ticks = market.last_ticks();
            }
            break;
        case Column::OfferPrice:
        case Column::OfferLots:
        case Column::OfferCount:
            if (market.best_offer().lots() != 0_lts) {
                lots = market.best_offer().lots();
                ticks = market.best_offer().ticks();
            } else if (market.last_lots() != 0_lts) {
                lots = market.last_lots();
                ticks = market.last_ticks();
            }
            break;
        }
        set_fields(market.instr().symbol(), market.settl_date(), lots, ticks);
    }
}

} // namespace ui
} // namespace swirly

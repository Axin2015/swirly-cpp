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
#include "MarketForm.hxx"

#include "InstrModel.hxx"
#include "Utility.hxx"

#include <QAbstractItemModel>
#include <QApplication>
#include <QComboBox>
#include <QDateEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <memory>

namespace swirly {
namespace ui {
using namespace std;

MarketForm::MarketForm(InstrModel& instr_model, QWidget* parent, Qt::WindowFlags f)
: QWidget{parent, f}
, instr_model_(instr_model)
{
    auto instr_combo_box = make_unique<QComboBox>();
    {
        auto del = make_deleter(instr_combo_box->model());
        instr_combo_box->setModel(&instr_model);
        instr_combo_box->setModelColumn(unbox(instr::Column::Symbol));
    }
    QFontMetrics fm{QApplication::font()};
    instr_combo_box->setMinimumWidth(fm.averageCharWidth() * 12);

    auto settl_date_edit = make_unique<QDateEdit>(QDate::currentDate().addDays(2));
    auto lots_edit = make_unique<QLineEdit>();
    lots_edit->setPlaceholderText(tr("Lots"));
    {
        auto del = make_deleter(const_cast<QValidator*>(lots_edit->validator()));
        lots_edit->setValidator(&lots_validator_);
    }
    auto price_edit = make_unique<QLineEdit>();
    price_edit->setPlaceholderText(tr("Price"));
    {
        auto del = make_deleter(const_cast<QValidator*>(price_edit->validator()));
        price_edit->setValidator(&price_validator_);
    }
    auto create_button = make_unique<QPushButton>(tr("Create"));
    auto buy_button = make_unique<QPushButton>(tr("Buy"));
    auto sell_button = make_unique<QPushButton>(tr("Sell"));

    connect(instr_combo_box.get(),
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &MarketForm::slot_instr_changed);
    connect(create_button.get(), &QPushButton::clicked, [this]() { this->slot_create_clicked(); });
    connect(buy_button.get(), &QPushButton::clicked,
            [this]() { this->slot_buy_or_sell_clicked(Side::Buy); });
    connect(sell_button.get(), &QPushButton::clicked,
            [this]() { this->slot_buy_or_sell_clicked(Side::Sell); });

    auto layout = make_unique<QHBoxLayout>();
    layout->addWidget(instr_combo_box_ = instr_combo_box.release());
    layout->addWidget(settl_date_edit_ = settl_date_edit.release());
    layout->addWidget(lots_edit_ = lots_edit.release());
    layout->addWidget(new QLabel{tr("@")});
    layout->addWidget(price_edit_ = price_edit.release());
    layout->addWidget(create_button.release());
    layout->addWidget(buy_button.release());
    layout->addWidget(sell_button.release());
    layout->addStretch(1);

    setLayout(layout.release());
}

MarketForm::~MarketForm() = default;

void MarketForm::set_fields(const QString& instr_symbol, QDate settl_date, std::optional<Lots> lots,
                            std::optional<Ticks> ticks)
{
    if (!instr_symbol.isNull()) {
        const auto i = instr_model_.index_of(instr_symbol);
        if (i >= 0) {
            instr_combo_box_->setCurrentIndex(i);
        }
    }
    if (!settl_date.isNull()) {
        settl_date_edit_->setDate(settl_date);
    }
    const auto instr = instr_combo_box_->currentData().value<Instr>();
    if (lots) {
        lots_edit_->setText(QString::number(lots->count()));
    }
    if (ticks) {
        price_edit_->setText(ticks_to_price_string(*ticks, instr));
    }
}

void MarketForm::slot_instr_changed(int index)
{
    if (index < 0) {
        // No item selected.
        return;
    }
    const auto instr = instr_combo_box_->currentData().value<Instr>();
    qDebug().nospace() << "slot_instr_changed: " << instr;
    lots_validator_.setRange(instr.min_lots().count(), instr.max_lots().count());
    price_validator_.setDecimals(instr.price_dp());
}

void MarketForm::slot_create_clicked()
{
    const auto instr = instr_combo_box_->currentData().value<Instr>();
    emit create_market(instr, settl_date_edit_->date());
}

void MarketForm::slot_buy_or_sell_clicked(Side side)
{
    const auto instr = instr_combo_box_->currentData().value<Instr>();
    const auto lots = Lots{lots_edit_->text().toLongLong()};
    const auto ticks = price_to_ticks(price_edit_->text().toDouble(), instr);
    emit create_order(instr, settl_date_edit_->date(), "", side, lots, ticks);
}

} // namespace ui
} // namespace swirly

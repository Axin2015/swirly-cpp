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

#include "ProductModel.hxx"
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

MarketForm::MarketForm(ProductModel& product_model, QWidget* parent, Qt::WindowFlags f)
: QWidget{parent, f}
, product_model_(product_model)
{
    auto product_combo_box = make_unique<QComboBox>();
    {
        auto del = make_deleter(product_combo_box->model());
        product_combo_box->setModel(&product_model);
        product_combo_box->setModelColumn(unbox(product::Column::Symbol));
    }
    QFontMetrics fm{QApplication::font()};
    product_combo_box->setMinimumWidth(fm.averageCharWidth() * 12);

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

    connect(product_combo_box.get(),
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &MarketForm::slot_product_changed);
    connect(create_button.get(), &QPushButton::clicked, [this]() { this->slot_create_clicked(); });
    connect(buy_button.get(), &QPushButton::clicked,
            [this]() { this->slot_buy_or_sell_clicked(Side::Buy); });
    connect(sell_button.get(), &QPushButton::clicked,
            [this]() { this->slot_buy_or_sell_clicked(Side::Sell); });

    auto layout = make_unique<QHBoxLayout>();
    layout->addWidget(product_combo_box_ = product_combo_box.release());
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

void MarketForm::set_fields(const QString& product_symbol, QDate settl_date,
                            std::optional<Lots> lots, std::optional<Ticks> ticks)
{
    if (!product_symbol.isNull()) {
        const auto i = product_model_.index_of(product_symbol);
        if (i >= 0) {
            product_combo_box_->setCurrentIndex(i);
        }
    }
    if (!settl_date.isNull()) {
        settl_date_edit_->setDate(settl_date);
    }
    const auto product = product_combo_box_->currentData().value<Product>();
    if (lots) {
        lots_edit_->setText(QString::number(lots->count()));
    }
    if (ticks) {
        price_edit_->setText(ticks_to_price_string(*ticks, product));
    }
}

void MarketForm::slot_product_changed(int index)
{
    if (index < 0) {
        // No item selected.
        return;
    }
    const auto product = product_combo_box_->currentData().value<Product>();
    qDebug().nospace() << "slot_product_changed: " << product;
    lots_validator_.setRange(product.min_lots().count(), product.max_lots().count());
    price_validator_.setDecimals(product.price_dp());
}

void MarketForm::slot_create_clicked()
{
    const auto product = product_combo_box_->currentData().value<Product>();
    emit create_market(product, settl_date_edit_->date());
}

void MarketForm::slot_buy_or_sell_clicked(Side side)
{
    const auto product = product_combo_box_->currentData().value<Product>();
    const auto lots = Lots{lots_edit_->text().toLongLong()};
    const auto ticks = price_to_ticks(price_edit_->text().toDouble(), product);
    emit create_order(product, settl_date_edit_->date(), "", side, lots, ticks);
}

} // namespace ui
} // namespace swirly

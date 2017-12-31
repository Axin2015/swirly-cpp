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

using namespace std;

namespace swirly {
namespace ui {

MarketForm::MarketForm(InstrModel& instrModel, QWidget* parent, Qt::WindowFlags f)
  : QWidget{parent, f}
  , instrModel_(instrModel)
{
    auto instrComboBox = make_unique<QComboBox>();
    {
        auto del = makeDeleter(instrComboBox->model());
        instrComboBox->setModel(&instrModel);
        instrComboBox->setModelColumn(unbox(instr::Column::Symbol));
    }
    QFontMetrics fm{QApplication::font()};
    instrComboBox->setMinimumWidth(fm.averageCharWidth() * 12);

    auto settlDateEdit = make_unique<QDateEdit>(QDate::currentDate().addDays(2));
    auto lotsEdit = make_unique<QLineEdit>();
    lotsEdit->setPlaceholderText(tr("Lots"));
    {
        auto del = makeDeleter(const_cast<QValidator*>(lotsEdit->validator()));
        lotsEdit->setValidator(&lotsValidator_);
    }
    auto priceEdit = make_unique<QLineEdit>();
    priceEdit->setPlaceholderText(tr("Price"));
    {
        auto del = makeDeleter(const_cast<QValidator*>(priceEdit->validator()));
        priceEdit->setValidator(&priceValidator_);
    }
    auto createButton = make_unique<QPushButton>(tr("Create"));
    auto buyButton = make_unique<QPushButton>(tr("Buy"));
    auto sellButton = make_unique<QPushButton>(tr("Sell"));

    connect(instrComboBox.get(),
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &MarketForm::slotInstrChanged);
    connect(createButton.get(), &QPushButton::clicked, [this]() { this->slotCreateClicked(); });
    connect(buyButton.get(), &QPushButton::clicked,
            [this]() { this->slotBuyOrSellClicked(Side::Buy); });
    connect(sellButton.get(), &QPushButton::clicked,
            [this]() { this->slotBuyOrSellClicked(Side::Sell); });

    auto layout = make_unique<QHBoxLayout>();
    layout->addWidget(instrComboBox_ = instrComboBox.release());
    layout->addWidget(settlDateEdit_ = settlDateEdit.release());
    layout->addWidget(lotsEdit_ = lotsEdit.release());
    layout->addWidget(new QLabel{tr("@")});
    layout->addWidget(priceEdit_ = priceEdit.release());
    layout->addWidget(createButton.release());
    layout->addWidget(buyButton.release());
    layout->addWidget(sellButton.release());
    layout->addStretch(1);

    setLayout(layout.release());
}

MarketForm::~MarketForm() noexcept = default;

void MarketForm::setFields(const QString& instrSymbol, QDate settlDate, std::optional<Lots> lots,
                           std::optional<Ticks> ticks)
{
    if (!instrSymbol.isNull()) {
        const auto i = instrModel_.indexOf(instrSymbol);
        if (i >= 0) {
            instrComboBox_->setCurrentIndex(i);
        }
    }
    if (!settlDate.isNull()) {
        settlDateEdit_->setDate(settlDate);
    }
    const auto instr = instrComboBox_->currentData().value<Instr>();
    if (lots) {
        lotsEdit_->setText(QString::number(lots->count()));
    }
    if (ticks) {
        priceEdit_->setText(ticksToPriceString(*ticks, instr));
    }
}

void MarketForm::slotInstrChanged(int index)
{
    if (index < 0) {
        // No item selected.
        return;
    }
    const auto instr = instrComboBox_->currentData().value<Instr>();
    qDebug().nospace() << "slotInstrChanged: " << instr;
    lotsValidator_.setRange(instr.minLots().count(), instr.maxLots().count());
    priceValidator_.setDecimals(instr.priceDp());
}

void MarketForm::slotCreateClicked()
{
    const auto instr = instrComboBox_->currentData().value<Instr>();
    emit createMarket(instr, settlDateEdit_->date());
}

void MarketForm::slotBuyOrSellClicked(Side side)
{
    const auto instr = instrComboBox_->currentData().value<Instr>();
    const auto lots = Lots{lotsEdit_->text().toLongLong()};
    const auto ticks = priceToTicks(priceEdit_->text().toDouble(), instr);
    emit createOrder(instr, settlDateEdit_->date(), "", side, lots, ticks);
}

} // namespace ui
} // namespace swirly

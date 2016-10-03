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
#include "MarketForm.hpp"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

#include <memory>

using namespace std;

namespace swirly {
namespace ui {

MarketForm::MarketForm(QWidget* parent, Qt::WindowFlags f) : QWidget{parent, f}
{
  auto contrEdit = make_unique<QLineEdit>();
  contrEdit->setPlaceholderText(tr("Contr"));
  auto settlDateEdit = make_unique<QLineEdit>();
  settlDateEdit->setPlaceholderText(tr("Settl Date"));
  auto lotsEdit = make_unique<QLineEdit>();
  lotsEdit->setPlaceholderText(tr("Lots"));
  auto priceEdit = make_unique<QLineEdit>();
  priceEdit->setPlaceholderText(tr("Price"));
  auto buyButton = make_unique<QPushButton>(tr("Buy"));
  auto sellButton = make_unique<QPushButton>(tr("Sell"));

  auto layout = make_unique<QHBoxLayout>();
  layout->addWidget(contrEdit.release());
  layout->addWidget(settlDateEdit.release());
  layout->addWidget(lotsEdit.release());
  layout->addWidget(priceEdit.release());
  layout->addWidget(buyButton.release());
  layout->addWidget(sellButton.release());
  layout->addStretch(1);

  setLayout(layout.release());
}

MarketForm::~MarketForm() noexcept = default;

} // ui
} // swirly

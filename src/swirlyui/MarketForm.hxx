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
#ifndef SWIRLYUI_MARKETFORM_HXX
#define SWIRLYUI_MARKETFORM_HXX

#include "Types.hxx"

#include <QDoubleValidator>
#include <QIntValidator>
#include <QWidget>

class QComboBox;
class QDateEdit;
class QLineEdit;

namespace swirly {
namespace ui {

class Instr;
class InstrModel;

class MarketForm : public QWidget {
    Q_OBJECT

  public:
    MarketForm(InstrModel& instrModel, QWidget* parent = nullptr,
               Qt::WindowFlags f = Qt::WindowFlags{});
    ~MarketForm() noexcept override;

    void setFields(const QString& instrSymbol, QDate settlDate, std::optional<Lots> lots,
                   std::optional<Ticks> ticks);

  signals:
    void createMarket(const Instr& instr, QDate settlDate);
    void createOrder(const Instr& instr, QDate settlDate, const QString& ref, Side side, Lots lots,
                     Ticks ticks);

  private slots:
    void slotInstrChanged(int index);
    void slotCreateClicked();
    void slotBuyOrSellClicked(Side side);

  private:
    InstrModel& instrModel_;
    QComboBox* instrComboBox_{nullptr};
    QDateEdit* settlDateEdit_{nullptr};
    QLineEdit* lotsEdit_{nullptr};
    QLineEdit* priceEdit_{nullptr};
    QIntValidator lotsValidator_;
    QDoubleValidator priceValidator_;
};

} // ui
} // swirly

#endif // SWIRLYUI_MARKETFORM_HXX

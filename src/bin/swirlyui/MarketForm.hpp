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
#ifndef SWIRLYUI_MARKETFORM_HPP
#define SWIRLYUI_MARKETFORM_HPP

#include <QWidget>

namespace swirly {
namespace ui {

class MarketForm : public QWidget {
  Q_OBJECT

 public:
  MarketForm(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags{});
  ~MarketForm() noexcept override;

 signals:

 private slots:

 private:
};

} // ui
} // swirly

#endif // SWIRLYUI_MARKETFORM_HPP

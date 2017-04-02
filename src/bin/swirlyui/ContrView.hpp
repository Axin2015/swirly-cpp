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
#ifndef SWIRLYUI_CONTRVIEW_HPP
#define SWIRLYUI_CONTRVIEW_HPP

#include <QWidget>

class QModelIndex;
class QTableView;

namespace swirly {
namespace ui {

class Contr;
class ContrModel;

class ContrView : public QWidget {
    Q_OBJECT

  public:
    ContrView(ContrModel& model, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags{});
    ~ContrView() noexcept override;

    void resizeColumnsToContents();

  signals:

  private slots:
    void slotClicked(const QModelIndex& index);

  private:
    ContrModel& model_;
    QTableView* table_{nullptr};
};

} // ui
} // swirly

#endif // SWIRLYUI_CONTRVIEW_HPP

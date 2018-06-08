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
#ifndef SWIRLYUI_ASSETVIEW_HXX
#define SWIRLYUI_ASSETVIEW_HXX

#include <QWidget>

class QModelIndex;
class QTableView;

namespace swirly {
namespace ui {

class Asset;
class AssetModel;

class AssetView : public QWidget {
    Q_OBJECT

  public:
    AssetView(AssetModel& model, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags{});
    ~AssetView() override;

    void resize_columns_to_contents();

  signals:

  private slots:
    void slot_clicked(const QModelIndex& index);

  private:
    AssetModel& model_;
    QTableView* table_{nullptr};
};

} // namespace ui
} // namespace swirly

#endif // SWIRLYUI_ASSETVIEW_HXX

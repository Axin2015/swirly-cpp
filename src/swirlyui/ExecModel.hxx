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
#ifndef SWIRLYUI_EXECMODEL_HXX
#define SWIRLYUI_EXECMODEL_HXX

#include "Exec.hxx"
#include "Row.hxx"

#include <QAbstractTableModel>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <boost/circular_buffer.hpp>
#pragma GCC diagnostic pop

namespace swirly {
namespace ui {

class ExecModel : public QAbstractTableModel {
  public:
    ExecModel(QObject* parent = nullptr);
    ~ExecModel() override;

    int rowCount(const QModelIndex& parent) const override;

    int columnCount(const QModelIndex& parent) const override;

    QVariant data(const QModelIndex& index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    const Exec& value_at(int n) const { return row_at(n).value(); }

    void reset()
    {
        beginResetModel();
        rows_.clear();
        endResetModel();
    }
    void toggle_check_state(int n)
    {
        auto& row = row_at(n);
        row.set_checked(!row.checked());
        const auto i = index(n, unbox(exec::Column::CheckState));
        emit dataChanged(i, i);
    }

    void update_row(std::uint64_t tag, const Exec& exec);

  private:
    const Row<Exec>& row_at(int n) const { return rows_[n]; }
    Row<Exec>& row_at(int n) { return rows_[n]; }

    QVariant header_[exec::ColumnCount];
    boost::circular_buffer<Row<Exec>> rows_{MaxExecs};
};

} // namespace ui
} // namespace swirly

#endif // SWIRLYUI_EXECMODEL_HXX

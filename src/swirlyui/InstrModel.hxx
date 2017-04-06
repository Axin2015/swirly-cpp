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
#ifndef SWIRLYUI_INSTRMODEL_HXX
#define SWIRLYUI_INSTRMODEL_HXX

#include "Instr.hxx"
#include "TableModel.hxx"

namespace swirly {
namespace ui {

class InstrModel
    : public TableModel<QString, Instr, unbox(instr::Column::CheckState), instr::ColumnCount> {
  public:
    InstrModel(QObject* parent = nullptr);
    ~InstrModel() noexcept override;

    QVariant data(const QModelIndex& index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    Instr find(const QString& symbol) const;

    int indexOf(const QString& symbol) const;

    void removeRow(const Instr& instr) { TableModel::removeRow(instr.symbol()); }
    void updateRow(std::uint64_t tag, const Instr& instr)
    {
        TableModel::updateRow(instr.symbol(), tag, instr);
    }

  private:
    QVariant header_[instr::ColumnCount];
};

} // ui
} // swirly

#endif // SWIRLYUI_INSTRMODEL_HXX

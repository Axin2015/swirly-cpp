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
#ifndef SWIRLYUI_ASSET_HXX
#define SWIRLYUI_ASSET_HXX

#include "Types.hxx"

#include <QMetaType>
#include <QString>

class QJsonObject;

namespace swirly {
namespace ui {
namespace asset {

enum class Column : int { //
    CheckState,           //
    Symbol,               //
    Display,              //
    Type
};
constexpr int ColumnCount{unbox(Column::Type) + 1};

} // namespace asset

class Asset {
  public:
    Asset(const QString& symbol, const QString& display, AssetType type)
    : symbol_{symbol}
    , display_{display}
    , type_{type}
    {
    }
    Asset() = default;
    ~Asset() noexcept = default;

    static Asset fromJson(const QJsonObject& obj);

    const QString& symbol() const noexcept { return symbol_; }
    const QString& display() const noexcept { return display_; }
    AssetType type() const noexcept { return type_; }

  private:
    QString symbol_{};
    QString display_{};
    AssetType type_{};
};

QDebug operator<<(QDebug debug, const Asset& asset);

inline bool isModified(const Asset& prev, const Asset& next) noexcept
{
    // Immutable.
    return false;
}

} // namespace ui
} // namespace swirly

Q_DECLARE_METATYPE(swirly::ui::Asset)

#endif // SWIRLYUI_ASSET_HXX

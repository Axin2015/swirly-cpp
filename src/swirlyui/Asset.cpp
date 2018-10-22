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
#include "Asset.hxx"

#include <QJsonObject>

namespace swirly {
namespace ui {

Asset Asset::from_json(const QJsonObject& obj)
{
    using swirly::ui::from_json;
    return Asset{from_json<Id32>(obj["id"]), from_json<QString>(obj["symbol"]),
                 from_json<QString>(obj["display"]), from_json<AssetType>(obj["type"])};
}

QDebug operator<<(QDebug debug, const Asset& asset)
{
    debug.nospace() << "Asset{id=" << asset.id()      //
                    << ",symbol=" << asset.symbol()   //
                    << ",display=" << asset.display() //
                    << ",type=" << asset.type()       //
                    << '}';
    return debug;
}

} // namespace ui
} // namespace swirly

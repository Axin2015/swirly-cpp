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
#include "Level.hxx"

#include <QJsonObject>

namespace swirly {
namespace ui {

Level Level::fromJson(const QJsonObject& obj)
{
    using swirly::ui::fromJson;
    return Level{fromJson<Ticks>(obj["ticks"]), fromJson<Lots>(obj["resd"]),
                 fromJson<int>(obj["count"])};
}

QDebug operator<<(QDebug debug, const Level& level)
{
    debug.nospace() << "Level{ticks=" << level.ticks() //
                    << ",resd=" << level.resd() //
                    << ",count=" << level.count() //
                    << '}';
    return debug;
}

} // ui
} // swirly

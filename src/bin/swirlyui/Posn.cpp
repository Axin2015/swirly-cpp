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
#include "Posn.hpp"

#include "Json.hpp"

#include <QDebug>
#include <QJsonObject>

namespace swirly {
namespace ui {

Posn Posn::fromJson(const QJsonObject& obj)
{
  using swirly::ui::fromJson;
  return Posn{fromJson<QString>(obj["accnt"]),   fromJson<QString>(obj["contr"]),
              fromJson<QDate>(obj["settlDate"]), fromJson<Lots>(obj["buyLots"]),
              fromJson<Cost>(obj["buyCost"]),    fromJson<Lots>(obj["sellLots"]),
              fromJson<Cost>(obj["sellCost"])};
}

QDebug operator<<(QDebug debug, const Posn& posn)
{
  debug.nospace() << "Posn{accnt=" << posn.accnt() //
                  << ",contr=" << posn.contr() //
                  << ",settlDate=" << posn.settlDate() //
                  << ",buyLots=" << posn.buyLots() //
                  << ",buyCost=" << posn.buyCost() //
                  << ",sellLots=" << posn.sellLots() //
                  << ",sellCost=" << posn.sellCost() //
                  << '}';
  return debug;
}

} // ui
} // swirly

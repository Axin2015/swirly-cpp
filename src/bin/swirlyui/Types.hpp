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
#ifndef SWIRLYUI_TYPES_HPP
#define SWIRLYUI_TYPES_HPP

#include <QDebug>
#include <QtGlobal>

namespace swirly {
namespace ui {

using Id32 = qint32;
using Id64 = qint64;
using Incs = qint64;
using Lots = Incs;
using Ticks = Incs;
using Cost = Incs;
using MarketState = unsigned;

enum class AssetType { Commodity = 1, Corporate, Currency, Equity, Government, Index };

const char* enumString(AssetType type);

inline QDebug operator<<(QDebug debug, AssetType type)
{
  return debug.nospace() << enumString(type);
}

enum class Direct { Paid = 1, Given = -1 };

const char* enumString(Direct direct);

inline QDebug operator<<(QDebug debug, Direct direct)
{
  return debug.nospace() << enumString(direct);
}

enum class LiqInd { None = 0, Maker, Taker };

const char* enumString(LiqInd liqInd);

inline QDebug operator<<(QDebug debug, LiqInd liqInd)
{
  return debug.nospace() << enumString(liqInd);
}

enum class Side { Buy = 1, Sell = -1 };

const char* enumString(Side side);

inline QDebug operator<<(QDebug debug, Side side)
{
  return debug.nospace() << enumString(side);
}

enum class State { None = 0, New, Revise, Cancel, Trade };

const char* enumString(State state);

inline QDebug operator<<(QDebug debug, State state)
{
  return debug.nospace() << enumString(state);
}

} // ui
} // swirly

#endif // SWIRLYUI_TYPES_HPP

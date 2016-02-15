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
#include <swirly/elm/Types.hpp>

using namespace std;

namespace swirly {

const char* enumString(AssetType type)
{
  switch (type) {
  case AssetType::Commodity:
    return "COMMODITY";
  case AssetType::Corporate:
    return "CORPORATE";
  case AssetType::Currency:
    return "CURRENCY";
  case AssetType::Equity:
    return "EQUITY";
  case AssetType::Government:
    return "GOVERNMENT";
  case AssetType::Index:
    return "INDEX";
  }
  terminate();
}

const char* enumString(Direct direct)
{
  switch (direct) {
  case Direct::Paid:
    return "PAID";
  case Direct::Given:
    return "GIVEN";
  }
  terminate();
}

const char* enumString(RecType type)
{
  switch (type) {
  case RecType::Asset:
    return "ASSET";
  case RecType::Contr:
    return "CONTR";
  case RecType::Market:
    return "MARKET";
  case RecType::Trader:
    return "TRADER";
  }
  terminate();
}

const char* enumString(Role role)
{
  switch (role) {
  case Role::None:
    return "NONE";
  case Role::Maker:
    return "MAKER";
  case Role::Taker:
    return "TAKER";
  }
  terminate();
}

const char* enumString(Side side)
{
  switch (side) {
  case Side::Buy:
    return "BUY";
  case Side::Sell:
    return "SELL";
  }
  terminate();
}

const char* enumString(State state)
{
  switch (state) {
  case State::None:
    return "NONE";
  case State::New:
    return "NEW";
  case State::Revise:
    return "REVISE";
  case State::Cancel:
    return "CANCEL";
  case State::Trade:
    return "TRADE";
  }
  terminate();
}

} // swirly

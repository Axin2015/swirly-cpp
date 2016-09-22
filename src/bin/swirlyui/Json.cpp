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

#include "Json.hpp"

using namespace std;

namespace swirly {
namespace ui {

AssetType JsonTraits<AssetType>::fromJson(const QJsonValue& value)
{
  AssetType type;
  const auto s = value.toString();
  if (s == "COMMODITY") {
    type = AssetType::Commodity;
  } else if (s == "CORPORATE") {
    type = AssetType::Corporate;
  } else if (s == "CURRENCY") {
    type = AssetType::Currency;
  } else if (s == "EQUITY") {
    type = AssetType::Equity;
  } else if (s == "GOVERNMENT") {
    type = AssetType::Government;
  } else if (s == "INDEX") {
    type = AssetType::Index;
  } else {
    throw domain_error{"invalid asset-type"};
  }
  return type;
}

Direct JsonTraits<Direct>::fromJson(const QJsonValue& value)
{
  Direct direct;
  const auto s = value.toString();
  if (s == "PAID") {
    direct = Direct::Paid;
  } else if (s == "GIVEN") {
    direct = Direct::Given;
  } else {
    throw domain_error{"invalid direction"};
  }
  return direct;
}

LiqInd JsonTraits<LiqInd>::fromJson(const QJsonValue& value)
{
  LiqInd liqInd;
  if (value.isNull()) {
    liqInd = LiqInd::None;
  } else {
    const auto s = value.toString();
    if (s == "MAKER") {
      liqInd = LiqInd::Maker;
    } else if (s == "TAKER") {
      liqInd = LiqInd::Taker;
    } else {
      throw domain_error{"invalid liquidity-indicator"};
    }
  }
  return liqInd;
}

Side JsonTraits<Side>::fromJson(const QJsonValue& value)
{
  Side side;
  const auto s = value.toString();
  if (s == "BUY") {
    side = Side::Buy;
  } else if (s == "SELL") {
    side = Side::Sell;
  } else {
    throw domain_error{"invalid side"};
  }
  return side;
}

State JsonTraits<State>::fromJson(const QJsonValue& value)
{
  State state;
  const auto s = value.toString();
  if (s == "NONE") {
    state = State::None;
  } else if (s == "NEW") {
    state = State::New;
  } else if (s == "REVISE") {
    state = State::Revise;
  } else if (s == "CANCEL") {
    state = State::Cancel;
  } else if (s == "TRADE") {
    state = State::Trade;
  } else {
    throw domain_error{"invalid state"};
  }
  return state;
}

} // ui
} // swirly

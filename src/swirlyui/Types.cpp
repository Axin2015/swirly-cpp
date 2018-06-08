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
#include "Types.hxx"

namespace swirly {
namespace ui {
using namespace std;

AssetType TypeTraits<AssetType>::from_string(const QString& value)
{
    AssetType type;
    if (value == "Cmdty") {
        type = AssetType::Cmdty;
    } else if (value == "Corp") {
        type = AssetType::Corp;
    } else if (value == "Ccy") {
        type = AssetType::Ccy;
    } else if (value == "Eqty") {
        type = AssetType::Eqty;
    } else if (value == "Govt") {
        type = AssetType::Govt;
    } else if (value == "Index") {
        type = AssetType::Index;
    } else {
        throw domain_error{"invalid asset-type"};
    }
    return type;
}

Direct TypeTraits<Direct>::from_string(const QString& value)
{
    Direct direct;
    if (value == "Paid") {
        direct = Direct::Paid;
    } else if (value == "Given") {
        direct = Direct::Given;
    } else {
        throw domain_error{"invalid direction"};
    }
    return direct;
}

LiqInd TypeTraits<LiqInd>::from_string(const QString& value)
{
    LiqInd liq_ind;
    if (value.isNull()) {
        liq_ind = LiqInd::None;
    } else {
        if (value == "Maker") {
            liq_ind = LiqInd::Maker;
        } else if (value == "Taker") {
            liq_ind = LiqInd::Taker;
        } else {
            throw domain_error{"invalid liquidity-indicator"};
        }
    }
    return liq_ind;
}

Side TypeTraits<Side>::from_string(const QString& value)
{
    Side side;
    if (value == "Buy") {
        side = Side::Buy;
    } else if (value == "Sell") {
        side = Side::Sell;
    } else {
        throw domain_error{"invalid side"};
    }
    return side;
}

State TypeTraits<State>::from_string(const QString& value)
{
    State state;
    if (value == "None") {
        state = State::None;
    } else if (value == "New") {
        state = State::New;
    } else if (value == "Revise") {
        state = State::Revise;
    } else if (value == "Cancel") {
        state = State::Cancel;
    } else if (value == "Trade") {
        state = State::Trade;
    } else {
        throw domain_error{"invalid state"};
    }
    return state;
}

} // namespace ui
} // namespace swirly

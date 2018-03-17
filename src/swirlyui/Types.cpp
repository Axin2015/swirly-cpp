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

AssetType TypeTraits<AssetType>::fromString(const QString& value)
{
    AssetType type;
    if (value == "CMDTY") {
        type = AssetType::Cmdty;
    } else if (value == "CORP") {
        type = AssetType::Corp;
    } else if (value == "CCY") {
        type = AssetType::Ccy;
    } else if (value == "EQTY") {
        type = AssetType::Eqty;
    } else if (value == "GOVT") {
        type = AssetType::Govt;
    } else if (value == "INDEX") {
        type = AssetType::Index;
    } else {
        throw domain_error{"invalid asset-type"};
    }
    return type;
}

Direct TypeTraits<Direct>::fromString(const QString& value)
{
    Direct direct;
    if (value == "PAID") {
        direct = Direct::Paid;
    } else if (value == "GIVEN") {
        direct = Direct::Given;
    } else {
        throw domain_error{"invalid direction"};
    }
    return direct;
}

LiqInd TypeTraits<LiqInd>::fromString(const QString& value)
{
    LiqInd liqInd;
    if (value.isNull()) {
        liqInd = LiqInd::None;
    } else {
        if (value == "MAKER") {
            liqInd = LiqInd::Maker;
        } else if (value == "TAKER") {
            liqInd = LiqInd::Taker;
        } else {
            throw domain_error{"invalid liquidity-indicator"};
        }
    }
    return liqInd;
}

Side TypeTraits<Side>::fromString(const QString& value)
{
    Side side;
    if (value == "BUY") {
        side = Side::Buy;
    } else if (value == "SELL") {
        side = Side::Sell;
    } else {
        throw domain_error{"invalid side"};
    }
    return side;
}

State TypeTraits<State>::fromString(const QString& value)
{
    State state;
    if (value == "NONE") {
        state = State::None;
    } else if (value == "NEW") {
        state = State::New;
    } else if (value == "REVISE") {
        state = State::Revise;
    } else if (value == "CANCEL") {
        state = State::Cancel;
    } else if (value == "TRADE") {
        state = State::Trade;
    } else {
        throw domain_error{"invalid state"};
    }
    return state;
}

} // namespace ui
} // namespace swirly

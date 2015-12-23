/*
 * Swirly Order-Book and Matching-Engine.
 * Copyright (C) 2013, 2015 Swirly Cloud Limited.
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

const char* enumToString(AssetType type)
{
    switch (type) {
    SWIRLY_ENUM_CASE(AssetType, COMMODITY);
    SWIRLY_ENUM_CASE(AssetType, CORPORATE);
    SWIRLY_ENUM_CASE(AssetType, CURRENCY);
    SWIRLY_ENUM_CASE(AssetType, EQUITY);
    SWIRLY_ENUM_CASE(AssetType, GOVERNMENT);
    SWIRLY_ENUM_CASE(AssetType, INDEX);
    }
    terminate();
}

const char* enumToString(Direct direct)
{
    switch (direct) {
    SWIRLY_ENUM_CASE(Direct, PAID);
    SWIRLY_ENUM_CASE(Direct, GIVEN);
    }
    terminate();
}

const char* enumToString(RecType type)
{
    switch (type) {
    SWIRLY_ENUM_CASE(RecType, ASSET);
    SWIRLY_ENUM_CASE(RecType, CONTR);
    SWIRLY_ENUM_CASE(RecType, MARKET);
    SWIRLY_ENUM_CASE(RecType, TRADER);
    }
    terminate();
}

const char* enumToString(Role role)
{
    switch (role) {
    SWIRLY_ENUM_CASE(Role, MAKER);
    SWIRLY_ENUM_CASE(Role, TAKER);
    }
    terminate();
}

const char* enumToString(Side side)
{
    switch (side) {
    SWIRLY_ENUM_CASE(Side, BUY);
    SWIRLY_ENUM_CASE(Side, SELL);
    }
    terminate();
}

const char* enumToString(State state)
{
    switch (state) {
    SWIRLY_ENUM_CASE(State, NONE);
    SWIRLY_ENUM_CASE(State, NEW);
    SWIRLY_ENUM_CASE(State, REVISE);
    SWIRLY_ENUM_CASE(State, CANCEL);
    SWIRLY_ENUM_CASE(State, TRADE);
    SWIRLY_ENUM_CASE(State, PECAN);
    }
    terminate();
}

} // swirly

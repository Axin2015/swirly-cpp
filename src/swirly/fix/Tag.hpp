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
#ifndef SWIRLY_FIX_TAG_HPP
#define SWIRLY_FIX_TAG_HPP

namespace swirly {
inline namespace fix {

enum class Tag : int {
    AvgPx = 6,
    BeginString = 8,
    BodyLength = 9,
    CumQty = 14,
    EncryptMethod = 98,
    ExecId = 17,
    ExecType = 150,
    HeartBtInt = 108,
    LastPx = 31,
    LastQty = 32,
    LeavesQty = 151,
    MaturityDate = 541,
    MdEntryPx = 270,
    MdEntrySize = 271,
    MdEntryType = 269,
    MinQty = 110,
    MsgSeqNum = 34,
    MsgType = 35,
    NoMdEntries = 268,
    OrdStatus = 39,
    OrderId = 37,
    OrderQty = 38,
    PossDupFlag = 43,
    PossResend = 97,
    Price = 44,
    SenderCompId = 49,
    SendingTime = 52,
    Side = 54,
    Symbol = 55,
    TargetCompId = 56,
    TradSesStatus = 340,
    TradingSessionId = 336
};

template <Tag TagN>
struct TagType {
};

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_TAG_HPP

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
#ifndef SWIRLY_FIN_MSGHANDLER_HPP
#define SWIRLY_FIN_MSGHANDLER_HPP

#include <swirly/fin/Msg.hpp>

namespace swirly {
inline namespace fin {

template <typename DerivedT>
struct BasicMsgHandler {

    BasicMsgHandler() noexcept = default;

    // Copy.
    BasicMsgHandler(const BasicMsgHandler&) noexcept = default;
    BasicMsgHandler& operator=(const BasicMsgHandler&) noexcept = default;

    // Move.
    BasicMsgHandler(BasicMsgHandler&&) noexcept = default;
    BasicMsgHandler& operator=(BasicMsgHandler&&) noexcept = default;

    void dispatch(const Msg& msg)
    {
        auto* const derived = static_cast<DerivedT*>(this);
        switch (msg.type) {
        case MsgType::CreateMarket:
            derived->on_create_market(to_time<WallClock>(Nanos{msg.time}), msg.create_market);
            break;
        case MsgType::UpdateMarket:
            derived->on_update_market(to_time<WallClock>(Nanos{msg.time}), msg.update_market);
            break;
        case MsgType::CreateExec:
            derived->on_create_exec(to_time<WallClock>(Nanos{msg.time}), msg.create_exec);
            break;
        case MsgType::ArchiveTrade:
            derived->on_archive_trade(to_time<WallClock>(Nanos{msg.time}), msg.archive_trade);
            break;
        }
    }

  protected:
    ~BasicMsgHandler() = default;
};

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_MSGHANDLER_HPP

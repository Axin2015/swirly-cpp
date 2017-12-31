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
        case MsgType::Reset:
            derived->onReset();
            break;
        case MsgType::CreateMarket:
            derived->onCreateMarket(msg.createMarket);
            break;
        case MsgType::UpdateMarket:
            derived->onUpdateMarket(msg.updateMarket);
            break;
        case MsgType::CreateExec:
            derived->onCreateExec(msg.createExec);
            break;
        case MsgType::ArchiveTrade:
            derived->onArchiveTrade(msg.archiveTrade);
            break;
        }
    }

  protected:
    ~BasicMsgHandler() noexcept = default;
};

} // namespace swirly

#endif // SWIRLY_FIN_MSGHANDLER_HPP

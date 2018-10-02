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
#ifndef SWIRLYD_FIXHANDLER_HXX
#define SWIRLYD_FIXHANDLER_HXX

#include <swirly/fix/Handler.hpp>

namespace swirly {

class FixHandler;
using FixHandlerPtr = std::unique_ptr<FixHandler>;
using FixHandlerMap = std::unordered_map<FixSessId, FixHandlerPtr>;

class FixHandler : public FixHandlerBase {
  public:
    FixHandler() noexcept = default;
    ~FixHandler() override;

    // Copy.
    FixHandler(const FixHandler&) noexcept;
    FixHandler& operator=(const FixHandler&) noexcept;

    // Move.
    FixHandler(FixHandler&&) noexcept;
    FixHandler& operator=(FixHandler&&) noexcept;

    void prepare(CyclTime now, const FixSessId& sess_id, const FixHandlerMap& handler_map)
    {
        do_prepare(now, sess_id, handler_map);
    }
    void send(CyclTime now, std::string_view msg_type, std::string_view msg)
    {
        do_send(now, msg_type, msg);
    }

  protected:
    virtual void do_prepare(CyclTime now, const FixSessId& sess_id,
                            const FixHandlerMap& handler_map)
        = 0;
    virtual void do_send(CyclTime now, std::string_view msg_type, std::string_view msg) = 0;
};

} // namespace swirly

#endif // SWIRLYD_FIXHANDLER_HXX

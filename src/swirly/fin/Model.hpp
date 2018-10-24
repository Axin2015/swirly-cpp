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
#ifndef SWIRLY_FIN_MODEL_HPP
#define SWIRLY_FIN_MODEL_HPP

#include <swirly/fin/Types.hpp>

#include <swirly/util/Date.hpp>
#include <swirly/util/Time.hpp>

namespace swirly {
inline namespace util {
class Config;
} // namespace util

inline namespace fin {

template <typename ValueT>
using ModelCallback = std::function<void(ValueT)>;

class SWIRLY_API Model {
  public:
    Model() noexcept = default;
    virtual ~Model();

    // Copy.
    constexpr Model(const Model&) noexcept = default;
    Model& operator=(const Model&) noexcept = default;

    // Move.
    constexpr Model(Model&&) noexcept = default;
    Model& operator=(Model&&) noexcept = default;

    void read_asset(const ModelCallback<AssetPtr>& cb) const { do_read_asset(cb); }
    void read_instr(const ModelCallback<InstrPtr>& cb) const { do_read_instr(cb); }
    void read_market(const ModelCallback<MarketPtr>& cb) const { do_read_market(cb); }
    void read_order(const ModelCallback<OrderPtr>& cb) const { do_read_order(cb); }
    void read_exec(WallTime since, const ModelCallback<ExecPtr>& cb) const
    {
        do_read_exec(since, cb);
    }
    void read_trade(const ModelCallback<ExecPtr>& cb) const { do_read_trade(cb); }
    void read_posn(JDay bus_day, const ModelCallback<PosnPtr>& cb) const
    {
        do_read_posn(bus_day, cb);
    }

  protected:
    virtual void do_read_asset(const ModelCallback<AssetPtr>& cb) const = 0;

    virtual void do_read_instr(const ModelCallback<InstrPtr>& cb) const = 0;

    virtual void do_read_market(const ModelCallback<MarketPtr>& cb) const = 0;

    virtual void do_read_order(const ModelCallback<OrderPtr>& cb) const = 0;

    virtual void do_read_exec(WallTime since, const ModelCallback<ExecPtr>& cb) const = 0;

    virtual void do_read_trade(const ModelCallback<ExecPtr>& cb) const = 0;

    virtual void do_read_posn(JDay bus_day, const ModelCallback<PosnPtr>& cb) const = 0;
};

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_MODEL_HPP

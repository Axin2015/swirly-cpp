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
#ifndef SWIRLY_FIN_EXEC_HPP
#define SWIRLY_FIN_EXEC_HPP

#include <swirly/fin/Conv.hpp>
#include <swirly/fin/Request.hpp>

#include <swirly/app/MemAlloc.hpp>

namespace swirly {
inline namespace fin {

/**
 * A transaction that occurs as an @ref Order transitions through a workflow.
 *
 * Trade executions represent the exchange of goods or services between counter-parties.
 */
class SWIRLY_API Exec
: public RefCount<Exec, ThreadUnsafePolicy>
, public Request
, public MemAlloc {
  public:
    Exec(Time created, Symbol accnt, Id64 market_id, Symbol instr, JDay settl_day, Id64 id,
         Id64 order_id, std::string_view ref, State state, Side side, Lots lots, Ticks ticks,
         Lots resd_lots, Lots exec_lots, Cost exec_cost, Lots last_lots, Ticks last_ticks,
         Lots min_lots, Id64 match_id, Lots posn_lots, Cost posn_cost, LiqInd liq_ind,
         Symbol cpty) noexcept
    : Request{created, accnt, market_id, instr, settl_day, id, ref, side, lots}
    , order_id_{order_id}
    , state_{state}
    , ticks_{ticks}
    , resd_lots_{resd_lots}
    , exec_lots_{exec_lots}
    , exec_cost_{exec_cost}
    , last_lots_{last_lots}
    , last_ticks_{last_ticks}
    , min_lots_{min_lots}
    , match_id_{match_id}
    , posn_lots_{posn_lots}
    , posn_cost_{posn_cost}
    , liq_ind_{liq_ind}
    , cpty_{cpty}
    {
    }
    ~Exec();

    // Copy.
    Exec(const Exec&) = delete;
    Exec& operator=(const Exec&) = delete;

    // Move.
    Exec(Exec&&);
    Exec& operator=(Exec&&) = delete;

    template <typename... ArgsT>
    static ExecPtr make(ArgsT&&... args)
    {
        return make_intrusive<Exec>(std::forward<ArgsT>(args)...);
    }
    ExecPtr opposite(Id64 id) const;

    void to_dsv(std::ostream& os, char delim = ',') const;
    void to_json(std::ostream& os) const;

    auto order_id() const noexcept { return order_id_; }
    auto state() const noexcept { return state_; }
    auto ticks() const noexcept { return ticks_; }
    auto resd_lots() const noexcept { return resd_lots_; }
    auto exec_lots() const noexcept { return exec_lots_; }
    auto exec_cost() const noexcept { return exec_cost_; }
    auto last_lots() const noexcept { return last_lots_; }
    auto last_ticks() const noexcept { return last_ticks_; }
    auto min_lots() const noexcept { return min_lots_; }
    auto match_id() const noexcept { return match_id_; }
    auto posn_lots() const noexcept { return posn_lots_; }
    auto posn_cost() const noexcept { return posn_cost_; }
    auto liq_ind() const noexcept { return liq_ind_; }
    auto cpty() const noexcept { return cpty_; }

    void revise(Lots lots) noexcept
    {
        state_ = State::Revise;
        const auto delta = lots_ - lots;
        assert(delta >= 0_lts);
        lots_ = lots;
        resd_lots_ -= delta;
    }
    void cancel() noexcept
    {
        state_ = State::Cancel;
        resd_lots_ = 0_lts;
    }
    void posn(Lots posn_lots, Cost posn_cost) noexcept
    {
        posn_lots_ = posn_lots;
        posn_cost_ = posn_cost;
    }
    void trade(Lots sum_lots, Cost sum_cost, Lots last_lots, Ticks last_ticks, Id64 match_id,
               LiqInd liq_ind, Symbol cpty) noexcept;

    void trade(Lots last_lots, Ticks last_ticks, Id64 match_id, LiqInd liq_ind,
               Symbol cpty) noexcept
    {
        trade(last_lots, swirly::cost(last_lots, last_ticks), last_lots, last_ticks, match_id,
              liq_ind, cpty);
    }

    boost::intrusive::set_member_hook<> id_hook;

  private:
    const Id64 order_id_;
    State state_;
    const Ticks ticks_;
    /**
     * Must be greater than zero.
     */
    Lots resd_lots_;
    /**
     * Must not be greater that lots.
     */
    Lots exec_lots_;
    Cost exec_cost_;
    Lots last_lots_;
    Ticks last_ticks_;
    /**
     * Minimum to be filled by this order.
     */
    const Lots min_lots_;
    Id64 match_id_;
    Lots posn_lots_;
    Cost posn_cost_;
    LiqInd liq_ind_;
    Symbol cpty_;
};

static_assert(sizeof(Exec) <= 5 * 64, "no greater than specified cache-lines");

inline std::ostream& operator<<(std::ostream& os, const Exec& exec)
{
    exec.to_json(os);
    return os;
}

using ExecIdSet = RequestIdSet<Exec>;

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_EXEC_HPP

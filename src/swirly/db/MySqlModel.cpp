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
#include "MySqlModel.hpp"

#include "MySql.hxx"
#include "Sql.hxx"

#include <swirly/fin/Asset.hpp>
#include <swirly/fin/Exec.hpp>
#include <swirly/fin/Instr.hpp>
#include <swirly/fin/Market.hpp>

#include <swirly/fin/Order.hpp>
#include <swirly/fin/Posn.hpp>

namespace swirly {
inline namespace db {
using namespace mysql;
using namespace std;

MySqlModel::MySqlModel(const DbPtr& db)
: db_{db}
{
}

MySqlModel::MySqlModel(const Config& config)
: MySqlModel{DbPtr{nullptr}}
{
}

MySqlModel::~MySqlModel() = default;

MySqlModel::MySqlModel(MySqlModel&&) = default;
MySqlModel& MySqlModel::operator=(MySqlModel&&) = default;

void MySqlModel::do_read_asset(const ModelCallback<AssetPtr>& cb) const
{
    using namespace asset;

    auto stmt = prepare(*db_, SelectSql);
    execute(*stmt);
    auto res = result_metadata(*stmt);

    BindArray<4> result;
    field::Id32 id{result[Id]};
    field::Symbol symbol{result[Symbol]};
    field::Display display{result[Display]};
    field::Id32 type_id{result[TypeId]};

    bind_result(*stmt, &result[0]);
    while (fetch(*stmt)) {
        cb(Asset::make(Id32{id.value()}, //
                       symbol.value(),   //
                       display.value(),  //
                       AssetType{type_id.value()}));
    }
}

void MySqlModel::do_read_instr(const ModelCallback<InstrPtr>& cb) const
{
    using namespace instr;

    auto stmt = prepare(*db_, SelectSql);
    execute(*stmt);
    auto res = result_metadata(*stmt);

    BindArray<12> result;
    field::Id32 id{result[Id]};
    field::Symbol symbol{result[Symbol]};
    field::Display display{result[Display]};
    field::Symbol base_asset{result[BaseAsset]};
    field::Symbol term_ccy{result[TermCcy]};
    field::Numer lot_numer{result[LotNumer]};
    field::Denom lot_denom{result[LotDenom]};
    field::Numer tick_numer{result[TickNumer]};
    field::Denom tick_denom{result[TickDenom]};
    field::PipDp pip_dp{result[PipDp]};
    field::Lots min_lots{result[MinLots]};
    field::Lots max_lots{result[MaxLots]};

    bind_result(*stmt, &result[0]);
    while (fetch(*stmt)) {
        cb(Instr::make(Id32{id.value()},       //
                       symbol.value(),         //
                       display.value(),        //
                       base_asset.value(),     //
                       term_ccy.value(),       //
                       lot_numer.value(),      //
                       lot_denom.value(),      //
                       tick_numer.value(),     //
                       tick_denom.value(),     //
                       pip_dp.value(),         //
                       Lots{min_lots.value()}, //
                       Lots{max_lots.value()}));
    }
}

void MySqlModel::do_read_market(const ModelCallback<MarketPtr>& cb) const
{
    using namespace market;

    auto stmt = prepare(*db_, SelectSql);
    execute(*stmt);
    auto res = result_metadata(*stmt);

    BindArray<8> result;
    field::Id64 id{result[Id]};
    field::Symbol instr{result[Instr]};
    field::JDay settl_day{result[SettlDay]};
    field::MarketState state{result[State]};
    field::Time last_time{result[LastTime]};
    field::Lots last_lots{result[LastLots]};
    field::Ticks last_ticks{result[LastTicks]};
    field::Id64 max_id{result[MaxId]};

    bind_result(*stmt, &result[0]);
    while (fetch(*stmt)) {
        cb(Market::make(Id64{id.value()},                             //
                        instr.value(),                                //
                        JDay{settl_day.value()},                      //
                        static_cast<MarketState>(state.value()),      //
                        to_time<WallClock>(Nanos{last_time.value()}), //
                        Lots{last_lots.value()},                      //
                        Ticks{last_ticks.value()},                    //
                        Id64{max_id.value()}));
    }
}

void MySqlModel::do_read_order(const ModelCallback<OrderPtr>& cb) const
{
    using namespace order;

    auto stmt = prepare(*db_, SelectSql);
    execute(*stmt);
    auto res = result_metadata(*stmt);

    BindArray<18> result;
    field::Time created{result[Created]};
    field::Time modified{result[Modified]};
    field::Symbol accnt{result[Accnt]};
    field::Id64 market_id{result[MarketId]};
    field::Symbol instr{result[Instr]};
    field::JDay settl_day{result[SettlDay]};
    field::Id64 id{result[Id]};
    field::Ref ref{result[Ref]};
    field::State state{result[State]};
    field::Side side{result[Side]};
    field::Lots lots{result[Lots]};
    field::Ticks ticks{result[Ticks]};
    field::Lots resd_lots{result[ResdLots]};
    field::Lots exec_lots{result[ExecLots]};
    field::Cost exec_cost{result[ExecCost]};
    field::Lots last_lots{result[LastLots]};
    field::Ticks last_ticks{result[LastTicks]};
    field::Lots min_lots{result[MinLots]};

    bind_result(*stmt, &result[0]);
    while (fetch(*stmt)) {
        cb(Order::make(to_time<WallClock>(Nanos{created.value()}),  //
                       to_time<WallClock>(Nanos{modified.value()}), //
                       accnt.value(),                               //
                       Id64{market_id.value()},                     //
                       instr.value(),                               //
                       JDay{settl_day.value()},                     //
                       Id64{id.value()},                            //
                       ref.value(),                                 //
                       swirly::State{state.value()},                //
                       swirly::Side{side.value()},                  //
                       swirly::Lots{lots.value()},                  //
                       swirly::Ticks{ticks.value()},                //
                       swirly::Lots{resd_lots.value()},             //
                       swirly::Lots{exec_lots.value()},             //
                       swirly::Cost{exec_cost.value()},             //
                       swirly::Lots{last_lots.value()},             //
                       swirly::Ticks{last_ticks.value()},           //
                       swirly::Lots{min_lots.value()}));
    }
}

void MySqlModel::do_read_exec(WallTime since, const ModelCallback<ExecPtr>& cb) const
{
    using namespace exec;

    auto stmt = prepare(*db_, SelectSql);

    BindArray<1> param;
    auto pit = param.begin();
    field::Time created_param{*pit++, ns_since_epoch(since)};
    bind_param(*stmt, &param[0]);

    execute(*stmt);
    auto res = result_metadata(*stmt);

    BindArray<23> result;
    field::Time created{result[Created]};
    field::Symbol accnt{result[Accnt]};
    field::Id64 market_id{result[MarketId]};
    field::Symbol instr{result[Instr]};
    field::JDay settl_day{result[SettlDay]};
    field::Id64 id{result[Id]};
    field::Id64 order_id{result[OrderId]};
    field::Ref ref{result[Ref]};
    field::State state{result[State]};
    field::Side side{result[Side]};
    field::Lots lots{result[Lots]};
    field::Ticks ticks{result[Ticks]};
    field::Lots resd_lots{result[ResdLots]};
    field::Lots exec_lots{result[ExecLots]};
    field::Cost exec_cost{result[ExecCost]};
    field::Lots last_lots{result[LastLots]};
    field::Ticks last_ticks{result[LastTicks]};
    field::Lots min_lots{result[MinLots]};
    field::Id64 match_id{result[MatchId]};
    field::Lots posn_lots{result[PosnLots]};
    field::Cost posn_cost{result[PosnCost]};
    field::LiqInd liq_ind{result[LiqInd]};
    field::Symbol cpty{result[Cpty]};

    bind_result(*stmt, &result[0]);
    while (fetch(*stmt)) {
        cb(Exec::make(to_time<WallClock>(Nanos{created.value()}), //
                      accnt.value(),                              //
                      Id64{market_id.value()},                    //
                      instr.value(),                              //
                      JDay{settl_day.value()},                    //
                      Id64{id.value()},                           //
                      Id64{order_id.value()},                     //
                      ref.value(),                                //
                      swirly::State{state.value()},               //
                      swirly::Side{side.value()},                 //
                      swirly::Lots{lots.value()},                 //
                      swirly::Ticks{ticks.value()},               //
                      swirly::Lots{resd_lots.value()},            //
                      swirly::Lots{exec_lots.value()},            //
                      swirly::Cost{exec_cost.value()},            //
                      swirly::Lots{last_lots.value()},            //
                      swirly::Ticks{last_ticks.value()},          //
                      swirly::Lots{min_lots.value()},             //
                      Id64{match_id.value()},                     //
                      swirly::Lots{posn_lots.value()},            //
                      swirly::Cost{posn_cost.value()},            //
                      swirly::LiqInd{liq_ind.value()},            //
                      cpty.value()));
    }
}

void MySqlModel::do_read_trade(const ModelCallback<ExecPtr>& cb) const
{
    using namespace trade;

    auto stmt = prepare(*db_, SelectSql);
    execute(*stmt);
    auto res = result_metadata(*stmt);

    BindArray<22> result;
    field::Time created{result[Created]};
    field::Symbol accnt{result[Accnt]};
    field::Id64 market_id{result[MarketId]};
    field::Symbol instr{result[Instr]};
    field::JDay settl_day{result[SettlDay]};
    field::Id64 id{result[Id]};
    field::Id64 order_id{result[OrderId]};
    field::Ref ref{result[Ref]};
    field::Side side{result[Side]};
    field::Lots lots{result[Lots]};
    field::Ticks ticks{result[Ticks]};
    field::Lots resd_lots{result[ResdLots]};
    field::Lots exec_lots{result[ExecLots]};
    field::Cost exec_cost{result[ExecCost]};
    field::Lots last_lots{result[LastLots]};
    field::Ticks last_ticks{result[LastTicks]};
    field::Lots min_lots{result[MinLots]};
    field::Id64 match_id{result[MatchId]};
    field::Lots posn_lots{result[PosnLots]};
    field::Cost posn_cost{result[PosnCost]};
    field::LiqInd liq_ind{result[LiqInd]};
    field::Symbol cpty{result[Cpty]};

    bind_result(*stmt, &result[0]);
    while (fetch(*stmt)) {
        cb(Exec::make(to_time<WallClock>(Nanos{created.value()}), //
                      accnt.value(),                              //
                      Id64{market_id.value()},                    //
                      instr.value(),                              //
                      JDay{settl_day.value()},                    //
                      Id64{id.value()},                           //
                      Id64{order_id.value()},                     //
                      ref.value(),                                //
                      State::Trade,                               //
                      swirly::Side{side.value()},                 //
                      swirly::Lots{lots.value()},                 //
                      swirly::Ticks{ticks.value()},               //
                      swirly::Lots{resd_lots.value()},            //
                      swirly::Lots{exec_lots.value()},            //
                      swirly::Cost{exec_cost.value()},            //
                      swirly::Lots{last_lots.value()},            //
                      swirly::Ticks{last_ticks.value()},          //
                      swirly::Lots{min_lots.value()},             //
                      Id64{match_id.value()},                     //
                      swirly::Lots{posn_lots.value()},            //
                      swirly::Cost{posn_cost.value()},            //
                      swirly::LiqInd{liq_ind.value()},            //
                      cpty.value()));
    }
}

void MySqlModel::do_read_posn(JDay bus_day, const ModelCallback<PosnPtr>& cb) const
{
    using namespace posn;

    PosnSet ps;
    PosnSet::Iterator it;

    auto stmt = prepare(*db_, SelectSql);
    execute(*stmt);
    auto res = result_metadata(*stmt);

    BindArray<7> result;
    field::Symbol accnt_field{result[Accnt]};
    field::Id64 market_id_field{result[MarketId]};
    field::Symbol instr_field{result[Instr]};
    field::JDay settl_day_field{result[SettlDay]};
    field::Side side_field{result[Side]};
    field::Lots lots_field{result[Lots]};
    field::Cost cost_field{result[Cost]};

    bind_result(*stmt, &result[0]);
    while (fetch(*stmt)) {
        const auto accnt = accnt_field.value();
        auto market_id = Id64{market_id_field.value()};
        const auto instr = instr_field.value();
        auto settl_day = JDay{settl_day_field.value()};

        // FIXME: review when end of day is implemented.
        if (settl_day != 0_jd && settl_day <= bus_day) {
            market_id &= Id64{~0xffff};
            settl_day = 0_jd;
        }

        bool found;
        tie(it, found) = ps.find_hint(accnt, market_id);
        if (!found) {
            it = ps.insert_hint(it, Posn::make(accnt, market_id, instr, settl_day));
        }

        const auto side = swirly::Side{side_field.value()};
        const auto lots = swirly::Lots{lots_field.value()};
        const auto cost = swirly::Cost{cost_field.value()};
        if (side == swirly::Side::Buy) {
            it->add_buy(lots, cost);
        } else {
            it->add_sell(lots, cost);
        }
    }

    for (it = ps.begin(); it != ps.end();) {
        cb(ps.remove(it++));
    }
}

} // namespace db
} // namespace swirly

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
#include "DsvModel.hpp"

#include "Asset.hpp"
#include "Exec.hpp"
#include "Instr.hpp"
#include "Market.hpp"
#include "Posn.hpp"

#include <swirly/util/Tokeniser.hpp>

#include <fstream>

namespace swirly {
using namespace std;

DsvModel::DsvModel() noexcept = default;

DsvModel::~DsvModel() noexcept = default;

void DsvModel::doReadAsset(const ModelCallback<AssetPtr>& cb) const
{
    enum {
        Id,      //
        Symbol,  //
        Display, //
        TypeId   //
    };

    ifstream is{"asset.txt"};
    string line;

    // Discard header.
    if (!getline(is, line)) {
        return;
    }

    while (getline(is, line)) {
        Row<TypeId + 1> row;
        split(line, "\t"sv, row);
        const auto id = fromString<Id32>(row[Id]);
        const auto symbol = fromString<string_view>(row[Symbol]);
        const auto display = fromString<string_view>(row[Display]);
        const auto type = fromString<AssetType>(row[TypeId]);
        cb(Asset::make(id, symbol, display, type));
    }
}

void DsvModel::doReadInstr(const ModelCallback<InstrPtr>& cb) const
{
    enum {
        Id,        //
        Symbol,    //
        Display,   //
        BaseAsset, //
        TermCcy,   //
        TickNumer, //
        TickDenom, //
        LotNumer,  //
        LotDenom,  //
        PipDp,     //
        MinLots,   //
        MaxLots    //
    };

    ifstream is{"instr.txt"};
    string line;

    // Discard header.
    if (!getline(is, line)) {
        return;
    }

    while (getline(is, line)) {
        Row<MaxLots + 1> row;
        split(line, "\t"sv, row);
        const auto id = fromString<Id32>(row[Id]);
        const auto symbol = fromString<string_view>(row[Symbol]);
        const auto display = fromString<string_view>(row[Display]);
        const auto baseAsset = fromString<swirly::Symbol>(row[BaseAsset]);
        const auto termCcy = fromString<swirly::Symbol>(row[TermCcy]);
        const auto lotNumer = fromString<int>(row[LotNumer]);
        const auto lotDenom = fromString<int>(row[LotDenom]);
        const auto tickNumer = fromString<int>(row[TickNumer]);
        const auto tickDenom = fromString<int>(row[TickDenom]);
        const auto pipDp = fromString<int>(row[PipDp]);
        const auto minLots = fromString<Lots>(row[MinLots]);
        const auto maxLots = fromString<Lots>(row[MaxLots]);
        cb(Instr::make(id, symbol, display, baseAsset, termCcy, lotNumer, lotDenom, tickNumer,
                       tickDenom, pipDp, minLots, maxLots));
    }
}

void DsvModel::doReadMarket(const ModelCallback<MarketPtr>& cb) const
{
    enum {         //
        Id,        //
        Instr,     //
        SettlDay,  //
        State,     //
        LastLots,  //
        LastTicks, //
        LastTime,  //
        MaxId      //
    };

    ifstream is{"market.txt"};
    string line;

    // Discard header.
    if (!getline(is, line)) {
        return;
    }

    while (getline(is, line)) {
        Row<MaxId + 1> row;
        split(line, "\t"sv, row);
        const auto id = fromString<Id64>(row[Id]);
        const auto instr = fromString<swirly::Symbol>(row[Instr]);
        const auto settlDay = fromString<JDay>(row[SettlDay]);
        const auto state = fromString<MarketState>(row[State]);
        const auto lastLots = fromString<Lots>(row[LastLots]);
        const auto lastTicks = fromString<Ticks>(row[LastTicks]);
        const auto lastTime = fromString<Time>(row[LastTime]);
        const auto maxId = fromString<Id64>(row[MaxId]);
        cb(Market::make(id, instr, settlDay, state, lastLots, lastTicks, lastTime, maxId));
    }
}

void DsvModel::doReadOrder(const ModelCallback<OrderPtr>& cb) const
{
    enum {         //
        Accnt,     //
        MarketId,  //
        Instr,     //
        SettlDay,  //
        Id,        //
        Ref,       //
        State,     //
        Side,      //
        Lots,      //
        Ticks,     //
        ResdLots,  //
        ExecLots,  //
        ExecCost,  //
        LastLots,  //
        LastTicks, //
        MinLots,   //
        Created,   //
        Modified   //
    };

    ifstream is{"order.txt"};
    string line;

    // Discard header.
    if (!getline(is, line)) {
        return;
    }

    while (getline(is, line)) {
        Row<Modified + 1> row;
        split(line, "\t"sv, row);
        const auto accnt = fromString<Symbol>(row[Accnt]);
        const auto marketId = fromString<Id64>(row[MarketId]);
        const auto instr = fromString<Symbol>(row[Instr]);
        const auto settlDay = fromString<JDay>(row[SettlDay]);
        const auto id = fromString<Id64>(row[Id]);
        const auto ref = fromString<string_view>(row[Ref]);
        const auto state = fromString<swirly::State>(row[State]);
        const auto side = fromString<swirly::Side>(row[Side]);
        const auto lots = fromString<swirly::Lots>(row[Lots]);
        const auto ticks = fromString<swirly::Ticks>(row[Ticks]);
        const auto resdLots = fromString<swirly::Lots>(row[ResdLots]);
        const auto execLots = fromString<swirly::Lots>(row[ExecLots]);
        const auto execCost = fromString<Cost>(row[ExecCost]);
        const auto lastLots = fromString<swirly::Lots>(row[LastLots]);
        const auto lastTicks = fromString<swirly::Ticks>(row[LastTicks]);
        const auto minLots = fromString<swirly::Lots>(row[MinLots]);
        const auto created = fromString<Time>(row[Created]);
        const auto modified = fromString<Time>(row[Modified]);
        cb(Order::make(accnt, marketId, instr, settlDay, id, ref, state, side, lots, ticks,
                       resdLots, execLots, execCost, lastLots, lastTicks, minLots, created,
                       modified));
    }
}

void DsvModel::doReadExec(Time since, const ModelCallback<ExecPtr>& cb) const
{
    enum {         //
        Accnt,     //
        MarketId,  //
        Instr,     //
        SettlDay,  //
        Id,        //
        OrderId,   //
        Ref,       //
        State,     //
        Side,      //
        Lots,      //
        Ticks,     //
        ResdLots,  //
        ExecLots,  //
        ExecCost,  //
        LastLots,  //
        LastTicks, //
        MinLots,   //
        MatchId,   //
        LiqInd,    //
        Cpty,      //
        Created    //
    };

    ifstream is{"exec.txt"};
    string line;

    // Discard header.
    if (!getline(is, line)) {
        return;
    }

    while (getline(is, line)) {
        Row<Created + 1> row;
        split(line, "\t"sv, row);
        const auto accnt = fromString<swirly::Symbol>(row[Accnt]);
        const auto marketId = fromString<Id64>(row[MarketId]);
        const auto instr = fromString<Symbol>(row[Instr]);
        const auto settlDay = fromString<JDay>(row[SettlDay]);
        const auto id = fromString<Id64>(row[Id]);
        const auto orderId = fromString<Id64>(row[OrderId]);
        const auto ref = fromString<string_view>(row[Ref]);
        const auto state = fromString<swirly::State>(row[State]);
        const auto side = fromString<swirly::Side>(row[Side]);
        const auto lots = fromString<swirly::Lots>(row[Lots]);
        const auto ticks = fromString<swirly::Ticks>(row[Ticks]);
        const auto resdLots = fromString<swirly::Lots>(row[ResdLots]);
        const auto execLots = fromString<swirly::Lots>(row[ExecLots]);
        const auto execCost = fromString<Cost>(row[ExecCost]);
        const auto lastLots = fromString<swirly::Lots>(row[LastLots]);
        const auto lastTicks = fromString<swirly::Ticks>(row[LastTicks]);
        const auto minLots = fromString<swirly::Lots>(row[MinLots]);
        const auto matchId = fromString<Id64>(row[MatchId]);
        const auto liqInd = fromString<swirly::LiqInd>(row[LiqInd]);
        const auto cpty = fromString<Symbol>(row[Cpty]);
        const auto created = fromString<Time>(row[Created]);
        cb(Exec::make(accnt, marketId, instr, settlDay, id, orderId, ref, state, side, lots, ticks,
                      resdLots, execLots, execCost, lastLots, lastTicks, minLots, matchId, liqInd,
                      cpty, created));
    }
}

void DsvModel::doReadTrade(const ModelCallback<ExecPtr>& cb) const
{
    enum {         //
        Accnt,     //
        MarketId,  //
        Instr,     //
        SettlDay,  //
        Id,        //
        OrderId,   //
        Ref,       //
        Side,      //
        Lots,      //
        Ticks,     //
        ResdLots,  //
        ExecLots,  //
        ExecCost,  //
        LastLots,  //
        LastTicks, //
        MinLots,   //
        MatchId,   //
        LiqInd,    //
        Cpty,      //
        Created    //
    };

    ifstream is{"trade.txt"};
    string line;

    // Discard header.
    if (!getline(is, line)) {
        return;
    }

    while (getline(is, line)) {
        Row<Created + 1> row;
        split(line, "\t"sv, row);
        const auto accnt = fromString<swirly::Symbol>(row[Accnt]);
        const auto marketId = fromString<Id64>(row[MarketId]);
        const auto instr = fromString<Symbol>(row[Instr]);
        const auto settlDay = fromString<JDay>(row[SettlDay]);
        const auto id = fromString<Id64>(row[Id]);
        const auto orderId = fromString<Id64>(row[OrderId]);
        const auto ref = fromString<string_view>(row[Ref]);
        const auto side = fromString<swirly::Side>(row[Side]);
        const auto lots = fromString<swirly::Lots>(row[Lots]);
        const auto ticks = fromString<swirly::Ticks>(row[Ticks]);
        const auto resdLots = fromString<swirly::Lots>(row[ResdLots]);
        const auto execLots = fromString<swirly::Lots>(row[ExecLots]);
        const auto execCost = fromString<Cost>(row[ExecCost]);
        const auto lastLots = fromString<swirly::Lots>(row[LastLots]);
        const auto lastTicks = fromString<swirly::Ticks>(row[LastTicks]);
        const auto minLots = fromString<swirly::Lots>(row[MinLots]);
        const auto matchId = fromString<Id64>(row[MatchId]);
        const auto liqInd = fromString<swirly::LiqInd>(row[LiqInd]);
        const auto cpty = fromString<Symbol>(row[Cpty]);
        const auto created = fromString<Time>(row[Created]);
        cb(Exec::make(accnt, marketId, instr, settlDay, id, orderId, ref, State::Trade, side, lots,
                      ticks, resdLots, execLots, execCost, lastLots, lastTicks, minLots, matchId,
                      liqInd, cpty, created));
    }
}

void DsvModel::doReadPosn(JDay busDay, const ModelCallback<PosnPtr>& cb) const
{
    enum {        //
        Accnt,    //
        MarketId, //
        Instr,    //
        SettlDay, //
        BuyLots,  //
        BuyCost,  //
        SellLots, //
        SellCost  //
    };

    ifstream is{"posn.txt"};
    string line;

    // Discard header.
    if (!getline(is, line)) {
        return;
    }

    while (getline(is, line)) {
        Row<SellCost + 1> row;
        split(line, "\t"sv, row);
        const auto accnt = fromString<Symbol>(row[Accnt]);
        const auto marketId = fromString<Id64>(row[MarketId]);
        const auto instr = fromString<Symbol>(row[Instr]);
        const auto settlDay = fromString<JDay>(row[SettlDay]);
        const auto buyLots = fromString<Lots>(row[BuyLots]);
        const auto buyCost = fromString<Cost>(row[BuyCost]);
        const auto sellLots = fromString<Lots>(row[SellLots]);
        const auto sellCost = fromString<Cost>(row[SellCost]);
        cb(Posn::make(accnt, marketId, instr, settlDay, buyLots, buyCost, sellLots, sellCost));
    }
}

} // namespace swirly

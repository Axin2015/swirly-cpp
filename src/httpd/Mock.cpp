/*
 * Swirly Order-Book and Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
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
#include "Mock.hpp"

#include <swirly/elm/Factory.hpp>

namespace swirly {

MockModel::MockModel() noexcept = default;
MockModel::~MockModel() noexcept = default;

constexpr MockModel::MockModel(const MockModel&) noexcept = default;
MockModel& MockModel::operator=(const MockModel&) noexcept = default;

constexpr MockModel::MockModel(MockModel&&) noexcept = default;
MockModel& MockModel::operator=(MockModel&&) noexcept = default;

AssetSet MockModel::doReadAsset(const Factory& factory) const
{
    AssetSet s;
    s.insert(factory.newAsset("EUR", "Euro Dollar", AssetType::CURRENCY));
    s.insert(factory.newAsset("GBP", "Sterling", AssetType::CURRENCY));
    s.insert(factory.newAsset("USD", "US Dollar", AssetType::CURRENCY));
    s.insert(factory.newAsset("USD", "US Dollar", AssetType::CURRENCY));
    return s;
}

ContrSet MockModel::doReadContr(const Factory& factory) const
{
    ContrSet s;
    return s;
}

MarketSet MockModel::doReadMarket(const Factory& factory) const
{
    MarketSet s;
    return s;
}

TraderSet MockModel::doReadTrader(const Factory& factory) const
{
    TraderSet s;
    return s;
}

MockJourn::MockJourn() noexcept = default;
MockJourn::~MockJourn() noexcept = default;

constexpr MockJourn::MockJourn(const MockJourn&) noexcept = default;
MockJourn& MockJourn::operator=(const MockJourn&) noexcept = default;

constexpr MockJourn::MockJourn(MockJourn&&) noexcept = default;
MockJourn& MockJourn::operator=(MockJourn&&) noexcept = default;

void MockJourn::doCreateMarket(const StringView& mnem, const StringView& display,
                               const StringView& contr, Jday settlDay, Jday expiryDay,
                               MarketState state)
{
}

void MockJourn::doUpdateMarket(const StringView& mnem, const StringView& display, MarketState state)
{
}

void MockJourn::doCreateTrader(const StringView& mnem, const StringView& display,
                               const StringView& email)
{
}

void MockJourn::doUpdateTrader(const StringView& mnem, const StringView& display)
{
}

void MockJourn::doCreateExec(const Exec& exec)
{
}

void MockJourn::doCreateExec(const StringView& market, const ArrayView<Exec*>& execs)
{
}

void MockJourn::doCreateExec(const ArrayView<Exec*>& execs)
{
}

void MockJourn::doArchiveOrder(const StringView& market, Iden id, Millis modified)
{
}

void MockJourn::doArchiveOrder(const StringView& market, const ArrayView<Iden>& ids,
                               Millis modified)
{
}

void MockJourn::doArchiveOrder(const ArrayView<MarketId>& ids, Millis modified)
{
}

void MockJourn::doArchiveTrade(const StringView& market, Iden id, Millis modified)
{
}

void MockJourn::doArchiveTrade(const StringView& market, const ArrayView<Iden>& ids,
                               Millis modified)
{
}

void MockJourn::doArchiveTrade(const ArrayView<MarketId>& ids, Millis modified)
{
}

} // swirly

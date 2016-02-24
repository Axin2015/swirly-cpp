/*
 * The Restful Matching-Engine.
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

using namespace std;

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
  // Forex.
  s.insert(factory.newAsset("CHF", "Switzerland, Francs", AssetType::Currency));
  s.insert(factory.newAsset("EUR", "Euro Member Countries, Euro", AssetType::Currency));
  s.insert(factory.newAsset("GBP", "United Kingdom, Pounds", AssetType::Currency));
  s.insert(factory.newAsset("JPY", "Japan, Yen", AssetType::Currency));
  s.insert(factory.newAsset("USD", "United States of America, Dollars", AssetType::Currency));
  s.insert(factory.newAsset("ZAR", "South Africa, Rand", AssetType::Currency));
  // Coal.
  s.insert(factory.newAsset("CAP", "Central Appalachia Coal", AssetType::Commodity));
  s.insert(factory.newAsset("NAP", "Northern Appalachia Coal", AssetType::Commodity));
  s.insert(factory.newAsset("ILB", "Illinois Basin Coal", AssetType::Commodity));
  s.insert(factory.newAsset("PRB", "Powder River Basin Coal", AssetType::Commodity));
  s.insert(factory.newAsset("UIB", "Uinta Basin Coal", AssetType::Commodity));
  // Coffee.
  s.insert(factory.newAsset("ETB", "Ethiopia, Birr", AssetType::Currency));
  s.insert(factory.newAsset("WYCA", "Yirgachefe A", AssetType::Commodity));
  s.insert(factory.newAsset("WWNA", "Wenago A", AssetType::Commodity));
  s.insert(factory.newAsset("WKCA", "Kochere A", AssetType::Commodity));
  s.insert(factory.newAsset("WGAA", "Gelena Abaya A", AssetType::Commodity));
  // US Corporates.
  s.insert(factory.newAsset("CSCO", "Cisco Systems Inc", AssetType::Corporate));
  s.insert(factory.newAsset("DIS", "Walt Disney", AssetType::Corporate));
  s.insert(factory.newAsset("IBM", "Ibm Corp", AssetType::Corporate));
  s.insert(factory.newAsset("INTC", "Intel Corp", AssetType::Corporate));
  s.insert(factory.newAsset("MSFT", "Microsoft Corp", AssetType::Corporate));
  s.insert(factory.newAsset("VIA", "Viacom Inc", AssetType::Corporate));
  s.insert(factory.newAsset("VOD", "Vodafone Group Plc", AssetType::Corporate));
  s.insert(factory.newAsset("VZ", "Verizon Com", AssetType::Corporate));
  return s;
}

ContrSet MockModel::doReadContr(const Factory& factory) const
{
  ContrSet s;
  // Forex.
  s.insert(
    factory.newContr("EURUSD", "EURUSD", "EUR", "USD", 1000000, 1, 1, 10000, 4, 1_lts, 10_lts));
  s.insert(
    factory.newContr("GBPUSD", "GBPUSD", "GBP", "USD", 1000000, 1, 1, 10000, 4, 1_lts, 10_lts));
  s.insert(
    factory.newContr("USDCHF", "USDCHF", "USD", "CHF", 1000000, 1, 1, 10000, 4, 1_lts, 10_lts));
  s.insert(
    factory.newContr("USDJPY", "USDJPY", "USD", "JPY", 1000000, 1, 1, 100, 2, 1_lts, 10_lts));
  // Coal.
  s.insert(factory.newContr("CAP", "Central Appalachia Coal", "CAP", "USD", 1000, 1, 1, 20, 2,
                            1_lts, 10_lts));
  s.insert(factory.newContr("NAP", "Northern Appalachia Coal", "NAP", "USD", 1000, 1, 1, 20, 2,
                            1_lts, 10_lts));
  s.insert(
    factory.newContr("ILB", "Illinois Basin Coal", "ILB", "USD", 1000, 1, 1, 20, 2, 1_lts, 10_lts));
  s.insert(factory.newContr("PRB", "Powder River Basin Coal", "PRB", "USD", 1000, 1, 1, 20, 2,
                            1_lts, 10_lts));
  s.insert(
    factory.newContr("UIB", "Uinta Basin Coal", "UIB", "USD", 1000, 1, 1, 20, 2, 1_lts, 10_lts));
  // Coffee.
  s.insert(factory.newContr("WYCA", "Yirgachefe A", "WYCA", "ETB", 1, 1, 1, 1, 0, 1_lts, 10_lts));
  s.insert(factory.newContr("WWNA", "Wenago A", "WWNA", "ETB", 1, 1, 1, 1, 0, 1_lts, 10_lts));
  s.insert(factory.newContr("WKCA", "Kochere A", "WKCA", "ETB", 1, 1, 1, 1, 0, 1_lts, 10_lts));
  s.insert(factory.newContr("WGAA", "Gelena Abaya A", "WGAA", "ETB", 1, 1, 1, 1, 0, 1_lts, 10_lts));
  // US Corporates.
  s.insert(
    factory.newContr("CSCO", "Cisco Systems Inc", "CSCO", "USD", 1, 1, 1, 1000, 3, 1_lts, 10_lts));
  s.insert(factory.newContr("DIS", "Walt Disney", "DIS", "USD", 1, 1, 1, 1000, 3, 1_lts, 10_lts));
  s.insert(factory.newContr("IBM", "Ibm Corp", "IBM", "USD", 1, 1, 1, 1000, 3, 1_lts, 10_lts));
  s.insert(factory.newContr("INTC", "Intel Corp", "INTC", "USD", 1, 1, 1, 1000, 3, 1_lts, 10_lts));
  s.insert(
    factory.newContr("MSFT", "Microsoft Corp", "MSFT", "USD", 1, 1, 1, 1000, 3, 1_lts, 10_lts));
  s.insert(factory.newContr("VIA", "Viacom Inc", "VIA", "USD", 1, 1, 1, 1000, 3, 1_lts, 10_lts));
  s.insert(
    factory.newContr("VOD", "Vodafone Group Plc", "VOD", "USD", 1, 1, 1, 1000, 3, 1_lts, 10_lts));
  s.insert(factory.newContr("VZ", "Verizon Com", "VZ", "USD", 1, 1, 1, 1000, 3, 1_lts, 10_lts));
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
  s.insert(factory.newTrader("MARAYL", "Mark Aylett", "mark.aylett@gmail.com"));
  s.insert(factory.newTrader("SWIRLY", "Swirly Cloud", "info@swirlycloud.com"));
  return s;
}

MockJourn::MockJourn() noexcept = default;
MockJourn::~MockJourn() noexcept = default;

constexpr MockJourn::MockJourn(const MockJourn&) noexcept = default;
MockJourn& MockJourn::operator=(const MockJourn&) noexcept = default;

constexpr MockJourn::MockJourn(MockJourn&&) noexcept = default;
MockJourn& MockJourn::operator=(MockJourn&&) noexcept = default;

void MockJourn::doCreateMarket(const string_view& mnem, const string_view& display,
                               const string_view& contr, Jday settlDay, Jday expiryDay,
                               MarketState state)
{
}

void MockJourn::doUpdateMarket(const string_view& mnem, const string_view& display,
                               MarketState state)
{
}

void MockJourn::doCreateTrader(const string_view& mnem, const string_view& display,
                               const string_view& email)
{
}

void MockJourn::doUpdateTrader(const string_view& mnem, const string_view& display)
{
}

void MockJourn::doCreateExec(const Exec& exec)
{
}

void MockJourn::doCreateExec(const string_view& market, const ArrayView<Exec*>& execs)
{
}

void MockJourn::doCreateExec(const ArrayView<Exec*>& execs)
{
}

void MockJourn::doArchiveOrder(const string_view& market, Iden id, Millis modified)
{
}

void MockJourn::doArchiveOrder(const string_view& market, const ArrayView<Iden>& ids,
                               Millis modified)
{
}

void MockJourn::doArchiveOrder(const ArrayView<MarketId>& ids, Millis modified)
{
}

void MockJourn::doArchiveTrade(const string_view& market, Iden id, Millis modified)
{
}

void MockJourn::doArchiveTrade(const string_view& market, const ArrayView<Iden>& ids,
                               Millis modified)
{
}

void MockJourn::doArchiveTrade(const ArrayView<MarketId>& ids, Millis modified)
{
}

} // swirly

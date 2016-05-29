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
#include <swirly/fig/Mock.hpp>

#include <swirly/elm/Asset.hpp>
#include <swirly/elm/Contr.hpp>
#include <swirly/elm/Factory.hpp>
#include <swirly/elm/Market.hpp>
#include <swirly/elm/Trader.hpp>

using namespace std;

namespace swirly {

MockModel::MockModel() noexcept = default;
MockModel::~MockModel() noexcept = default;

constexpr MockModel::MockModel(const MockModel&) noexcept = default;
MockModel& MockModel::operator=(const MockModel&) noexcept = default;

constexpr MockModel::MockModel(MockModel&&) noexcept = default;
MockModel& MockModel::operator=(MockModel&&) noexcept = default;

void MockModel::doReadAsset(const Factory& factory, const ModelCallback<AssetPtr>& cb) const
{
  // Forex.
  cb(factory.newAsset("CHF"_sv, "Switzerland, Francs"_sv, AssetType::Currency));
  cb(factory.newAsset("EUR"_sv, "Euro Member Countries, Euro"_sv, AssetType::Currency));
  cb(factory.newAsset("GBP"_sv, "United Kingdom, Pounds"_sv, AssetType::Currency));
  cb(factory.newAsset("JPY"_sv, "Japan, Yen"_sv, AssetType::Currency));
  cb(factory.newAsset("USD"_sv, "United States of America, Dollars"_sv, AssetType::Currency));
  cb(factory.newAsset("ZAR"_sv, "South Africa, Rand"_sv, AssetType::Currency));
  // Coal.
  cb(factory.newAsset("CAP"_sv, "Central Appalachia Coal"_sv, AssetType::Commodity));
  cb(factory.newAsset("NAP"_sv, "Northern Appalachia Coal"_sv, AssetType::Commodity));
  cb(factory.newAsset("ILB"_sv, "Illinois Basin Coal"_sv, AssetType::Commodity));
  cb(factory.newAsset("PRB"_sv, "Powder River Basin Coal"_sv, AssetType::Commodity));
  cb(factory.newAsset("UIB"_sv, "Uinta Basin Coal"_sv, AssetType::Commodity));
  // Coffee.
  cb(factory.newAsset("ETB"_sv, "Ethiopia, Birr"_sv, AssetType::Currency));
  cb(factory.newAsset("WYCA"_sv, "Yirgachefe A"_sv, AssetType::Commodity));
  cb(factory.newAsset("WWNA"_sv, "Wenago A"_sv, AssetType::Commodity));
  cb(factory.newAsset("WKCA"_sv, "Kochere A"_sv, AssetType::Commodity));
  cb(factory.newAsset("WGAA"_sv, "Gelena Abaya A"_sv, AssetType::Commodity));
  // US Corporates.
  cb(factory.newAsset("CSCO"_sv, "Cisco Systems Inc"_sv, AssetType::Corporate));
  cb(factory.newAsset("DIS"_sv, "Walt Disney"_sv, AssetType::Corporate));
  cb(factory.newAsset("IBM"_sv, "Ibm Corp"_sv, AssetType::Corporate));
  cb(factory.newAsset("INTC"_sv, "Intel Corp"_sv, AssetType::Corporate));
  cb(factory.newAsset("MSFT"_sv, "Microsoft Corp"_sv, AssetType::Corporate));
  cb(factory.newAsset("VIA"_sv, "Viacom Inc"_sv, AssetType::Corporate));
  cb(factory.newAsset("VOD"_sv, "Vodafone Group Plc"_sv, AssetType::Corporate));
  cb(factory.newAsset("VZ"_sv, "Verizon Com"_sv, AssetType::Corporate));
}

void MockModel::doReadContr(const Factory& factory, const ModelCallback<ContrPtr>& cb) const
{
  // Forex.
  cb(factory.newContr("EURUSD"_sv, "EURUSD"_sv, "EUR"_sv, "USD"_sv, 1000000, 1, 1, 10000, 4, 1_lts,
                      10_lts));
  cb(factory.newContr("GBPUSD"_sv, "GBPUSD"_sv, "GBP"_sv, "USD"_sv, 1000000, 1, 1, 10000, 4, 1_lts,
                      10_lts));
  cb(factory.newContr("USDCHF"_sv, "USDCHF"_sv, "USD"_sv, "CHF"_sv, 1000000, 1, 1, 10000, 4, 1_lts,
                      10_lts));
  cb(factory.newContr("USDJPY"_sv, "USDJPY"_sv, "USD"_sv, "JPY"_sv, 1000000, 1, 1, 100, 2, 1_lts,
                      10_lts));
  // Coal.
  cb(factory.newContr("CAP"_sv, "Central Appalachia Coal"_sv, "CAP"_sv, "USD"_sv, 1000, 1, 1, 20, 2,
                      1_lts, 10_lts));
  cb(factory.newContr("NAP"_sv, "Northern Appalachia Coal"_sv, "NAP"_sv, "USD"_sv, 1000, 1, 1, 20,
                      2, 1_lts, 10_lts));
  cb(factory.newContr("ILB"_sv, "Illinois Basin Coal"_sv, "ILB"_sv, "USD"_sv, 1000, 1, 1, 20, 2,
                      1_lts, 10_lts));
  cb(factory.newContr("PRB"_sv, "Powder River Basin Coal"_sv, "PRB"_sv, "USD"_sv, 1000, 1, 1, 20, 2,
                      1_lts, 10_lts));
  cb(factory.newContr("UIB"_sv, "Uinta Basin Coal"_sv, "UIB"_sv, "USD"_sv, 1000, 1, 1, 20, 2, 1_lts,
                      10_lts));
  // Coffee.
  cb(factory.newContr("WYCA"_sv, "Yirgachefe A"_sv, "WYCA"_sv, "ETB"_sv, 1, 1, 1, 1, 0, 1_lts,
                      10_lts));
  cb(factory.newContr("WWNA"_sv, "Wenago A"_sv, "WWNA"_sv, "ETB"_sv, 1, 1, 1, 1, 0, 1_lts, 10_lts));
  cb(
    factory.newContr("WKCA"_sv, "Kochere A"_sv, "WKCA"_sv, "ETB"_sv, 1, 1, 1, 1, 0, 1_lts, 10_lts));
  cb(factory.newContr("WGAA"_sv, "Gelena Abaya A"_sv, "WGAA"_sv, "ETB"_sv, 1, 1, 1, 1, 0, 1_lts,
                      10_lts));
  // US Corporates.
  cb(factory.newContr("CSCO"_sv, "Cisco Systems Inc"_sv, "CSCO"_sv, "USD"_sv, 1, 1, 1, 1000, 3,
                      1_lts, 10_lts));
  cb(factory.newContr("DIS"_sv, "Walt Disney"_sv, "DIS"_sv, "USD"_sv, 1, 1, 1, 1000, 3, 1_lts,
                      10_lts));
  cb(
    factory.newContr("IBM"_sv, "Ibm Corp"_sv, "IBM"_sv, "USD"_sv, 1, 1, 1, 1000, 3, 1_lts, 10_lts));
  cb(factory.newContr("INTC"_sv, "Intel Corp"_sv, "INTC"_sv, "USD"_sv, 1, 1, 1, 1000, 3, 1_lts,
                      10_lts));
  cb(factory.newContr("MSFT"_sv, "Microsoft Corp"_sv, "MSFT"_sv, "USD"_sv, 1, 1, 1, 1000, 3, 1_lts,
                      10_lts));
  cb(factory.newContr("VIA"_sv, "Viacom Inc"_sv, "VIA"_sv, "USD"_sv, 1, 1, 1, 1000, 3, 1_lts,
                      10_lts));
  cb(factory.newContr("VOD"_sv, "Vodafone Group Plc"_sv, "VOD"_sv, "USD"_sv, 1, 1, 1, 1000, 3,
                      1_lts, 10_lts));
  cb(factory.newContr("VZ"_sv, "Verizon Com"_sv, "VZ"_sv, "USD"_sv, 1, 1, 1, 1000, 3, 1_lts,
                      10_lts));
}

void MockModel::doReadMarket(const Factory& factory, const ModelCallback<MarketPtr>& cb) const
{
}

void MockModel::doReadTrader(const Factory& factory, const ModelCallback<TraderPtr>& cb) const
{
  cb(factory.newTrader("EDDAYL"_sv, "Eddie Aylett"_sv, "eddie.aylett@swirlycloud.com"_sv));
  cb(factory.newTrader("GOSAYL"_sv, "Goska Aylett"_sv, "goska.aylett@swirlycloud.com"_sv));
  cb(factory.newTrader("MARAYL"_sv, "Mark Aylett"_sv, "mark.aylett@swirlycloud.com"_sv));
}

void MockModel::doReadOrder(const Factory& factory, const ModelCallback<OrderPtr>& cb) const
{
}

void MockModel::doReadTrade(const Factory& factory, const ModelCallback<ExecPtr>& cb) const
{
}

void MockModel::doReadPosn(Jday busDay, const Factory& factory,
                           const ModelCallback<PosnPtr>& cb) const
{
}

MockJourn::MockJourn() noexcept = default;
MockJourn::~MockJourn() noexcept = default;

constexpr MockJourn::MockJourn(const MockJourn&) noexcept = default;
MockJourn& MockJourn::operator=(const MockJourn&) noexcept = default;

constexpr MockJourn::MockJourn(MockJourn&&) noexcept = default;
MockJourn& MockJourn::operator=(MockJourn&&) noexcept = default;

void MockJourn::doCreateMarket(Mnem mnem, string_view display, Mnem contr, Jday settlDay,
                               Jday expiryDay, MarketState state)
{
}

void MockJourn::doUpdateMarket(Mnem mnem, string_view display, MarketState state)
{
}

void MockJourn::doCreateTrader(Mnem mnem, string_view display, string_view email)
{
}

void MockJourn::doUpdateTrader(Mnem mnem, string_view display)
{
}

void MockJourn::doCreateExec(const Exec& exec)
{
}

void MockJourn::doCreateExec(Mnem market, ArrayView<ConstExecPtr> execs)
{
}

void MockJourn::doCreateExec(ArrayView<ConstExecPtr> execs)
{
}

void MockJourn::doArchiveOrder(Mnem market, Iden id, Millis modified)
{
}

void MockJourn::doArchiveOrder(Mnem market, ArrayView<Iden> ids, Millis modified)
{
}

void MockJourn::doArchiveTrade(Mnem market, Iden id, Millis modified)
{
}

void MockJourn::doArchiveTrade(Mnem market, ArrayView<Iden> ids, Millis modified)
{
}

} // swirly

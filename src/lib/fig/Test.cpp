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
#include <swirly/fig/Test.hpp>

#include <swirly/elm/Asset.hpp>
#include <swirly/elm/Contr.hpp>
#include <swirly/elm/Factory.hpp>
#include <swirly/elm/Market.hpp>
#include <swirly/elm/Trader.hpp>

using namespace std;

namespace swirly {

TestModel::TestModel() noexcept = default;
TestModel::~TestModel() noexcept = default;

constexpr TestModel::TestModel(const TestModel&) noexcept = default;
TestModel& TestModel::operator=(const TestModel&) noexcept = default;

constexpr TestModel::TestModel(TestModel&&) noexcept = default;
TestModel& TestModel::operator=(TestModel&&) noexcept = default;

void TestModel::doReadAsset(const ModelCallback<AssetPtr>& cb) const
{
  // Forex.
  cb(Asset::make("CHF"_sv, "Switzerland, Francs"_sv, AssetType::Currency));
  cb(Asset::make("EUR"_sv, "Euro Member Countries, Euro"_sv, AssetType::Currency));
  cb(Asset::make("GBP"_sv, "United Kingdom, Pounds"_sv, AssetType::Currency));
  cb(Asset::make("JPY"_sv, "Japan, Yen"_sv, AssetType::Currency));
  cb(Asset::make("USD"_sv, "United States of America, Dollars"_sv, AssetType::Currency));
  cb(Asset::make("ZAR"_sv, "South Africa, Rand"_sv, AssetType::Currency));
  // Coal.
  cb(Asset::make("CAP"_sv, "Central Appalachia Coal"_sv, AssetType::Commodity));
  cb(Asset::make("NAP"_sv, "Northern Appalachia Coal"_sv, AssetType::Commodity));
  cb(Asset::make("ILB"_sv, "Illinois Basin Coal"_sv, AssetType::Commodity));
  cb(Asset::make("PRB"_sv, "Powder River Basin Coal"_sv, AssetType::Commodity));
  cb(Asset::make("UIB"_sv, "Uinta Basin Coal"_sv, AssetType::Commodity));
  // Coffee.
  cb(Asset::make("ETB"_sv, "Ethiopia, Birr"_sv, AssetType::Currency));
  cb(Asset::make("WYCA"_sv, "Yirgachefe A"_sv, AssetType::Commodity));
  cb(Asset::make("WWNA"_sv, "Wenago A"_sv, AssetType::Commodity));
  cb(Asset::make("WKCA"_sv, "Kochere A"_sv, AssetType::Commodity));
  cb(Asset::make("WGAA"_sv, "Gelena Abaya A"_sv, AssetType::Commodity));
  // US Corporates.
  cb(Asset::make("CSCO"_sv, "Cisco Systems Inc"_sv, AssetType::Corporate));
  cb(Asset::make("DIS"_sv, "Walt Disney"_sv, AssetType::Corporate));
  cb(Asset::make("IBM"_sv, "Ibm Corp"_sv, AssetType::Corporate));
  cb(Asset::make("INTC"_sv, "Intel Corp"_sv, AssetType::Corporate));
  cb(Asset::make("MSFT"_sv, "Microsoft Corp"_sv, AssetType::Corporate));
  cb(Asset::make("VIA"_sv, "Viacom Inc"_sv, AssetType::Corporate));
  cb(Asset::make("VOD"_sv, "Vodafone Group Plc"_sv, AssetType::Corporate));
  cb(Asset::make("VZ"_sv, "Verizon Com"_sv, AssetType::Corporate));
}

void TestModel::doReadContr(const ModelCallback<ContrPtr>& cb) const
{
  // Forex.
  cb(Contr::make("EURUSD"_sv, "EURUSD"_sv, "EUR"_sv, "USD"_sv, 1000000, 1, 1, 10000, 4, 1_lts,
                 10_lts));
  cb(Contr::make("GBPUSD"_sv, "GBPUSD"_sv, "GBP"_sv, "USD"_sv, 1000000, 1, 1, 10000, 4, 1_lts,
                 10_lts));
  cb(Contr::make("USDCHF"_sv, "USDCHF"_sv, "USD"_sv, "CHF"_sv, 1000000, 1, 1, 10000, 4, 1_lts,
                 10_lts));
  cb(Contr::make("USDJPY"_sv, "USDJPY"_sv, "USD"_sv, "JPY"_sv, 1000000, 1, 1, 100, 2, 1_lts,
                 10_lts));
  // Coal.
  cb(Contr::make("CAP"_sv, "Central Appalachia Coal"_sv, "CAP"_sv, "USD"_sv, 1000, 1, 1, 20, 2,
                 1_lts, 10_lts));
  cb(Contr::make("NAP"_sv, "Northern Appalachia Coal"_sv, "NAP"_sv, "USD"_sv, 1000, 1, 1, 20, 2,
                 1_lts, 10_lts));
  cb(Contr::make("ILB"_sv, "Illinois Basin Coal"_sv, "ILB"_sv, "USD"_sv, 1000, 1, 1, 20, 2, 1_lts,
                 10_lts));
  cb(Contr::make("PRB"_sv, "Powder River Basin Coal"_sv, "PRB"_sv, "USD"_sv, 1000, 1, 1, 20, 2,
                 1_lts, 10_lts));
  cb(Contr::make("UIB"_sv, "Uinta Basin Coal"_sv, "UIB"_sv, "USD"_sv, 1000, 1, 1, 20, 2, 1_lts,
                 10_lts));
  // Coffee.
  cb(Contr::make("WYCA"_sv, "Yirgachefe A"_sv, "WYCA"_sv, "ETB"_sv, 1, 1, 1, 1, 0, 1_lts, 10_lts));
  cb(Contr::make("WWNA"_sv, "Wenago A"_sv, "WWNA"_sv, "ETB"_sv, 1, 1, 1, 1, 0, 1_lts, 10_lts));
  cb(Contr::make("WKCA"_sv, "Kochere A"_sv, "WKCA"_sv, "ETB"_sv, 1, 1, 1, 1, 0, 1_lts, 10_lts));
  cb(
    Contr::make("WGAA"_sv, "Gelena Abaya A"_sv, "WGAA"_sv, "ETB"_sv, 1, 1, 1, 1, 0, 1_lts, 10_lts));
  // US Corporates.
  cb(Contr::make("CSCO"_sv, "Cisco Systems Inc"_sv, "CSCO"_sv, "USD"_sv, 1, 1, 1, 1000, 3, 1_lts,
                 10_lts));
  cb(Contr::make("DIS"_sv, "Walt Disney"_sv, "DIS"_sv, "USD"_sv, 1, 1, 1, 1000, 3, 1_lts, 10_lts));
  cb(Contr::make("IBM"_sv, "Ibm Corp"_sv, "IBM"_sv, "USD"_sv, 1, 1, 1, 1000, 3, 1_lts, 10_lts));
  cb(Contr::make("INTC"_sv, "Intel Corp"_sv, "INTC"_sv, "USD"_sv, 1, 1, 1, 1000, 3, 1_lts, 10_lts));
  cb(Contr::make("MSFT"_sv, "Microsoft Corp"_sv, "MSFT"_sv, "USD"_sv, 1, 1, 1, 1000, 3, 1_lts,
                 10_lts));
  cb(Contr::make("VIA"_sv, "Viacom Inc"_sv, "VIA"_sv, "USD"_sv, 1, 1, 1, 1000, 3, 1_lts, 10_lts));
  cb(Contr::make("VOD"_sv, "Vodafone Group Plc"_sv, "VOD"_sv, "USD"_sv, 1, 1, 1, 1000, 3, 1_lts,
                 10_lts));
  cb(Contr::make("VZ"_sv, "Verizon Com"_sv, "VZ"_sv, "USD"_sv, 1, 1, 1, 1000, 3, 1_lts, 10_lts));
}

void TestModel::doReadMarket(const Factory& factory, const ModelCallback<MarketPtr>& cb) const
{
}

void TestModel::doReadTrader(const Factory& factory, const ModelCallback<TraderPtr>& cb) const
{
  cb(factory.newTrader("EDDAYL"_sv, "Eddie Aylett"_sv, "eddie.aylett@swirlycloud.com"_sv));
  cb(factory.newTrader("GOSAYL"_sv, "Goska Aylett"_sv, "goska.aylett@swirlycloud.com"_sv));
  cb(factory.newTrader("MARAYL"_sv, "Mark Aylett"_sv, "mark.aylett@swirlycloud.com"_sv));
}

void TestModel::doReadOrder(const ModelCallback<OrderPtr>& cb) const
{
}

void TestModel::doReadTrade(const ModelCallback<ExecPtr>& cb) const
{
}

void TestModel::doReadPosn(Jday busDay, const ModelCallback<PosnPtr>& cb) const
{
}

TestJourn::TestJourn() noexcept = default;
TestJourn::~TestJourn() noexcept = default;

constexpr TestJourn::TestJourn(const TestJourn&) noexcept = default;
TestJourn& TestJourn::operator=(const TestJourn&) noexcept = default;

constexpr TestJourn::TestJourn(TestJourn&&) noexcept = default;
TestJourn& TestJourn::operator=(TestJourn&&) noexcept = default;

void TestJourn::doUpdate(const Msg& msg)
{
}

} // swirly

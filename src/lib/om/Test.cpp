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
#include <swirly/om/Test.hpp>

#include <swirly/fin/Asset.hpp>
#include <swirly/fin/Contr.hpp>

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
  cb(Asset::make(1_id32, "CHF"_sv, "Switzerland, Francs"_sv, AssetType::Currency));
  cb(Asset::make(2_id32, "EUR"_sv, "Euro Member Countries, Euro"_sv, AssetType::Currency));
  cb(Asset::make(3_id32, "GBP"_sv, "United Kingdom, Pounds"_sv, AssetType::Currency));
  cb(Asset::make(4_id32, "JPY"_sv, "Japan, Yen"_sv, AssetType::Currency));
  cb(Asset::make(5_id32, "USD"_sv, "United States of America, Dollars"_sv, AssetType::Currency));
  cb(Asset::make(6_id32, "ZAR"_sv, "South Africa, Rand"_sv, AssetType::Currency));
  // Coal.
  cb(Asset::make(7_id32, "CAP"_sv, "Central Appalachia Coal"_sv, AssetType::Commodity));
  cb(Asset::make(8_id32, "NAP"_sv, "Northern Appalachia Coal"_sv, AssetType::Commodity));
  cb(Asset::make(9_id32, "ILB"_sv, "Illinois Basin Coal"_sv, AssetType::Commodity));
  cb(Asset::make(10_id32, "PRB"_sv, "Powder River Basin Coal"_sv, AssetType::Commodity));
  cb(Asset::make(11_id32, "UIB"_sv, "Uinta Basin Coal"_sv, AssetType::Commodity));
  // Coffee.
  cb(Asset::make(12_id32, "ETB"_sv, "Ethiopia, Birr"_sv, AssetType::Currency));
  cb(Asset::make(13_id32, "WYCA"_sv, "Yirgachefe A"_sv, AssetType::Commodity));
  cb(Asset::make(14_id32, "WWNA"_sv, "Wenago A"_sv, AssetType::Commodity));
  cb(Asset::make(15_id32, "WKCA"_sv, "Kochere A"_sv, AssetType::Commodity));
  cb(Asset::make(16_id32, "WGAA"_sv, "Gelena Abaya A"_sv, AssetType::Commodity));
  // US Corporates.
  cb(Asset::make(17_id32, "CSCO"_sv, "Cisco Systems Inc"_sv, AssetType::Corporate));
  cb(Asset::make(18_id32, "DIS"_sv, "Walt Disney"_sv, AssetType::Corporate));
  cb(Asset::make(19_id32, "IBM"_sv, "Ibm Corp"_sv, AssetType::Corporate));
  cb(Asset::make(20_id32, "INTC"_sv, "Intel Corp"_sv, AssetType::Corporate));
  cb(Asset::make(21_id32, "MSFT"_sv, "Microsoft Corp"_sv, AssetType::Corporate));
  cb(Asset::make(22_id32, "VIA"_sv, "Viacom Inc"_sv, AssetType::Corporate));
  cb(Asset::make(23_id32, "VOD"_sv, "Vodafone Group Plc"_sv, AssetType::Corporate));
  cb(Asset::make(24_id32, "VZ"_sv, "Verizon Com"_sv, AssetType::Corporate));
}

void TestModel::doReadContr(const ModelCallback<ContrPtr>& cb) const
{
  // Forex.
  cb(Contr::make(1_id32, "EURUSD"_sv, "EURUSD"_sv, "EUR"_sv, "USD"_sv, 1000000, 1, 1, 10000, 4,
                 1_lts, 10_lts));
  cb(Contr::make(2_id32, "GBPUSD"_sv, "GBPUSD"_sv, "GBP"_sv, "USD"_sv, 1000000, 1, 1, 10000, 4,
                 1_lts, 10_lts));
  cb(Contr::make(3_id32, "USDCHF"_sv, "USDCHF"_sv, "USD"_sv, "CHF"_sv, 1000000, 1, 1, 10000, 4,
                 1_lts, 10_lts));
  cb(Contr::make(4_id32, "USDJPY"_sv, "USDJPY"_sv, "USD"_sv, "JPY"_sv, 1000000, 1, 1, 100, 2, 1_lts,
                 10_lts));
  // Coal.
  cb(Contr::make(5_id32, "CAP"_sv, "Central Appalachia Coal"_sv, "CAP"_sv, "USD"_sv, 1000, 1, 1, 20,
                 2, 1_lts, 10_lts));
  cb(Contr::make(6_id32, "NAP"_sv, "Northern Appalachia Coal"_sv, "NAP"_sv, "USD"_sv, 1000, 1, 1,
                 20, 2, 1_lts, 10_lts));
  cb(Contr::make(7_id32, "ILB"_sv, "Illinois Basin Coal"_sv, "ILB"_sv, "USD"_sv, 1000, 1, 1, 20, 2,
                 1_lts, 10_lts));
  cb(Contr::make(8_id32, "PRB"_sv, "Powder River Basin Coal"_sv, "PRB"_sv, "USD"_sv, 1000, 1, 1, 20,
                 2, 1_lts, 10_lts));
  cb(Contr::make(9_id32, "UIB"_sv, "Uinta Basin Coal"_sv, "UIB"_sv, "USD"_sv, 1000, 1, 1, 20, 2,
                 1_lts, 10_lts));
  // Coffee.
  cb(Contr::make(10_id32, "WYCA"_sv, "Yirgachefe A"_sv, "WYCA"_sv, "ETB"_sv, 1, 1, 1, 1, 0, 1_lts,
                 10_lts));
  cb(Contr::make(11_id32, "WWNA"_sv, "Wenago A"_sv, "WWNA"_sv, "ETB"_sv, 1, 1, 1, 1, 0, 1_lts,
                 10_lts));
  cb(Contr::make(12_id32, "WKCA"_sv, "Kochere A"_sv, "WKCA"_sv, "ETB"_sv, 1, 1, 1, 1, 0, 1_lts,
                 10_lts));
  cb(Contr::make(13_id32, "WGAA"_sv, "Gelena Abaya A"_sv, "WGAA"_sv, "ETB"_sv, 1, 1, 1, 1, 0, 1_lts,
                 10_lts));
  // US Corporates.
  cb(Contr::make(14_id32, "CSCO"_sv, "Cisco Systems Inc"_sv, "CSCO"_sv, "USD"_sv, 1, 1, 1, 1000, 3,
                 1_lts, 10_lts));
  cb(Contr::make(15_id32, "DIS"_sv, "Walt Disney"_sv, "DIS"_sv, "USD"_sv, 1, 1, 1, 1000, 3, 1_lts,
                 10_lts));
  cb(Contr::make(16_id32, "IBM"_sv, "Ibm Corp"_sv, "IBM"_sv, "USD"_sv, 1, 1, 1, 1000, 3, 1_lts,
                 10_lts));
  cb(Contr::make(17_id32, "INTC"_sv, "Intel Corp"_sv, "INTC"_sv, "USD"_sv, 1, 1, 1, 1000, 3, 1_lts,
                 10_lts));
  cb(Contr::make(18_id32, "MSFT"_sv, "Microsoft Corp"_sv, "MSFT"_sv, "USD"_sv, 1, 1, 1, 1000, 3,
                 1_lts, 10_lts));
  cb(Contr::make(19_id32, "VIA"_sv, "Viacom Inc"_sv, "VIA"_sv, "USD"_sv, 1, 1, 1, 1000, 3, 1_lts,
                 10_lts));
  cb(Contr::make(20_id32, "VOD"_sv, "Vodafone Group Plc"_sv, "VOD"_sv, "USD"_sv, 1, 1, 1, 1000, 3,
                 1_lts, 10_lts));
  cb(Contr::make(21_id32, "VZ"_sv, "Verizon Com"_sv, "VZ"_sv, "USD"_sv, 1, 1, 1, 1000, 3, 1_lts,
                 10_lts));
}

void TestModel::doReadMarket(const ModelCallback<MarketPtr>& cb) const
{
}

void TestModel::doReadAccnt(Time now, const ModelCallback<string_view>& cb) const
{
}

void TestModel::doReadOrder(const ModelCallback<OrderPtr>& cb) const
{
}

void TestModel::doReadExec(string_view accnt, size_t limit, const ModelCallback<ExecPtr>& cb) const
{
}

void TestModel::doReadTrade(const ModelCallback<ExecPtr>& cb) const
{
}

void TestModel::doReadPosn(JDay busDay, const ModelCallback<PosnPtr>& cb) const
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

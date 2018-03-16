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
#include "Test.hpp"

#include <swirly/fin/Asset.hpp>
#include <swirly/fin/Instr.hpp>

namespace swirly {
inline namespace clob {
using namespace std;

TestModel::TestModel() noexcept = default;
TestModel::~TestModel() noexcept = default;

constexpr TestModel::TestModel(const TestModel&) noexcept = default;
TestModel& TestModel::operator=(const TestModel&) noexcept = default;

constexpr TestModel::TestModel(TestModel&&) noexcept = default;
TestModel& TestModel::operator=(TestModel&&) noexcept = default;

void TestModel::doReadAsset(const ModelCallback<AssetPtr>& cb) const
{
    // Forex.
    cb(Asset::make(1_id32, "CHF"sv, "Switzerland, Francs"sv, AssetType::Ccy));
    cb(Asset::make(2_id32, "EUR"sv, "Euro Member Countries, Euro"sv, AssetType::Ccy));
    cb(Asset::make(3_id32, "GBP"sv, "United Kingdom, Pounds"sv, AssetType::Ccy));
    cb(Asset::make(4_id32, "JPY"sv, "Japan, Yen"sv, AssetType::Ccy));
    cb(Asset::make(5_id32, "USD"sv, "United States of America, Dollars"sv, AssetType::Ccy));
    cb(Asset::make(6_id32, "ZAR"sv, "South Africa, Rand"sv, AssetType::Ccy));
    // Coal.
    cb(Asset::make(7_id32, "CAP"sv, "Central Appalachia Coal"sv, AssetType::Cmdty));
    cb(Asset::make(8_id32, "NAP"sv, "Northern Appalachia Coal"sv, AssetType::Cmdty));
    cb(Asset::make(9_id32, "ILB"sv, "Illinois Basin Coal"sv, AssetType::Cmdty));
    cb(Asset::make(10_id32, "PRB"sv, "Powder River Basin Coal"sv, AssetType::Cmdty));
    cb(Asset::make(11_id32, "UIB"sv, "Uinta Basin Coal"sv, AssetType::Cmdty));
    // Coffee.
    cb(Asset::make(12_id32, "ETB"sv, "Ethiopia, Birr"sv, AssetType::Ccy));
    cb(Asset::make(13_id32, "WYCA"sv, "Yirgachefe A"sv, AssetType::Cmdty));
    cb(Asset::make(14_id32, "WWNA"sv, "Wenago A"sv, AssetType::Cmdty));
    cb(Asset::make(15_id32, "WKCA"sv, "Kochere A"sv, AssetType::Cmdty));
    cb(Asset::make(16_id32, "WGAA"sv, "Gelena Abaya A"sv, AssetType::Cmdty));
    // US Corporates.
    cb(Asset::make(17_id32, "CSCO"sv, "Cisco Systems Inc"sv, AssetType::Corp));
    cb(Asset::make(18_id32, "DIS"sv, "Walt Disney"sv, AssetType::Corp));
    cb(Asset::make(19_id32, "IBM"sv, "Ibm Corp"sv, AssetType::Corp));
    cb(Asset::make(20_id32, "INTC"sv, "Intel Corp"sv, AssetType::Corp));
    cb(Asset::make(21_id32, "MSFT"sv, "Microsoft Corp"sv, AssetType::Corp));
    cb(Asset::make(22_id32, "VIA"sv, "Viacom Inc"sv, AssetType::Corp));
    cb(Asset::make(23_id32, "VOD"sv, "Vodafone Group Plc"sv, AssetType::Corp));
    cb(Asset::make(24_id32, "VZ"sv, "Verizon Com"sv, AssetType::Corp));
}

void TestModel::doReadInstr(const ModelCallback<InstrPtr>& cb) const
{
    // Forex.
    cb(Instr::make(1_id32, "EURUSD"sv, "EURUSD"sv, "EUR"sv, "USD"sv, 1000000, 1, 1, 10000, 4, 1_lts,
                   10_lts));
    cb(Instr::make(2_id32, "GBPUSD"sv, "GBPUSD"sv, "GBP"sv, "USD"sv, 1000000, 1, 1, 10000, 4, 1_lts,
                   10_lts));
    cb(Instr::make(3_id32, "USDCHF"sv, "USDCHF"sv, "USD"sv, "CHF"sv, 1000000, 1, 1, 10000, 4, 1_lts,
                   10_lts));
    cb(Instr::make(4_id32, "USDJPY"sv, "USDJPY"sv, "USD"sv, "JPY"sv, 1000000, 1, 1, 100, 2, 1_lts,
                   10_lts));
    // Coal.
    cb(Instr::make(5_id32, "CAP"sv, "Central Appalachia Coal"sv, "CAP"sv, "USD"sv, 1000, 1, 1, 20,
                   2, 1_lts, 10_lts));
    cb(Instr::make(6_id32, "NAP"sv, "Northern Appalachia Coal"sv, "NAP"sv, "USD"sv, 1000, 1, 1, 20,
                   2, 1_lts, 10_lts));
    cb(Instr::make(7_id32, "ILB"sv, "Illinois Basin Coal"sv, "ILB"sv, "USD"sv, 1000, 1, 1, 20, 2,
                   1_lts, 10_lts));
    cb(Instr::make(8_id32, "PRB"sv, "Powder River Basin Coal"sv, "PRB"sv, "USD"sv, 1000, 1, 1, 20,
                   2, 1_lts, 10_lts));
    cb(Instr::make(9_id32, "UIB"sv, "Uinta Basin Coal"sv, "UIB"sv, "USD"sv, 1000, 1, 1, 20, 2,
                   1_lts, 10_lts));
    // Coffee.
    cb(Instr::make(10_id32, "WYCA"sv, "Yirgachefe A"sv, "WYCA"sv, "ETB"sv, 1, 1, 1, 1, 0, 1_lts,
                   10_lts));
    cb(Instr::make(11_id32, "WWNA"sv, "Wenago A"sv, "WWNA"sv, "ETB"sv, 1, 1, 1, 1, 0, 1_lts,
                   10_lts));
    cb(Instr::make(12_id32, "WKCA"sv, "Kochere A"sv, "WKCA"sv, "ETB"sv, 1, 1, 1, 1, 0, 1_lts,
                   10_lts));
    cb(Instr::make(13_id32, "WGAA"sv, "Gelena Abaya A"sv, "WGAA"sv, "ETB"sv, 1, 1, 1, 1, 0, 1_lts,
                   10_lts));
    // US Corporates.
    cb(Instr::make(14_id32, "CSCO"sv, "Cisco Systems Inc"sv, "CSCO"sv, "USD"sv, 1, 1, 1, 1000, 3,
                   1_lts, 10_lts));
    cb(Instr::make(15_id32, "DIS"sv, "Walt Disney"sv, "DIS"sv, "USD"sv, 1, 1, 1, 1000, 3, 1_lts,
                   10_lts));
    cb(Instr::make(16_id32, "IBM"sv, "Ibm Corp"sv, "IBM"sv, "USD"sv, 1, 1, 1, 1000, 3, 1_lts,
                   10_lts));
    cb(Instr::make(17_id32, "INTC"sv, "Intel Corp"sv, "INTC"sv, "USD"sv, 1, 1, 1, 1000, 3, 1_lts,
                   10_lts));
    cb(Instr::make(18_id32, "MSFT"sv, "Microsoft Corp"sv, "MSFT"sv, "USD"sv, 1, 1, 1, 1000, 3,
                   1_lts, 10_lts));
    cb(Instr::make(19_id32, "VIA"sv, "Viacom Inc"sv, "VIA"sv, "USD"sv, 1, 1, 1, 1000, 3, 1_lts,
                   10_lts));
    cb(Instr::make(20_id32, "VOD"sv, "Vodafone Group Plc"sv, "VOD"sv, "USD"sv, 1, 1, 1, 1000, 3,
                   1_lts, 10_lts));
    cb(Instr::make(21_id32, "VZ"sv, "Verizon Com"sv, "VZ"sv, "USD"sv, 1, 1, 1, 1000, 3, 1_lts,
                   10_lts));
}

void TestModel::doReadMarket(const ModelCallback<MarketPtr>& cb) const {}

void TestModel::doReadOrder(const ModelCallback<OrderPtr>& cb) const {}

void TestModel::doReadExec(Time since, const ModelCallback<ExecPtr>& cb) const {}

void TestModel::doReadTrade(const ModelCallback<ExecPtr>& cb) const {}

void TestModel::doReadPosn(JDay busDay, const ModelCallback<PosnPtr>& cb) const {}

TestJourn::TestJourn() noexcept = default;
TestJourn::~TestJourn() noexcept = default;

constexpr TestJourn::TestJourn(const TestJourn&) noexcept = default;
TestJourn& TestJourn::operator=(const TestJourn&) noexcept = default;

constexpr TestJourn::TestJourn(TestJourn&&) noexcept = default;
TestJourn& TestJourn::operator=(TestJourn&&) noexcept = default;

void TestJourn::doUpdate(const Msg& msg) {}

} // namespace clob
} // namespace swirly

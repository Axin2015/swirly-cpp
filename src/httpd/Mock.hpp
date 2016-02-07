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
#ifndef SWIRLY_HTTPD_MOCK_HPP
#define SWIRLY_HTTPD_MOCK_HPP

#include <swirly/elm/Journ.hpp>
#include <swirly/elm/Model.hpp>

namespace swirly {

/**
 * @addtogroup App
 * @{
 */

class MockModel : public Model {
 protected:
    AssetSet doReadAsset(const Factory& factory) const override;

    ContrSet doReadContr(const Factory& factory) const override;

    MarketSet doReadMarket(const Factory& factory) const override;

    TraderSet doReadTrader(const Factory& factory) const override;

 public:
    MockModel() noexcept;
    ~MockModel() noexcept override;

    // Copy.
    constexpr MockModel(const MockModel&) noexcept;
    MockModel& operator=(const MockModel&) noexcept;

    // Move.
    constexpr MockModel(MockModel&&) noexcept;
    MockModel& operator=(MockModel&&) noexcept;
};

class MockJourn : public Journ {
 protected:
    void doCreateMarket(const StringView& mnem, const StringView& display, const StringView& contr,
                        Jday settlDay, Jday expiryDay, MarketState state) override;

    void doUpdateMarket(const StringView& mnem, const StringView& display,
                        MarketState state) override;

    void doCreateTrader(const StringView& mnem, const StringView& display,
                        const StringView& email) override;

    void doUpdateTrader(const StringView& mnem, const StringView& display) override;

    void doCreateExec(const Exec& exec) override;

    void doCreateExec(const StringView& market, const ArrayView<Exec*>& execs) override;

    void doCreateExec(const ArrayView<Exec*>& execs) override;

    void doArchiveOrder(const StringView& market, Iden id, Millis modified) override;

    void doArchiveOrder(const StringView& market, const ArrayView<Iden>& ids,
                        Millis modified) override;

    void doArchiveOrder(const ArrayView<MarketId>& ids, Millis modified) override;

    void doArchiveTrade(const StringView& market, Iden id, Millis modified) override;

    void doArchiveTrade(const StringView& market, const ArrayView<Iden>& ids,
                        Millis modified) override;

    void doArchiveTrade(const ArrayView<MarketId>& ids, Millis modified) override;

 public:
    MockJourn() noexcept;
    ~MockJourn() noexcept override;

    // Copy.
    constexpr MockJourn(const MockJourn&) noexcept;
    MockJourn& operator=(const MockJourn&) noexcept;

    // Move.
    constexpr MockJourn(MockJourn&&) noexcept;
    MockJourn& operator=(MockJourn&&) noexcept;
};

/** @} */

} // swirly

#endif // SWIRLY_HTTPD_MOCK_HPP

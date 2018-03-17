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
#ifndef SWIRLY_CLOB_TEST_HPP
#define SWIRLY_CLOB_TEST_HPP

#include <swirly/fin/Journ.hpp>
#include <swirly/fin/Model.hpp>

namespace swirly {
inline namespace clob {

class SWIRLY_API TestModel : public Model {
  public:
    TestModel() noexcept;
    ~TestModel() noexcept override;

    // Copy.
    constexpr TestModel(const TestModel&) noexcept;
    TestModel& operator=(const TestModel&) noexcept;

    // Move.
    constexpr TestModel(TestModel&&) noexcept;
    TestModel& operator=(TestModel&&) noexcept;

  protected:
    void doReadAsset(const ModelCallback<AssetPtr>& cb) const override;

    void doReadInstr(const ModelCallback<InstrPtr>& cb) const override;

    void doReadMarket(const ModelCallback<MarketPtr>& cb) const override;

    void doReadOrder(const ModelCallback<OrderPtr>& cb) const override;

    void doReadExec(Time since, const ModelCallback<ExecPtr>& cb) const override;

    void doReadTrade(const ModelCallback<ExecPtr>& cb) const override;

    void doReadPosn(JDay busDay, const ModelCallback<PosnPtr>& cb) const override;
};

class SWIRLY_API TestJourn : public Journ {
  public:
    TestJourn() noexcept;
    ~TestJourn() noexcept override;

    // Copy.
    constexpr TestJourn(const TestJourn&) noexcept;
    TestJourn& operator=(const TestJourn&) noexcept;

    // Move.
    constexpr TestJourn(TestJourn&&) noexcept;
    TestJourn& operator=(TestJourn&&) noexcept;

  protected:
    void doUpdate(const Msg& msg) override;
};

} // namespace clob
} // namespace swirly

#endif // SWIRLY_CLOB_TEST_HPP

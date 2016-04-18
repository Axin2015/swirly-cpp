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
#ifndef SWIRLY_FIG_MOCK_HPP
#define SWIRLY_FIG_MOCK_HPP

#include <swirly/elm/Journ.hpp>
#include <swirly/elm/Model.hpp>

namespace swirly {

/**
 * @addtogroup IO
 * @{
 */

class SWIRLY_API MockModel : public Model {
 public:
  MockModel() noexcept;
  ~MockModel() noexcept override;

  // Copy.
  constexpr MockModel(const MockModel&) noexcept;
  MockModel& operator=(const MockModel&) noexcept;

  // Move.
  constexpr MockModel(MockModel&&) noexcept;
  MockModel& operator=(MockModel&&) noexcept;

 protected:
  AssetSet doReadAsset(const Factory& factory) const override;

  ContrSet doReadContr(const Factory& factory) const override;

  MarketSet doReadMarket(const Factory& factory) const override;

  TraderSet doReadTrader(const Factory& factory) const override;
};

class SWIRLY_API MockJourn : public Journ {
 public:
  MockJourn() noexcept;
  ~MockJourn() noexcept override;

  // Copy.
  constexpr MockJourn(const MockJourn&) noexcept;
  MockJourn& operator=(const MockJourn&) noexcept;

  // Move.
  constexpr MockJourn(MockJourn&&) noexcept;
  MockJourn& operator=(MockJourn&&) noexcept;

 protected:
  void doCreateMarket(std::string_view mnem, std::string_view display, std::string_view contr,
                      Jday settlDay, Jday expiryDay, MarketState state) override;

  void doUpdateMarket(std::string_view mnem, std::string_view display, MarketState state) override;

  void doCreateTrader(std::string_view mnem, std::string_view display,
                      std::string_view email) override;

  void doUpdateTrader(std::string_view mnem, std::string_view display) override;

  void doCreateExec(const Exec& exec) override;

  void doCreateExec(std::string_view market, ArrayView<Exec*> execs) override;

  void doCreateExec(ArrayView<Exec*> execs) override;

  void doArchiveOrder(std::string_view market, ArrayView<Iden> ids, Millis modified) override;

  void doArchiveTrade(std::string_view market, ArrayView<Iden> ids, Millis modified) override;
};

/** @} */

} // swirly

#endif // SWIRLY_FIG_MOCK_HPP

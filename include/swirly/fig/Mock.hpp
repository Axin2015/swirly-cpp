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
  void doReadAsset(const Factory& factory, const ModelCallback<AssetPtr>& cb) const override;

  void doReadContr(const Factory& factory, const ModelCallback<ContrPtr>& cb) const override;

  void doReadMarket(const Factory& factory, const ModelCallback<MarketPtr>& cb) const override;

  void doReadTrader(const Factory& factory, const ModelCallback<TraderPtr>& cb) const override;

  void doReadOrder(const Factory& factory, const ModelCallback<OrderPtr>& cb) const override;

  void doReadTrade(const Factory& factory, const ModelCallback<ExecPtr>& cb) const override;

  void doReadPosn(Jday busDay, const Factory& factory,
                  const ModelCallback<PosnPtr>& cb) const override;
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
  void doReset() noexcept override;

  void doCreateMarket(Mnem mnem, std::string_view display, Mnem contr, Jday settlDay,
                      Jday expiryDay, MarketState state) override;

  void doUpdateMarket(Mnem mnem, std::string_view display, MarketState state) override;

  void doCreateTrader(Mnem mnem, std::string_view display, std::string_view email) override;

  void doUpdateTrader(Mnem mnem, std::string_view display) override;

  void doCreateExec(const Exec& exec, More more) override;

  void doArchiveOrder(Mnem market, Iden id, Millis modified, More more) override;

  void doArchiveTrade(Mnem market, Iden id, Millis modified, More more) override;
};

/** @} */

} // swirly

#endif // SWIRLY_FIG_MOCK_HPP

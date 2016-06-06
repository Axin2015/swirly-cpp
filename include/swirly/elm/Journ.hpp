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
#ifndef SWIRLY_ELM_JOURN_HPP
#define SWIRLY_ELM_JOURN_HPP

#include <swirly/elm/Types.hpp>

#include <swirly/ash/Array.hpp>
#include <swirly/ash/Mnem.hpp>
#include <swirly/ash/Types.hpp>

namespace swirly {

class Conf;

/**
 * @addtogroup IO
 * @{
 */

class SWIRLY_API Journ {
 public:
  Journ() noexcept = default;
  virtual ~Journ() noexcept;

  // Copy.
  constexpr Journ(const Journ&) noexcept = default;
  Journ& operator=(const Journ&) noexcept = default;

  // Move.
  constexpr Journ(Journ&&) noexcept = default;
  Journ& operator=(Journ&&) noexcept = default;

  /**
   * Reset multi-part sequence. Must not throw.
   */
  void reset() noexcept { doReset(); }

  /**
   * Create Market.
   */
  void createMarket(Mnem mnem, std::string_view display, Mnem contr, Jday settlDay, Jday expiryDay,
                    MarketState state)
  {
    doCreateMarket(mnem, display, contr, settlDay, expiryDay, state);
  }
  /**
   * Update Market.
   */
  void updateMarket(Mnem mnem, std::string_view display, MarketState state)
  {
    doUpdateMarket(mnem, display, state);
  }
  /**
   * Create Trader.
   */
  void createTrader(Mnem mnem, std::string_view display, std::string_view email)
  {
    doCreateTrader(mnem, display, email);
  }
  /**
   * Update Trader.
   */
  void updateTrader(Mnem mnem, std::string_view display) { doUpdateTrader(mnem, display); }
  /**
   * Create Execution.
   */
  void createExec(const Exec& exec) { doCreateExec(exec, More::No); }
  /**
   * Create Executions. This overload may be less efficient than ones that are market-specific.
   */
  void createExec(ArrayView<ConstExecPtr> execs);
  /**
   * Archive Order.
   */
  void archiveOrder(Mnem market, Iden id, Millis modified)
  {
    doArchiveOrder(market, id, modified, More::No);
  }
  /**
   * Archive Orders.
   */
  void archiveOrder(Mnem market, ArrayView<Iden> ids, Millis modified);
  /**
   * Archive Trade.
   */
  void archiveTrade(Mnem market, Iden id, Millis modified)
  {
    doArchiveTrade(market, id, modified, More::No);
  }
  /**
   * Archive Trades.
   */
  void archiveTrade(Mnem market, ArrayView<Iden> ids, Millis modified);

 protected:
  virtual void doReset() noexcept = 0;

  virtual void doCreateMarket(Mnem mnem, std::string_view display, Mnem contr, Jday settlDay,
                              Jday expiryDay, MarketState state)
    = 0;

  virtual void doUpdateMarket(Mnem mnem, std::string_view display, MarketState state) = 0;

  virtual void doCreateTrader(Mnem mnem, std::string_view display, std::string_view email) = 0;

  virtual void doUpdateTrader(Mnem mnem, std::string_view display) = 0;

  virtual void doCreateExec(const Exec& exec, More more) = 0;

  virtual void doArchiveOrder(Mnem market, Iden id, Millis modified, More more) = 0;

  virtual void doArchiveTrade(Mnem market, Iden id, Millis modified, More more) = 0;
};

/**
 * Make Journal. Forward declaration for Journal backend.
 *
 * @param conf Configuration.
 */
SWIRLY_API std::unique_ptr<Journ> makeJourn(const Conf& conf);

/** @} */

} // swirly

#endif // SWIRLY_ELM_JOURN_HPP

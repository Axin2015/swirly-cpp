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
   * Create Market.
   */
  void createMarket(std::string_view mnem, std::string_view display, std::string_view contr,
                    Jday settlDay, Jday expiryDay, MarketState state)
  {
    doCreateMarket(mnem, display, contr, settlDay, expiryDay, state);
  }
  /**
   * Update Market.
   */
  void updateMarket(std::string_view mnem, std::string_view display, MarketState state)
  {
    doUpdateMarket(mnem, display, state);
  }
  /**
   * Create Trader.
   */
  void createTrader(std::string_view mnem, std::string_view display, std::string_view email)
  {
    doCreateTrader(mnem, display, email);
  }
  /**
   * Update Trader.
   */
  void updateTrader(std::string_view mnem, std::string_view display)
  {
    doUpdateTrader(mnem, display);
  }
  /**
   * Create Execution.
   */
  void createExec(const Exec& exec) { doCreateExec(exec); }
  /**
   * Create Executions.
   */
  void createExec(std::string_view market, ArrayView<ConstExecPtr> execs)
  {
    doCreateExec(market, execs);
  }
  /**
   * Create Executions. This overload may be less efficient than ones that are market-specific.
   */
  void createExec(ArrayView<ConstExecPtr> execs) { doCreateExec(execs); }
  /**
   * Archive Order.
   */
  void archiveOrder(std::string_view market, Iden id, Millis modified)
  {
    doArchiveOrder(market, id, modified);
  }
  /**
   * Archive Orders.
   */
  void archiveOrder(std::string_view market, ArrayView<Iden> ids, Millis modified)
  {
    doArchiveOrder(market, ids, modified);
  }
  /**
   * Archive Trade.
   */
  void archiveTrade(std::string_view market, Iden id, Millis modified)
  {
    doArchiveTrade(market, id, modified);
  }
  /**
   * Archive Trades.
   */
  void archiveTrade(std::string_view market, ArrayView<Iden> ids, Millis modified)
  {
    doArchiveTrade(market, ids, modified);
  }

 protected:
  virtual void doCreateMarket(std::string_view mnem, std::string_view display,
                              std::string_view contr, Jday settlDay, Jday expiryDay,
                              MarketState state)
    = 0;

  virtual void doUpdateMarket(std::string_view mnem, std::string_view display, MarketState state)
    = 0;

  virtual void doCreateTrader(std::string_view mnem, std::string_view display,
                              std::string_view email)
    = 0;

  virtual void doUpdateTrader(std::string_view mnem, std::string_view display) = 0;

  virtual void doCreateExec(const Exec& exec) = 0;

  virtual void doCreateExec(std::string_view market, ArrayView<ConstExecPtr> execs) = 0;

  virtual void doCreateExec(ArrayView<ConstExecPtr> execs) = 0;

  virtual void doArchiveOrder(std::string_view market, Iden id, Millis modified) = 0;

  virtual void doArchiveOrder(std::string_view market, ArrayView<Iden> ids, Millis modified) = 0;

  virtual void doArchiveTrade(std::string_view market, Iden id, Millis modified) = 0;

  virtual void doArchiveTrade(std::string_view market, ArrayView<Iden> ids, Millis modified) = 0;
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

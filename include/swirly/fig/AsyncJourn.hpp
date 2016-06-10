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
#ifndef SWIRLY_ELM_ASYNCJOURN_HPP
#define SWIRLY_ELM_ASYNCJOURN_HPP

#include <swirly/elm/Msg.hpp>

#include <swirly/ash/Array.hpp>

namespace swirly {

class Journ;

/**
 * @addtogroup IO
 * @{
 */

class SWIRLY_API AsyncJourn {
 public:
  AsyncJourn(Journ& journ, std::size_t capacity);
  ~AsyncJourn() noexcept;

  // Copy.
  AsyncJourn(const AsyncJourn&) = delete;
  AsyncJourn& operator=(const AsyncJourn&) = delete;

  // Move.
  AsyncJourn(AsyncJourn&&) = delete;
  AsyncJourn& operator=(AsyncJourn&&) = delete;

  /**
   * Reset multi-part sequence.
   */
  void reset() { doReset(); }

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

 private:
  void doReset();

  void doCreateMarket(Mnem mnem, std::string_view display, Mnem contr, Jday settlDay,
                      Jday expiryDay, MarketState state);

  void doUpdateMarket(Mnem mnem, std::string_view display, MarketState state);

  void doCreateTrader(Mnem mnem, std::string_view display, std::string_view email);

  void doUpdateTrader(Mnem mnem, std::string_view display);

  void doCreateExec(const Exec& exec, More more);

  void doArchiveOrder(Mnem market, Iden id, Millis modified, More more);

  void doArchiveTrade(Mnem market, Iden id, Millis modified, More more);

  MsgPipe pipe_;
  std::thread thread_;
};

/** @} */

} // swirly

#endif // SWIRLY_ELM_ASYNCJOURN_HPP

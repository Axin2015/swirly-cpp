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

namespace detail {

template <std::size_t StepN>
class AsyncWindow {
 public:
  explicit AsyncWindow(std::size_t total) noexcept : total_{total} {}
  ~AsyncWindow() noexcept = default;

  // Copy.
  AsyncWindow(const AsyncWindow&) = delete;
  AsyncWindow& operator=(const AsyncWindow&) = delete;

  // Move.
  AsyncWindow(AsyncWindow&&) = delete;
  AsyncWindow& operator=(AsyncWindow&&) = delete;

  std::size_t index() const noexcept { return index_; }
  std::size_t size() const noexcept { return std::min(total_ - index_, StepN); }
  More more() const noexcept { return total_ - index_ > StepN ? More::Yes : More::No; }
  bool done() const noexcept { return index_ >= total_; }
  bool next() noexcept
  {
    index_ += StepN;
    return !done();
  }

 private:
  const std::size_t total_;
  std::size_t index_{0};
};

} // detail

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
   * Create Execution.
   */
  void createExec(const Exec& exec) { doCreateExec(exec, More::No); }
  /**
   * Create Executions. This overload may be less efficient than ones that are market-specific.
   */
  void createExec(ArrayView<ConstExecPtr> execs);
  /**
   * Archive Trade.
   */
  void archiveTrade(Mnem market, Iden id, Millis modified)
  {
    doArchiveTrade(market, {&id, 1}, modified, More::No);
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

  void doCreateExec(const Exec& exec, More more);

  void doArchiveTrade(Mnem market, ArrayView<Iden> ids, Millis modified, More more);

  MsgPipe pipe_;
  std::thread thread_;
};

} // swirly

#endif // SWIRLY_ELM_ASYNCJOURN_HPP
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
#ifndef SWIRLY_FIN_MSGQUEUE_HPP
#define SWIRLY_FIN_MSGQUEUE_HPP

#include <swirly/fin/Msg.hpp>

#include <swirly/util/Array.hpp>

#include <swirly/sys/MemQueue.hpp>

#include <cassert>

namespace swirly {
inline namespace fin {
class Journ;
namespace detail {
template <std::size_t StepN>
class Range {
  public:
    constexpr explicit Range(std::size_t size) noexcept
    : size_{size}
    {
    }
    constexpr Range() noexcept = default;
    ~Range() = default;

    // Copy.
    Range(const Range&) = delete;
    Range& operator=(const Range&) = delete;

    // Move.
    Range(Range&&) = delete;
    Range& operator=(Range&&) = delete;

    constexpr std::size_t stepOffset() const noexcept { return offset_; }
    constexpr std::size_t stepSize() const noexcept { return std::min(size_ - offset_, StepN); }
    constexpr std::size_t steps() const noexcept { return (size_ + StepN - 1) / StepN; }
    constexpr bool done() const noexcept { return offset_ >= size_; }
    bool next() noexcept
    {
        offset_ += StepN;
        return !done();
    }

  private:
    const std::size_t size_{0};
    std::size_t offset_{0};
};
} // namespace detail

class SWIRLY_API MsgQueue {
  public:
    explicit MsgQueue(std::size_t capacity)
    : mq_{capacity}
    {
    }
    explicit MsgQueue(const char* path)
    : mq_{path}
    {
    }
    ~MsgQueue();

    // Copy.
    MsgQueue(const MsgQueue&) = delete;
    MsgQueue& operator=(const MsgQueue&) = delete;

    // Move.
    MsgQueue(MsgQueue&&) = delete;
    MsgQueue& operator=(MsgQueue&&) = delete;

    /**
     * Send interrupt.
     */
    bool interrupt(Id32 num, Millis timeout = 5000ms) noexcept
    {
        assert(num > 0_id32);
        return doInterrupt(num, timeout);
    }
    /**
     * Create Market.
     */
    void createMarket(Id64 id, Symbol instr, JDay settlDay, MarketState state)
    {
        doCreateMarket(id, instr, settlDay, state);
    }
    /**
     * Update Market.
     */
    void updateMarket(Id64 id, MarketState state) { doUpdateMarket(id, state); }
    /**
     * Create Execution.
     */
    void createExec(const Exec& exec) { doCreateExec(exec); }
    /**
     * Create Executions.
     */
    void createExec(ArrayView<ConstExecPtr> execs);
    /**
     * Archive Trade.
     */
    void archiveTrade(Id64 marketId, Id64 id, Time modified)
    {
        doArchiveTrade(marketId, {&id, 1}, modified);
    }
    /**
     * Archive Trades.
     */
    void archiveTrade(Id64 marketId, ArrayView<Id64> ids, Time modified);
    /**
     * Returns false if queue is empty.
     */
    bool pop(Msg& msg) noexcept { return mq_.pop(msg); }

  private:
    bool doInterrupt(Id32 num, Millis timeout);

    void doCreateMarket(Id64 id, Symbol instr, JDay settlDay, MarketState state);

    void doUpdateMarket(Id64 id, MarketState state);

    void doCreateExec(const Exec& exec);

    void doArchiveTrade(Id64 marketId, ArrayView<Id64> ids, Time modified);

    MemQueue<Msg> mq_;
};

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_MSGQUEUE_HPP

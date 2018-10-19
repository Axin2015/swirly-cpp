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

#include <swirly/app/MemQueue.hpp>

#include <swirly/util/Array.hpp>

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

    constexpr std::size_t step_offset() const noexcept { return offset_; }
    constexpr std::size_t step_size() const noexcept { return std::min(size_ - offset_, StepN); }
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
    MsgQueue(std::nullptr_t = nullptr) noexcept {}
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
    MsgQueue(MsgQueue&&) = default;
    MsgQueue& operator=(MsgQueue&&) = default;

    /**
     * Create Market.
     */
    void create_market(WallTime now, Id64 id, Symbol product, JDay settl_day, MarketState state)
    {
        do_create_market(now, id, product, settl_day, state);
    }
    /**
     * Update Market.
     */
    void update_market(WallTime now, Id64 id, MarketState state)
    {
        do_update_market(now, id, state);
    }
    /**
     * Create Execution.
     */
    void create_exec(const Exec& exec) { do_create_exec(exec); }
    /**
     * Create Executions.
     */
    void create_exec(ArrayView<ConstExecPtr> execs);
    /**
     * Archive Trade.
     */
    void archive_trade(WallTime now, Id64 market_id, Id64 id)
    {
        do_archive_trade(now, market_id, {&id, 1});
    }
    /**
     * Archive Trades.
     */
    void archive_trade(WallTime now, Id64 market_id, ArrayView<Id64> ids);
    /**
     * Returns false if queue is empty.
     */
    bool pop(Msg& msg) noexcept { return mq_.pop(msg); }

  private:
    void do_create_market(WallTime now, Id64 id, Symbol product, JDay settl_day, MarketState state);

    void do_update_market(WallTime now, Id64 id, MarketState state);

    void do_create_exec(const Exec& exec);

    void do_archive_trade(WallTime now, Id64 market_id, ArrayView<Id64> ids);

    MemQueue<Msg> mq_{nullptr};
};

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_MSGQUEUE_HPP

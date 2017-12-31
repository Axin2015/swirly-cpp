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
#ifndef SWIRLY_UTIL_PIPE_HPP
#define SWIRLY_UTIL_PIPE_HPP

#include <swirly/util/RingBuffer.hpp>

#include <cassert>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace swirly {

template <typename ValueT>
class Pipe {
  public:
    explicit Pipe(std::size_t capacity)
      : buf_{capacity}
    {
    }
    ~Pipe() noexcept = default;

    // Copy.
    Pipe(const Pipe& rhs) = delete;
    Pipe& operator=(const Pipe& rhs) = delete;

    // Move.
    Pipe(Pipe&&) = delete;
    Pipe& operator=(Pipe&&) = delete;

    bool empty() const noexcept
    {
        std::lock_guard<std::mutex> lock{mutex_};
        return buf_.empty();
    }
    bool full(uint16_t size) const noexcept
    {
        std::lock_guard<std::mutex> lock{mutex_};
        return buf_.full();
    }
    template <typename FnT>
    bool read(FnT fn)
    {
        std::unique_lock<std::mutex> lock{mutex_};
        notEmpty_.wait(lock, [this] { return !this->buf_.empty() || closed_; });
        if (buf_.empty()) {
            assert(closed_);
            return false;
        }
        // Continue to read when closed while buffer is not empty.
        buf_.read(fn);
        lock.unlock();
        notFull_.notify_one();
        return true;
    }
    template <typename FnT>
    bool write(FnT fn)
    {
        std::unique_lock<std::mutex> lock{mutex_};
        notFull_.wait(lock, [this] { return !this->buf_.full() || closed_; });
        // Prevent further writes when closed.
        if (closed_) {
            return false;
        }
        buf_.write(fn);
        lock.unlock();
        notEmpty_.notify_one();
        return true;
    }
    void close()
    {
        {
            std::unique_lock<std::mutex> lock{mutex_};
            closed_ = true;
            // Unlock mutex before notifying to avoid contention.
        }
        notEmpty_.notify_all();
        notFull_.notify_all();
    }

  private:
    std::mutex mutex_;
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;
    bool closed_{false};
    RingBuffer<ValueT> buf_;
};

} // namespace swirly

#endif // SWIRLY_UTIL_PIPE_HPP

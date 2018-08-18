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
#ifndef SWIRLY_APP_THREAD_HPP
#define SWIRLY_APP_THREAD_HPP

#include <swirly/app/Backoff.hpp>

#include <swirly/sys/Signal.hpp>

#include <swirly/util/Log.hpp>

#include <atomic>
#include <thread>

#include <unistd.h>

namespace swirly {
inline namespace sys {
class Reactor;
} // namespace sys
inline namespace app {

struct ThreadConfig {
    std::string name;
};

class AgentThread {
  public:
    template <typename AgentT>
    AgentThread(AgentT& agent, ThreadConfig config)
    : thread_{run<AgentT, NoBackoff>, std::ref(agent), NoBackoff{}, config, std::cref(stop_)}
    {
    }
    template <typename AgentT, typename BackoffT>
    AgentThread(AgentT& agent, BackoffT backoff, ThreadConfig config)
    : thread_{run<AgentT, BackoffT>, std::ref(agent), backoff, config, std::cref(stop_)}
    {
    }
    ~AgentThread()
    {
        stop_.store(true, std::memory_order_relaxed);
        thread_.join();
    }

    // Copy.
    AgentThread(const AgentThread&) = delete;
    AgentThread& operator=(const AgentThread&) = delete;

    // Move.
    AgentThread(AgentThread&&) = delete;
    AgentThread& operator=(AgentThread&&) = delete;

  private:
    template <typename AgentT, typename BackoffT>
    static void run(AgentT& agent, BackoffT backoff, ThreadConfig config,
                    const std::atomic<bool>& stop)
    {
        sig_block_all();
        pthread_setname_np(pthread_self(), config.name.c_str());
        SWIRLY_NOTICE << "started " << config.name << " thread";
        try {
            while (!stop.load(std::memory_order_relaxed)) {
                if (agent() == 0) {
                    backoff.reset();
                    do {
                        backoff.idle();
                        if (stop.load(std::memory_order_relaxed)) {
                            break;
                        }
                    } while (agent() == 0);
                }
            }
        } catch (const std::exception& e) {
            SWIRLY_ERROR << "exception: " << e.what();
            kill(getpid(), SIGTERM);
        }
        SWIRLY_NOTICE << "stopping " << config.name << " thread";
    }
    std::atomic<bool> stop_{false};
    std::thread thread_;
};

class SWIRLY_API ReactorThread {
  public:
    ReactorThread(Reactor& r, ThreadConfig config);
    ~ReactorThread();

    // Copy.
    ReactorThread(const ReactorThread&) = delete;
    ReactorThread& operator=(const ReactorThread&) = delete;

    // Move.
    ReactorThread(ReactorThread&&) = delete;
    ReactorThread& operator=(ReactorThread&&) = delete;

  private:
    Reactor& reactor_;
    std::atomic<bool> stop_{false};
    std::thread thread_;
};

} // namespace app
} // namespace swirly

#endif // SWIRLY_APP_THREAD_HPP

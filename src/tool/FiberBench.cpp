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
#include <swirly/util/CloseHandler.hpp>
#include <swirly/util/Log.hpp>
#include <swirly/util/Profile.hpp>

#include <swirly/sys/File.hpp>
#include <swirly/sys/Reactor.hpp>
#include <swirly/sys/Signal.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ptree_fwd.hpp>

#include <algorithm>
#include <atomic>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>

#include <signal.h>

namespace swirly {

enum class Topic : int { None = 0 };

class TopicMap {
    using Id = std::underlying_type_t<Topic>;

  public:
    TopicMap() = default;
    ~TopicMap() noexcept = default;

    // Copy.
    TopicMap(const TopicMap&) = default;
    TopicMap& operator=(const TopicMap&) = default;

    // Move.
    TopicMap(TopicMap&&) = default;
    TopicMap& operator=(TopicMap&&) = default;

    Topic operator[](const std::string& name)
    {
        std::lock_guard<std::mutex> lock{mutex_};
        auto& id = map_[name];
        if (id == 0) {
            id = ++maxId_;
        }
        return static_cast<Topic>(id);
    }

  private:
    std::mutex mutex_;
    Id maxId_{0};
    std::unordered_map<std::string, Id> map_;
};

struct FiberConfig {
    FiberConfig(std::string name, std::string type)
      : name{std::move(name)}
      , type{std::move(type)}
    {
    }
    std::string name, type;
};

struct ThreadConfig {
    ThreadConfig(std::string name)
      : name{std::move(name)}
    {
    }
    std::string name;
    std::vector<FiberConfig> fibers;
};

struct AppConfig {
    std::vector<ThreadConfig> threads;
};

void readConfig(boost::property_tree::ptree& root, AppConfig& config)
{
    for (const auto& child : root.get_child("threads")) {
        const auto& obj = child.second;
        const auto& name = obj.get<std::string>("name");
        config.threads.emplace_back(name);
        for (const auto& child : obj.get_child("fibers")) {
            const auto& obj = child.second;
            const auto& name = obj.get<std::string>("name");
            const auto& type = obj.get<std::string>("type");
            config.threads.back().fibers.emplace_back(name, type);
        }
    }
}

void readConfig(std::istream& is, AppConfig& config)
{
    namespace pt = boost::property_tree;
    pt::ptree root;
    pt::read_json(is, root);
    readConfig(root, config);
}

using Fiber = EventHandler;
using FiberPtr = EventHandlerPtr;

} // namespace swirly

using namespace std;
using namespace swirly;

namespace {

class SendFiber : public Fiber {
  public:
    SendFiber(Reactor& r, const string& name, Time now)
      : Fiber{r}
    {
        const auto eh = self();
        tok_ = r.subscribe(eh);
        tmr_ = r.setTimer(now + 1s, 10ms, eh);
    }
    ~SendFiber() noexcept override { SWIRLY_INFO("~SendFiber()"); }
    static auto make(Reactor& r, const string& name, Time now)
    {
        return makeIntrusive<SendFiber>(r, name, now);
    }

  protected:
    void doClose() noexcept override
    {
        tmr_.reset();
        tok_.reset();
    }
    void doTimer(const Timer& tmr, Time now) override
    {
        using Clock = chrono::high_resolution_clock;

        for (int i{0}; i < 10; ++i) {
            const auto now = Clock::now();
            auto fn = [now](MsgEvent & ev) noexcept
            {
                emplaceEvent<Clock::time_point>(ev, 1, now);
            };
            postEvent(fn);
        }
    }

  private:
    EventToken tok_;
    Timer tmr_;
};

class RecvFiber : public Fiber {
  public:
    RecvFiber(Reactor& r, const string& name, Time now)
      : Fiber{r}
      , prof_{{name.data(), name.size()}}
    {
        const auto eh = self();
        tok_ = r.subscribe(eh);
    }
    ~RecvFiber() noexcept override { SWIRLY_INFO("~RecvFiber()"); }
    static auto make(Reactor& r, const string& name, Time now)
    {
        return makeIntrusive<RecvFiber>(r, name, now);
    }

  protected:
    void doClose() noexcept override
    {
        tok_.reset();
    }
    void doEvent(const MsgEvent& ev, Time now) override
    {
        using Clock = chrono::high_resolution_clock;

        if (++events_ <= 1000) {
            // Warmup.
            return;
        }

        const auto start = data<Clock::time_point>(ev);
        const auto end = Clock::now();
        const chrono::duration<double, micro> diff{end - start};
        const auto usec = diff.count();
        prof_.record(usec);

        if (events_ % 1000 == 0) {
            prof_.report();
        }
    }

  private:
    int events_{};
    Profile prof_;
    EventToken tok_;
};

FiberPtr makeFiber(Reactor& r, const string& name, const string& type, Time now)
{
    using namespace string_literals;

    FiberPtr fiber;
    if (type == "Send") {
        fiber = SendFiber::make(r, name, now);
    } else if (type == "Recv") {
        fiber = RecvFiber::make(r, name, now);
    } else {
        throw domain_error{"invalid fiber type: "s + type};
    }
    return fiber;
}

void run(Reactor& r, const ThreadConfig& config)
{
    sigBlockAll();
    pthread_setname_np(pthread_self(), config.name.c_str());
    // TODO: set addditional thread attributes here.

    const auto now = UnixClock::now();
    for (const auto& fiber : config.fibers) {
        makeFiber(r, fiber.name, fiber.type, now);
    }
    const auto ch = CloseHandler::make(r);
    while (!ch->closed()) {
        r.poll();
    }
}

constexpr char SampleConfig[] = R"JSON(
{
    "threads":
    [
        {
            "name": "Foo",
            "fibers":
            [
                {
                    "name": "FooSend",
                    "type": "Send"
                }
            ]
        },
        {
            "name": "Bar",
            "fibers":
            [
                {
                    "name": "BarRecv",
                    "type": "Recv"
                }
            ]
        },
        {
            "name": "Baz",
            "fibers":
            [
                {
                    "name": "BazRecv",
                    "type": "Recv"
                }
            ]
        }
    ]
}
)JSON";

} // namespace

int main(int argc, char* argv[])
{
    int ret = 1;
    try {

        // Read Container config.
        stringstream is{SampleConfig};
        AppConfig config;
        readConfig(is, config);

        // Create Reactors.
        vector<Reactor> rs;
        rs.resize(config.threads.size());
        setReactors(rs);

        // Start Threads.
        vector<thread> ts;
        ts.reserve(config.threads.size());
        for (size_t i{0}; i < config.threads.size(); ++i) {
            ts.emplace_back(run, ref(rs[i]), cref(config.threads[i]));
        }

        // Wait for termination.
        {
            SigWait sigWait;
            while (const auto sig = sigWait()) {
                auto fn = [sig](MsgEvent & ev) noexcept { emplaceSignal(ev, sig); };
                postEvent(fn);
                switch (sig) {
                case SIGINT:
                case SIGTERM:
                    // Exit application.
                    break;
                default:
                    continue;
                }
                break;
            }
        }

        // Join threads.
        for_each(ts.begin(), ts.end(), [](auto& t) { t.join(); });
        ret = 0;

    } catch (const exception& e) {
        SWIRLY_ERROR(logMsg() << "exception: " << e.what());
    }
    return ret;
}

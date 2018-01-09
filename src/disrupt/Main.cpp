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
#include "Disruptor.hpp"
#include "EventHandler.hpp"
#include "Executor.hpp"
#include "RingBuffer.hpp"
#include "Sequence.hpp"

#include <iostream>

using namespace std;

namespace swirly {

class LongEventHandler : public EventHandler {
  public:
    ~LongEventHandler() noexcept override {}
    void onEvent(const LongEvent& event, long sequence, bool endOfBatch) override
    {
        cout << "Event: " << event << endl;
    }
};

class LongEventProducer {
  public:
    explicit LongEventProducer(RingBuffer& ringBuffer) noexcept
      : ringBuffer_(ringBuffer)
    {
    }
    void onData(long value)
    {
        long sequence = ringBuffer_.next();
        LongEvent& event = ringBuffer_.get(sequence);
        event.set(value);
        ringBuffer_.publish(sequence);
    }

  private:
    RingBuffer& ringBuffer_;
};

} // namespace swirly

int main(int argc, char* argv[])
{
    constexpr int RingBufferSize{1 << 10};

    using namespace swirly;
    int ret{1};
    try {
        Executor executor;
        Disruptor disruptor(RingBufferSize, executor);
        disruptor.handleEventsWith(make_shared<LongEventHandler>());
        disruptor.start();
        auto& ringBuffer = disruptor.ringBuffer();
        LongEventProducer producer{ringBuffer};
        for (int i{0}; i < 100; ++i) {
            producer.onData(i);
        }
        cout << sizeof(Sequence) << endl;
        ret = 0;
    } catch (const std::exception& e) {
        cerr << "exception: " << e.what() << endl;
    }
    return ret;
}

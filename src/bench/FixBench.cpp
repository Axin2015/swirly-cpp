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
#include <swirly/fix/Random.hpp>
#include <swirly/fix/Stream.hpp>
#include <swirly/fix/Traits.hpp>

#include <swirly/hdr/Histogram.hpp>
#include <swirly/hdr/Recorder.hpp>

#include <swirly/util/Log.hpp>

using namespace std;
using namespace swirly;

int main(int argc, char* argv[])
{
    int ret = 1;
    try {

        std::random_device rd;
        RandomBbo bbo{rd};
        Buffer buf{1024};
        FixStream os{buf};

        Histogram hist{1, 1'000'000, 5};

        for (int i{0}; i < 5'000'000; ++i) {
            const auto [bid, offer] = bbo(12345_tks);
            {
                Recorder tr{hist};
                os.reset({4, 3});
                // clang-format off
                os << put_fix<Tag::MsgType>("W"sv)
                   << put_fix<Tag::NoMdEntries>(4)
                   << put_fix<Tag::MdEntryType>(Side::Buy)
                   << put_fix<Tag::MdEntryPx>(bid - 1_tks)
                   << put_fix<Tag::MdEntrySize>(2000_lts)
                   << put_fix<Tag::MdEntryType>(Side::Buy)
                   << put_fix<Tag::MdEntryPx>(bid)
                   << put_fix<Tag::MdEntrySize>(1000_lts)
                   << put_fix<Tag::MdEntryType>(Side::Sell)
                   << put_fix<Tag::MdEntryPx>(offer)
                   << put_fix<Tag::MdEntrySize>(1000_lts)
                   << put_fix<Tag::MdEntryType>(Side::Sell)
                   << put_fix<Tag::MdEntryPx>(offer + 1_tks)
                   << put_fix<Tag::MdEntrySize>(2000_lts);
                // clang-format on
                os.commit();
            }
            buf.clear();
        }

        hist.percentiles_print("fix.hdr", 5, 1000);
        ret = 0;

    } catch (const exception& e) {
        SWIRLY_ERROR << "exception: " << e.what();
    }
    return ret;
}

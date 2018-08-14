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
#include "Options.hpp"

#include <swirly/util/Variant.hpp>

#include <iomanip>
#include <iostream>

using namespace std;

namespace {
constexpr string_view long_opt_prefix{"--"};
constexpr string_view short_opt_prefix{"-"};
} // namespace

namespace swirly {
inline namespace util {

Options::Options(string description)
: description_{move(description)}
{
}

bool Options::operator[](const string& long_opt) const noexcept
{
    return opts_.count(long_opt);
}

bool Options::operator[](char short_opt) const noexcept
{
    return opts_.count(short_opt);
}

void Options::parse(int argc, char* argv[])
{
    size_t positional_idx{0};
    for (int idx = 1; idx < argc; ++idx) {
        string_view opt{argv[idx]};

        bool positional{false};
        OptsMap::iterator it{opts_.end()};

        if (!opt.compare(0, long_opt_prefix.size(), long_opt_prefix)) {
            opt.remove_prefix(long_opt_prefix.size());
            it = opts_.find(opt);
        } else if (!opt.compare(0, short_opt_prefix.size(), short_opt_prefix)) {
            opt.remove_prefix(short_opt_prefix.size());
            it = opts_.find(opt[0]);
        } else {
            if (positional_idx >= positional_.size()) {
                throw runtime_error("unexpected option: " + string{opt});
            }
            positional = true;
        }

        if (!positional && it == opts_.end()) {
            throw runtime_error("unknown option: " + string{opt});
        }

        auto& data = positional ? positional_[positional_idx++].data_ : it->second.data_;

        visit(
            overloaded{
                [this](const Help& arg) {
                    cout << *this << "\n";
                    exit(0);
                },
                [](Switch& arg) { arg.run(); },
                [&](Value& arg) {
                    // Adjust index to read value from next arg, if it's not
                    ///  a positional option
                    if (!positional && ++idx >= argc) {
                        throw runtime_error("expected parameter for option: " + string{opt});
                    }
                    if (arg.set_ && !arg.multitoken_) {
                        throw runtime_error{"duplicate option: " + string{opt}};
                    }
                    arg.run(argv[idx]);
                },
                [](const NoOp& arg) {},
            },
            data);
    }

    // Ensure all required parameters have been set
    for (const auto& data : opts_) {
        visit(overloaded{[](const auto& def) {},
                         [&](const Value& arg) {
                             if (arg.presence() == Value::Required && !arg.set()) {
                                 throw runtime_error("missing required option: "
                                                     + data.first.long_opt);
                             }
                         }},
              data.second.data_);
    }
}

ostream& operator<<(ostream& out, const Options& options)
{
    out << "Usage: " << options.description_ << "\nOptions:\n";

    for (const auto& opt : options.opts_) {
        unsigned max_width{15};
        // value find
        out << "  ";
        if (opt.first.short_opt) {
            max_width -= short_opt_prefix.size() + 1;
            out << short_opt_prefix << opt.first.short_opt;
        }
        if (!opt.first.long_opt.empty()) {
            if (opt.first.short_opt) {
                max_width -= 2;
                out << ", ";
            }
            max_width -= long_opt_prefix.size() + opt.first.long_opt.size();
            out << long_opt_prefix << opt.first.long_opt;
        }
        out << setw(max_width) << ' ' << opt.second.description_ << "\n";
    }
    return out;
}

} // namespace util
} // namespace swirly

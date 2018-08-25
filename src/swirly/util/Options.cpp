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

#include "Argv.hpp"
#include "Variant.hpp"

#include <iomanip>
#include <iostream>

namespace swirly {
inline namespace util {
using namespace std;

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
    return opts_.count(std::string_view{&short_opt, 1});
}

void Options::parse(int argc, const char* const argv[])
{
    size_t positional_idx{0};
    ArgvLexer lex{argc - 1, argv + 1};
    while (!lex.empty()) {
        const auto opt = lex.opt();

        bool positional{false};
        OptsMap::iterator it{opts_.end()};

        if (!opt.empty()) {
            it = opts_.find(opt);
        } else {
            if (positional_idx >= positional_.size()) {
                throw runtime_error{"unexpected argument: " + string{lex.arg()}};
            }
            positional = true;
        }

        if (!positional && it == opts_.end()) {
            throw runtime_error{"unknown option: " + string{opt}};
        }

        auto& data = positional ? positional_[positional_idx++] : it->second->data;

        visit(overloaded{[&](const Help& arg) {
                             lex.pop_switch();
                             cout << *this << "\n";
                             exit(0);
                         },
                         [&](Switch& arg) {
                             lex.pop_switch();
                             arg.run();
                         },
                         [&](Value& arg) {
                             const auto val = lex.pop_value();
                             if (arg.set_ && !arg.multitoken_) {
                                 throw runtime_error{"duplicate option: " + string{opt}};
                             }
                             arg.run(val);
                         },
                         [&](const NoOp& arg) { lex.pop(); }},
              data);
    }

    // Ensure all required parameters have been set.
    // FIXME: but we're not checking required positional arguments?
    for (const auto& data : opts_) {
        visit(overloaded{[](const auto& def) {},
                         [&](const Value& arg) {
                             if (arg.presence() == Value::Required && !arg.set()) {
                                 throw runtime_error{"missing required option: "
                                                     + data.second->opt()};
                             }
                         }},
              data.second->data);
    }
    for (const auto& data : positional_) {
        visit(overloaded{[](const auto& def) {},
                         [&](const Value& arg) {
                             if (arg.presence() == Value::Required && !arg.set()) {
                                 throw runtime_error{"missing positional argument"};
                             }
                         }},
              data);
    }
}

ostream& operator<<(ostream& out, const Options& options)
{
    out << "Usage: " << options.description_ << "\nOptions:\n";

    for (const auto& opt : options.help_) {
        unsigned max_width{15};
        // value find
        out << "  ";
        if (!opt->short_opt.empty()) {
            max_width -= 2;
            out << '-' << opt->short_opt;
        }
        if (!opt->long_opt.empty()) {
            if (!opt->short_opt.empty()) {
                max_width -= 2;
                out << ", ";
            }
            max_width -= 2 + opt->long_opt.size();
            out << "--" << opt->long_opt;
        }
        out << setw(max_width) << ' ' << opt->description << "\n";
    }
    return out;
}

} // namespace util
} // namespace swirly

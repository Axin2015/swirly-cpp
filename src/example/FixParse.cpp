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
#include <swirly/fix/Lexer.hpp>
#include <swirly/fix/Parser.hpp>

#include <swirly/sys/File.hpp>

#include <iostream>
#include <random>

using namespace std;
using namespace swirly;

namespace {

class FixParser : public BasicFixParser<FixParser> {
  public:
    void on_message(CyclTime now, string_view msg, size_t msg_type_off, Version ver)
    {
        FixLexer lex{msg};
        while (!lex.empty()) {
            const auto [t, v] = lex.next();
            cout << t << '=' << v << '^';
        }
        cout << endl;
    }
};

} // namespace

int main(int argc, char* argv[])
{
    int ret = 1;
    try {

        random_device rd;
        mt19937 gen{rd()};
        uniform_int_distribution<> dis{1, 3};

        FixParser p;

        Buffer buf;
        for (;;) {
            const auto in = buf.prepare(dis(gen));
            const auto n = os::read(0, in);
            if (n == 0) {
                break;
            }
            buf.commit(n);
            buf.consume(p.parse(CyclTime::set(), buf.data()));
        }

        ret = 0;
    } catch (const exception& e) {
        cerr << "exception: " << e.what();
    }
    return ret;
}

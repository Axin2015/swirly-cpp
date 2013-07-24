/*
 *  Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
 *
 *  This file is part of Doobry written by Mark Aylett.
 *
 *  Doobry is free software; you can redistribute it and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Doobry is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program; if
 *  not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA.
 */
#include "test.hpp"

#include <iostream>

#undef  TEST_CASE
#define TEST_CASE(x) void x(void);
#include <test_cases.txt>

#undef  TEST_CASE
#define TEST_CASE(x) { .name = #x, .fn = x },

static const struct TestCase {
    const char* name;
    void (*fn)(void);
} TEST_CASES[] = {
#include <test_cases.txt>
};

using namespace std;

static void
run(const TestCase& tc)
{
    cout << tc.name << ':';
    cout.flush();
    try {
        tc.fn();
    } catch (const dbr::DbrException& e) {
        cout << " fail" << endl;
        cerr << e.file() << ':' << e.line() << ": " << e.what() << " (" << e.num() << ')' << endl;
        exit(1);
    } catch (const exception& e) {
        cout << " fail" << endl;
        cerr << "exception: " << e.what() << endl;
        exit(1);
    }
    cout << " pass" << endl;
}

int
main(int argc, char* argv[])
{
    if (argc > 1) {
        // Run specific test-cases according to arguments.
        int n = 0;
        for (int i = 1; i < argc; ++i) {
            for (size_t j = 0; j < sizeof(TEST_CASES) / sizeof(TEST_CASES[0]); ++j)
                if (strcmp(argv[i], TEST_CASES[j].name) == 0) {
                    run(TEST_CASES[j]);
                    ++n;
                }
        }
        if (n == 0) {
            cerr << "no test found\n";
            exit(1);
        }
    } else {
        // Run all test-cases.
        for (size_t j = 0; j < sizeof(TEST_CASES) / sizeof(TEST_CASES[0]); ++j)
            run(TEST_CASES [j]);
    }
    return 0;
}

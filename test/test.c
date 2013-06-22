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
#include "test.h"

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

static bool running = false;

static void
exit_handler(void)
{
    if (running)
        printf("fail\n");
}

static void
run(const struct TestCase* tc)
{
    printf("%s: ", tc->name);
    running = true;
    tc->fn();
    running = false;
    printf("pass\n");
}

int
main(int argc, char* argv[])
{
    atexit(exit_handler);
    if (argc > 1) {
        // Run specific test-cases according to arguments.
        int n = 0;
        for (int i = 1; i < argc; ++i) {
            for (int j = 0; j < sizeof(TEST_CASES) / sizeof(TEST_CASES[0]); ++j)
                if (strcmp(argv[i], TEST_CASES[j].name) == 0) {
                    run(TEST_CASES + j);
                    ++n;
                }
        }
        if (n == 0)
            die("no test found");
    } else {
        // Run all test-cases.
        for (int j = 0; j < sizeof(TEST_CASES) / sizeof(TEST_CASES[0]); ++j)
            run(TEST_CASES + j);
    }
    return 0;
}

/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "test.hpp"

#include <dbrpp/ash/except.hpp>

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
    } catch (const TestException& e) {
        cout << " fail" << endl;
        cerr << e.file() << ':' << e.line() << ": " << e.what() << endl;
        exit(1);
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
    srand(time(NULL));

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

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
#ifndef SWIRLY_TOOL_MICROBENCH_HXX
#define SWIRLY_TOOL_MICROBENCH_HXX

#include <swirly/hdr/Histogram.hpp>
#include <swirly/hdr/Recorder.hpp>

#include <iostream>

namespace swirly {
inline namespace tool {

class BenchmarkBase {
  public:
    BenchmarkBase() = default;
    void run() { return do_run(); }

  protected:
    virtual ~BenchmarkBase() = default;
    virtual void do_run() = 0;
};

template <typename DerivedT>
class BasicBenchmark : public BenchmarkBase {
  public:
    explicit BasicBenchmark(const std::string& name)
    : name_{name}
    {
    }
    const std::string& name() const noexcept { return name_; }

  protected:
    ~BasicBenchmark() override = default;
    void do_run() override
    {
        using namespace std;

        Histogram hist{1, 1'000'000, 5};
        auto fn = static_cast<DerivedT*>(this)->create();

        // Estimate the number of function calls required for a cumulative time of approximately 50
        // microseconds.
        int n{1};
        do {
            Recorder tr{hist, n};
            for (int j{0}; j < n; ++j) {
                fn();
            }
            n <<= 1;
            // Where n-1 is now the sum of all trials.
            // I.e. (1<<4)-1 == 15 == 1+2+4+8.
        } while (hist.value_at_percentile(50) * (n - 1) < 50'000);
        --n;

        hist.clear();

        int count{0};
        const auto start = chrono::high_resolution_clock::now();
        for (;;) {
            {
                Recorder tr{hist, n};
                for (int j{0}; j < n; ++j) {
                    fn();
                }
            }
            const auto end = chrono::high_resolution_clock::now();
            count += n;
            if (end - start >= 1s) {
                break;
            }
        }

        // clang-format off
        cout << left << setw(24) << name_
             << right << setw(12) << count
             << right << setw(12) << hist.min()
             << right << setw(12) << hist.value_at_percentile(50)
             << right << setw(12) << hist.value_at_percentile(95)
             << right << setw(12) << hist.value_at_percentile(99)
             << right << setw(12) << hist.max()
             << endl;
        // clang-format on
    }

  private:
    const std::string name_;
};

inline void print_header()
{
    using namespace std;
    // clang-format off
    cout << left << setw(24) << "NAME"
         << right << setw(12) << "COUNT"
         << right << setw(12) << "MIN"
         << right << setw(12) << "%50"
         << right << setw(12) << "%95"
         << right << setw(12) << "%99"
         << right << setw(12) << "MAX"
         << endl;
    // clang-format on

    // Separator.
    cout << setw(96) << setfill('-') << '-' << setfill(' ') << endl;
}

// The clobber() and escape() functions are used to defeat the optimiser.
// See CppCon 2015: Chandler Carruth "Tuning C++: Benchmarks, and CPUs, and Compilers! Oh My!"
// And https://github.com/google/benchmark/blob/master/include/benchmark/benchmark.h

inline void clobber_memory() noexcept
{
    asm volatile("" : : : "memory");
}

template <typename ValueT>
inline void do_not_optimise(const ValueT& val)
{
    asm volatile("" : : "r,m"(val) : "memory");
}

template <typename ValueT>
inline void do_not_optimise(ValueT& val)
{
#if defined(__clang__)
    asm volatile("" : "+r,m"(val) : : "memory");
#else
    asm volatile("" : "+m,r"(val) : : "memory");
#endif
}

void register_benchmark(const std::string& name, BenchmarkBase& b);

} // namespace tool
} // namespace swirly

#define SWIRLY_BENCHMARK(NAME)                                                                     \
    namespace swirly::benchmark::NAME {                                                            \
    constexpr char Name[] = #NAME;                                                                 \
    struct Benchmark : BasicBenchmark<Benchmark> {                                                 \
        Benchmark()                                                                                \
        : BasicBenchmark{Name}                                                                     \
        {                                                                                          \
            register_benchmark(name(), *this);                                                     \
        }                                                                                          \
        inline auto create();                                                                      \
    } instance;                                                                                    \
    }                                                                                              \
    auto swirly::benchmark::NAME::Benchmark::create()

#endif // SWIRLY_TOOL_MICROBENCH_HXX

/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
#ifndef SWIRLY_UNIT_TEST_HPP
#define SWIRLY_UNIT_TEST_HPP

#ifndef SWIRLY_API
#define SWIRLY_API __attribute__((visibility("default")))
#endif // SWIRLY_API

#include <cmath>
#include <exception>

namespace swirly {
namespace test {

class SWIRLY_API TestException : public std::exception {
  enum { MaxErr = 511 };
  const char* file_;
  int line_;
  char msg_[MaxErr + 1];

 public:
  TestException(const char* file, int line, const char* msg) noexcept;

  ~TestException() noexcept override;

  // Copy.
  TestException(const TestException& rhs) noexcept = default;
  TestException& operator=(const TestException& rhs) noexcept = default;

  // Move.
  TestException(TestException&&) noexcept = default;
  TestException& operator=(TestException&&) noexcept = default;

  const char* what() const noexcept override;
  const char* file() const noexcept { return file_; }
  int line() const noexcept { return line_; }
};

template <typename ExceptionT, typename FnT>
bool checkThrow(FnT fn)
{
  bool pass{false};
  try {
    fn();
  } catch (const ExceptionT&) {
    pass = true;
  } catch (...) {
  }
  return pass;
}

/**
 * Add test-case. Test-cases use this function to register themselves during static initialisation.
 */
SWIRLY_API void addTestCase(const char* name, void (*fn)(void));

/**
 * Run all test-cases.
 */
SWIRLY_API int run(int argc, char* argv[]);

constexpr auto Epsilon = 1e-7;

inline bool isSame(double lhs, double rhs, double delta = Epsilon) noexcept
{
  return std::abs(lhs - rhs) <= delta;
}

} // test
} // swirly

#define SWIRLY_FAIL_(file, line, what) throw swirly::test::TestException(file, line, what)

#define SWIRLY_FAIL(what) SWIRLY_FAIL_(__FILE__, __LINE__, what)

#define SWIRLY_CHECK(expr) (expr) ? (void)0 : SWIRLY_FAIL("assertion [" #expr "] failed")

#define SWIRLY_CHECK_THROW(expr, ExceptionT)                                                       \
  swirly::test::checkThrow<ExceptionT>([&]() { expr; })                                            \
    ? (void)0                                                                                      \
    : SWIRLY_FAIL("throw expression [" #expr "] failed")

#define SWIRLY_TEST_CASE(name)                                                                     \
  void init##name() __attribute__((constructor));                                                  \
  void test##name();                                                                               \
  void init##name() { swirly::test::addTestCase(#name, test##name); }                              \
  void test##name()

#define SWIRLY_FIXTURE_TEST_CASE(name, FixtureT)                                                   \
  struct name##Fixture_ : FixtureT {                                                               \
    void run();                                                                                    \
  };                                                                                               \
  SWIRLY_TEST_CASE(name)                                                                           \
  {                                                                                                \
    name##Fixture_ fixture;                                                                        \
    fixture.run();                                                                                 \
  }                                                                                                \
  void name##Fixture_::run()

#endif // SWIRLY_UNIT_TEST_HPP

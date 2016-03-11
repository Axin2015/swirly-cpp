/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
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
#ifndef TEST_HPP
#define TEST_HPP

#include <cstring>
#include <exception>

class TestException : public std::exception {
  enum { ErrMax = 127 };
  const char* file_;
  int line_;
  char msg_[ErrMax + 1];

 public:
  TestException(const char* file, int line, const char* msg) noexcept
  {
    file_ = file;
    line_ = line;
    strncpy(msg_, msg, ErrMax);
    msg_[ErrMax] = '\0';
  }
  virtual ~TestException() noexcept {}
  virtual const char* what() const noexcept { return msg_; }
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

#define SWIRLY_FAIL_(file, line, what) throw TestException(file, line, what)

#define SWIRLY_FAIL(what) SWIRLY_FAIL_(__FILE__, __LINE__, what)

#define SWIRLY_CHECK(expr) (expr) ? (void)0 : SWIRLY_FAIL("assertion [" #expr "] failed")

#define SWIRLY_CHECK_THROW(expr, ExceptionT)                                                       \
  checkThrow<ExceptionT>([&]() { expr; }) ? (void)0                                                \
                                          : SWIRLY_FAIL("throw expression [" #expr "] failed")

#define SWIRLY_TEST_CASE(name) void test##name(void)

#endif // TEST_HPP

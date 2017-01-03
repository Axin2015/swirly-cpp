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
#include <swirly/ws/Url.hpp>

#include <swirly/util/String.hpp>

#include <swirly/unit/Test.hpp>

using namespace std;
using namespace swirly;

namespace {

class UrlString : public BasicUrl<UrlString> {
 public:
  explicit UrlString(const string& url) : url_{url} { parse(); }
  const auto& url() const noexcept { return url_; }

 private:
  string url_;
};

class UrlStringView : public BasicUrl<UrlStringView> {
 public:
  explicit UrlStringView(const string_view& url) : url_{url} { parse(); }
  const auto& url() const noexcept { return url_; }

 private:
  string_view url_;
};

} // anonymous

SWIRLY_TEST_CASE(UrlString)
{
  UrlString url{"http://www.swirlycloud.com:8080/accnt/exec?offset=3&limit5"s};
  SWIRLY_CHECK(url.schema() == "http"s);
  SWIRLY_CHECK(url.host() == "www.swirlycloud.com"s);
  SWIRLY_CHECK(url.port() == "8080"s);
  SWIRLY_CHECK(url.path() == "/accnt/exec"s);
  SWIRLY_CHECK(url.query() == "offset=3&limit5"s);
  SWIRLY_CHECK(url.fragment().empty());
  SWIRLY_CHECK(url.userInfo().empty());
}

SWIRLY_TEST_CASE(UrlStringView)
{
  UrlStringView url{"http://www.swirlycloud.com:8080/accnt/exec?offset=3&limit5"_sv};
  SWIRLY_CHECK(url.schema() == "http"_sv);
  SWIRLY_CHECK(url.host() == "www.swirlycloud.com"_sv);
  SWIRLY_CHECK(url.port() == "8080"_sv);
  SWIRLY_CHECK(url.path() == "/accnt/exec"_sv);
  SWIRLY_CHECK(url.query() == "offset=3&limit5"_sv);
  SWIRLY_CHECK(url.fragment().empty());
  SWIRLY_CHECK(url.userInfo().empty());
}

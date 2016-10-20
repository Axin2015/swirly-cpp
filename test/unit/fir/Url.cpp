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
#include <swirly/fir/Url.hpp>

#include <swirly/tea/Test.hpp>

using namespace std;
using namespace swirly;

namespace {

class Url : public BasicUrl<Url> {
 public:
  explicit Url(const string& url) : url_{url} { parse(); }
  const auto& url() const noexcept { return url_; }

 private:
  string url_;
};

} // anonymous

SWIRLY_TEST_CASE(UrlSchemaHostAndPath)
{
  Url url{"http://www.somehost.com/path/file.html"s};
  SWIRLY_CHECK(url.schema() == "http"s);
  SWIRLY_CHECK(url.host() == "www.somehost.com"s);
  SWIRLY_CHECK(url.port().empty());
  SWIRLY_CHECK(url.path() == "/path/file.html"s);
  SWIRLY_CHECK(url.query().empty());
  SWIRLY_CHECK(url.fragment().empty());
  SWIRLY_CHECK(url.userInfo().empty());
}

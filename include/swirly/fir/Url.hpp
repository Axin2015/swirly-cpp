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
#ifndef SWIRLY_FIR_URL_HPP
#define SWIRLY_FIR_URL_HPP

#include <swirly/fir/Exception.hpp>

#include <swirly/fir/http_parser.h>

namespace swirly {

template <typename DerivedT>
class BasicUrl {
 public:
  BasicUrl() noexcept { http_parser_url_init(&parser_); }

  // Copy.
  BasicUrl(const BasicUrl&) noexcept = default;
  BasicUrl& operator=(const BasicUrl&) noexcept = default;

  // Move.
  BasicUrl(BasicUrl&&) noexcept = default;
  BasicUrl& operator=(BasicUrl&&) noexcept = default;

  auto schema() const noexcept
  {
    const auto& field = parser_.field_data[UF_SCHEMA];
    return url().substr(field.off, field.len);
  }
  auto host() const noexcept
  {
    const auto& field = parser_.field_data[UF_HOST];
    return url().substr(field.off, field.len);
  }
  auto port() const noexcept
  {
    const auto& field = parser_.field_data[UF_PORT];
    return url().substr(field.off, field.len);
  }
  auto path() const noexcept
  {
    const auto& field = parser_.field_data[UF_PATH];
    return url().substr(field.off, field.len);
  }
  auto query() const noexcept
  {
    const auto& field = parser_.field_data[UF_QUERY];
    return url().substr(field.off, field.len);
  }
  auto fragment() const noexcept
  {
    const auto& field = parser_.field_data[UF_FRAGMENT];
    return url().substr(field.off, field.len);
  }
  auto userInfo() const noexcept
  {
    const auto& field = parser_.field_data[UF_USERINFO];
    return url().substr(field.off, field.len);
  }

 protected:
  ~BasicUrl() noexcept = default;

  void reset() noexcept { http_parser_url_init(&parser_); }
  void parse(bool isConnect = false)
  {
    const auto rc = http_parser_parse_url(url().data(), url().size(), isConnect ? 1 : 0, &parser_);
    if (rc != 0) {
      throw ParseException{errMsg() << "invalid url: " << url()};
    }
  }

 private:
  decltype(auto) url() const noexcept { return static_cast<const DerivedT*>(this)->url(); }
  http_parser_url parser_{};
};

} // swirly

#endif // SWIRLY_FIR_URL_HPP

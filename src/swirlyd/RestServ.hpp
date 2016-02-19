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
#ifndef SWIRLYD_RESTSERV_HPP
#define SWIRLYD_RESTSERV_HPP

#include "Mongoose.hpp"
#include "Stream.hpp"

#include <swirly/ash/Tokeniser.hpp>

namespace swirly {
class Rest;
namespace mg {

/**
 * @addtogroup App
 * @{
 */

class RestServ : public mg::Mgr<RestServ> {
 public:
  RestServ(Rest& rest, const mg_serve_http_opts& httpOpts) noexcept
  : rest_(rest),
    httpOpts_(httpOpts)
  {
  }
  ~RestServ() noexcept;

  // Copy.
  RestServ(const RestServ& rhs) = delete;
  RestServ& operator=(const RestServ& rhs) = delete;

  // Move.
  RestServ(RestServ&&) = delete;
  RestServ& operator=(RestServ&&) = delete;

  void reset(std::string_view sv) noexcept;
  void httpRequest(mg_connection& nc, mg::HttpMessage data);
  void getRec(mg::HttpMessage data);
  void postRec(mg::HttpMessage data);
  void putRec(mg::HttpMessage data);
  void getSess(mg::HttpMessage data);
  void postSess(mg::HttpMessage data);
  void putSess(mg::HttpMessage data);
  void deleteSess(mg::HttpMessage data);
  void getView(mg::HttpMessage data);

 private:
  Rest& rest_;
  const mg_serve_http_opts& httpOpts_;
  mg::OStream out_;
  Tokeniser<'/'> uri_;
};

/** @} */

} // mg
} // swirly

#endif // SWIRLYD_RESTSERV_HPP

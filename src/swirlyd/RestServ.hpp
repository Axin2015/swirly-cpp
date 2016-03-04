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
#include <swirly/ash/Types.hpp>

#include <swirly/elm/Types.hpp>

#include <vector>

namespace swirly {
class Rest;
namespace mg {

/**
 * @addtogroup App
 * @{
 */

class RestServ : public mg::Mgr<RestServ> {
 public:
  RestServ(Rest& rest, const mg_serve_http_opts& httpOpts) noexcept : rest_(rest),
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

  void setUri(std::string_view uri) noexcept;
  void httpRequest(mg_connection& nc, mg::HttpMessage data);
  void restRequest(mg::HttpMessage data, Millis now);
  void getRec(mg::HttpMessage data, Millis now);
  void getAsset(mg::HttpMessage data, Millis now);
  void getContr(mg::HttpMessage data, Millis now);
  void getMarket(mg::HttpMessage data, Millis now);
  void getTrader(mg::HttpMessage data, Millis now);
  void postRec(mg::HttpMessage data, Millis now);
  void putRec(mg::HttpMessage data, Millis now);
  void getSess(mg::HttpMessage data, Millis now);
  void postSess(mg::HttpMessage data, Millis now);
  void putSess(mg::HttpMessage data, Millis now);
  void deleteSess(mg::HttpMessage data, Millis now);
  void getView(mg::HttpMessage data, Millis now);

 private:
  void splitIds(const std::string_view& sv) noexcept;

  Rest& rest_;
  const mg_serve_http_opts& httpOpts_;
  Tokeniser<'/'> uri_;
  std::vector<Iden> ids_;
  mg::OStream out_;
  bool match_{false};
};

/** @} */

} // mg
} // swirly

#endif // SWIRLYD_RESTSERV_HPP

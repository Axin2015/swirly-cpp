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

#include <swirly/fir/RestRequest.hpp>

#include <swirly/ash/Array.hpp>
#include <swirly/ash/Profile.hpp>
#include <swirly/ash/Tokeniser.hpp>

namespace swirly {
class Rest;
namespace mg {

class RestServ : public mg::Mgr<RestServ> {
 public:
  explicit RestServ(Rest& rest, bool testMode = false) noexcept
    : rest_(rest), testMode_{testMode}, profile_{"profile"_sv}
  {
  }
  ~RestServ() noexcept;

  // Copy.
  RestServ(const RestServ& rhs) = delete;
  RestServ& operator=(const RestServ& rhs) = delete;

  // Move.
  RestServ(RestServ&&) = delete;
  RestServ& operator=(RestServ&&) = delete;

  bool reset(mg::HttpMessage data) noexcept;

  void httpRequest(mg_connection& nc, mg::HttpMessage data);
  void restRequest(mg::HttpMessage data, Millis now);

  void recRequest(mg::HttpMessage data, Millis now);
  void assetRequest(mg::HttpMessage data, Millis now);
  void contrRequest(mg::HttpMessage data, Millis now);
  void marketRequest(mg::HttpMessage data, Millis now);

  void accntRequest(mg::HttpMessage data, Millis now);
  void orderRequest(mg::HttpMessage data, Millis now);
  void tradeRequest(mg::HttpMessage data, Millis now);
  void posnRequest(mg::HttpMessage data, Millis now);

  void viewRequest(mg::HttpMessage data, Millis now);

 private:
  enum : int {
    // Method values are represented as powers of two for simplicity.
    MethodGet = 1 << 0,
    MethodPost = 1 << 1,
    MethodPut = 1 << 2,
    MethodDelete = 1 << 3,
    // Method value mask.
    MethodMask = MethodGet | MethodPost | MethodPut | MethodDelete,

    // Subsequent bits represent matching components.
    MatchMethod = 1 << 4,
    MatchUri = 1 << 5,
    // Match result mask.
    MatchMask = MatchMethod | MatchUri
  };
  bool isSet(int bs) const noexcept { return (state_ & bs) == bs; }

  Rest& rest_;
  bool testMode_{false};
  int state_{0};
  Tokeniser<'/'> uri_;
  std::vector<Iden> ids_;
  std::vector<Mnem> mnems_;
  RestRequest request_;
  mg::OStream out_;
  Profile profile_;
};

} // mg
} // swirly

#endif // SWIRLYD_RESTSERV_HPP

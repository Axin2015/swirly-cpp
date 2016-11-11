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

#include <swirly/ws/RestRequest.hpp>

#include <swirly/util/Array.hpp>
#include <swirly/util/BasicTypes.hpp>
#include <swirly/util/Profile.hpp>
#include <swirly/util/Tokeniser.hpp>

namespace swirly {
class Rest;
namespace mg {

class RestServ : public mg::Mgr<RestServ> {
 public:
  explicit RestServ(Rest& rest) noexcept : rest_(rest), profile_{"profile"_sv} {}
  ~RestServ() noexcept;

  // Copy.
  RestServ(const RestServ& rhs) = delete;
  RestServ& operator=(const RestServ& rhs) = delete;

  // Move.
  RestServ(RestServ&&) = delete;
  RestServ& operator=(RestServ&&) = delete;

  bool reset(mg::HttpMessage data) noexcept;

  void httpRequest(mg_connection& nc, mg::HttpMessage data);
  void restRequest(mg::HttpMessage data, Time now);

  void refDataRequest(mg::HttpMessage data, Time now);
  void assetRequest(mg::HttpMessage data, Time now);
  void contrRequest(mg::HttpMessage data, Time now);

  void marketRequest(mg::HttpMessage data, Time now);

  void accntRequest(mg::HttpMessage data, Time now);
  void orderRequest(mg::HttpMessage data, Time now);
  void execRequest(mg::HttpMessage data, Time now);
  void tradeRequest(mg::HttpMessage data, Time now);
  void posnRequest(mg::HttpMessage data, Time now);

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
  int state_{0};
  Tokeniser uri_;
  std::vector<Id64> ids_;
  std::vector<Mnem> mnems_;
  RestRequest request_;
  mg::OStream out_;
  Profile profile_;
};

} // mg
} // swirly

#endif // SWIRLYD_RESTSERV_HPP

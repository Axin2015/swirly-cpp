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
#ifndef SWIRLY_FIR_REST_HPP
#define SWIRLY_FIR_REST_HPP

#include <swirly/fig/Serv.hpp>

namespace swirly {

/**
 * @addtogroup Rest
 * @{
 */

class SWIRLY_API Rest {
 public:
  Rest(const Model& model, Journ& journ, Millis now) : serv_{model, journ, now} {}
  ~Rest() noexcept;

  // Copy.
  Rest(const Rest&) = delete;
  Rest& operator=(const Rest&) = delete;

  // Move.
  Rest(Rest&&);
  Rest& operator=(Rest&&);

  void assets(Millis now, std::ostream& out) const;

  void contrs(Millis now, std::ostream& out) const;

 private:
  Serv serv_;
};

/** @} */

} // swirly

#endif // SWIRLY_FIR_REST_HPP

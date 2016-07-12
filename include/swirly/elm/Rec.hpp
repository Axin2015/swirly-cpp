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
#ifndef SWIRLY_ELM_REC_HPP
#define SWIRLY_ELM_REC_HPP

#include <swirly/elm/Types.hpp>

#include <swirly/ash/Compare.hpp>
#include <swirly/ash/Mnem.hpp>

namespace swirly {

class SWIRLY_API Rec : public Comparable<Rec> {
 public:
  Rec(RecType type, Mnem mnem, std::string_view display) noexcept
    : type_{type}, mnem_{mnem}, display_{display}
  {
  }

  virtual ~Rec() noexcept;

  // Copy.
  Rec(const Rec&);
  Rec& operator=(const Rec&) = delete;

  // Move.
  Rec(Rec&&);
  Rec& operator=(Rec&&) = delete;

  virtual void toJson(std::ostream& os) const;

  int compare(const Rec& rhs) const noexcept
  {
    int result{swirly::compare(type_, rhs.type_)};
    if (result == 0) {
      result = mnem_.compare(rhs.mnem_);
    }
    return result;
  }
  auto type() const noexcept { return type_; }
  auto mnem() const noexcept { return mnem_; }
  auto display() const noexcept { return +display_; }
  void setDisplay(std::string_view display) noexcept { display_ = display; }

 protected:
  const RecType type_;
  const Mnem mnem_;
  Display display_;
};

inline std::ostream& operator<<(std::ostream& os, const Rec& rec)
{
  rec.toJson(os);
  return os;
}

} // swirly

#endif // SWIRLY_ELM_REC_HPP

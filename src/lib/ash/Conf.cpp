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
#include <swirly/ash/Conf.hpp>

#include <swirly/ash/VarSub.hpp>

using namespace std;

namespace swirly {

Conf::Conf() = default;
Conf::~Conf() noexcept = default;

// Copy.
Conf::Conf(const Conf&) = default;
Conf& Conf::operator=(const Conf&) = default;

// Move.
Conf::Conf(Conf&&) = default;
Conf& Conf::operator=(Conf&&) = default;

const char* Conf::get(const char* key, const char* dfl) const noexcept
{
  auto it = map_.find(key);
  return it != map_.end() ? it->second.c_str() : dfl;
}

int Conf::get(const char* key, int dfl) const noexcept
{
  auto it = map_.find(key);
  // The atoi() function does not throw.
  return it != map_.end() ? atoi(it->second.c_str()) : dfl;
}

bool Conf::get(const char* key, bool dfl) const noexcept
{
  auto it = map_.find(key);
  return it != map_.end() ? stob(it->second, dfl) : dfl;
}

void Conf::read(std::istream& is)
{
  VarSub varSub;
  parsePairs(is, [this, &varSub](const auto& key, string val) {
    varSub(val);
    this->map_.emplace(key, move(val));
  });
}

} // swirly

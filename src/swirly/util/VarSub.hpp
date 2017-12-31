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
#ifndef SWIRLY_UTIL_VARSUB_HPP
#define SWIRLY_UTIL_VARSUB_HPP

#include <swirly/Config.h>

#include <functional>
#include <set>
#include <string>

namespace swirly {

SWIRLY_API std::string getEnv(const std::string& name);

/**
 * Variable substitution.
 */
class SWIRLY_API VarSub {
  public:
    explicit VarSub(std::function<std::string(const std::string&)> fn = getEnv)
      : fn_{std::move(fn)}
    {
    }
    ~VarSub() noexcept;

    // Copy.
    VarSub(const VarSub&);
    VarSub& operator=(const VarSub&);

    // Move.
    VarSub(VarSub&&);
    VarSub& operator=(VarSub&&);

    void operator()(std::string& s) const { substitute(s, std::string::npos, 0); }

  private:
    /**
     * Substitute variables.
     *
     * @param s The string to substitute.
     *
     * @param i Position of opening brace or std::string::npos if top-level.
     *
     * @param j Starting position of search. The search space is [j, size).
     *
     * @param outer Set of names seen at the same position in outer level.
     *
     * @return true if closing brace was found for brace at position i.
     */
    bool substitute(std::string& s, const std::size_t i, std::size_t j,
                    std::set<std::string>* outer = nullptr) const;

    std::function<std::string(const std::string&)> fn_;
};

} // namespace swirly

#endif // SWIRLY_UTIL_VARSUB_HPP

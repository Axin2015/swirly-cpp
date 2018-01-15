/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
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
#ifndef SWIRLY_UTIL_CONFIG_HPP
#define SWIRLY_UTIL_CONFIG_HPP

#include <swirly/util/Numeric.hpp>
#include <swirly/util/String.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <boost/container/flat_map.hpp>
#pragma GCC diagnostic pop

#include <string>
#include <tuple>

namespace swirly {

template <typename FnT>
void parsePairs(std::istream& is, FnT fn)
{
    std::string line;
    while (std::getline(is, line)) {

        trim(line);

        // Ignore empty lines and comments.

        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::string key, val;
        std::tie(key, val) = splitPair(line, '=');
        rtrim(key);
        ltrim(val);

        fn(key, val);
    }
}

/**
 * Simple config reader with environment variable substitution.
 */
class SWIRLY_API Config {
  public:
    explicit Config(std::istream& is) { read(is); }

    Config();
    ~Config() noexcept;

    // Copy.
    Config(const Config&);
    Config& operator=(const Config&);

    // Move.
    Config(Config&&);
    Config& operator=(Config&&);

    const char* get(const char* key, const char* dfl) const noexcept
    {
        auto it = map_.find(key);
        return it != map_.end() ? it->second.c_str() : dfl;
    }
    bool get(const char* key, bool dfl) const noexcept
    {
        auto it = map_.find(key);
        return it != map_.end() ? stob(it->second, dfl) : dfl;
    }
    template <typename ValueT, typename = std::enable_if_t<std::is_arithmetic_v<ValueT>>>
    ValueT get(const char* key, ValueT dfl) const noexcept
    {
        auto it = map_.find(key);
        return it != map_.end() ? numericCast<ValueT>(it->second.c_str()) : dfl;
    }
    void clear() noexcept { map_.clear(); }
    void read(std::istream& is);
    void set(std::string key, std::string val) { map_.emplace(std::move(key), std::move(val)); }

  private:
    boost::container::flat_map<std::string, std::string> map_;
};

} // namespace swirly

#endif // SWIRLY_UTIL_CONFIG_HPP

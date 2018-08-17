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

#include <swirly/util/String.hpp>
#include <swirly/util/TypeTraits.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <boost/container/flat_map.hpp>
#pragma GCC diagnostic pop

#include <string>
#include <tuple>

namespace swirly {
inline namespace util {

template <typename FnT>
std::istream& parse_section(std::istream& is, FnT fn, std::string* name = nullptr)
{
    std::string line;
    while (std::getline(is, line)) {

        trim(line);

        // Ignore empty lines and comments.

        if (line.empty() || line[0] == '#') {
            continue;
        }

        if (line.front() == '[' && line.back() == ']') {
            if (name) {
                name->assign(line, 1, line.size() - 2);
                trim(*name);
            }
            return is;
        }

        std::string key, val;
        std::tie(key, val) = split_pair(line, '=');
        rtrim(key);
        ltrim(val);

        fn(key, val);
    }
    if (name) {
        name->clear();
    }
    return is;
}

/**
 * Simple config reader with environment variable substitution.
 */
class SWIRLY_API Config {
  public:
    Config();
    ~Config();

    // Copy.
    Config(const Config&);
    Config& operator=(const Config&);

    // Move.
    Config(Config&&);
    Config& operator=(Config&&);

    /**
     * Throws std::runtime_error if key does not exist.
     */
    const std::string& get(const std::string& key) const;
    const char* get(const std::string& key, const char* dfl) const noexcept;
    const char* get(const std::string& key, std::nullptr_t) const noexcept
    {
        return get(key, static_cast<const char*>(nullptr));
    }
    /**
     * Throws std::runtime_error if key does not exist.
     */
    template <typename ValueT>
    ValueT get(const std::string& key) const
    {
        auto it = map_.find(key);
        if (it != map_.end()) {
            if constexpr (std::is_same_v<ValueT, std::string_view>) {
                // Explicitly allow conversion to std::string_view in this case, because we know
                // that the argument is not a temporary.
                return from_string<ValueT>(std::string_view{it->second});
            } else {
                return from_string<ValueT>(it->second);
            }
        }
        if (!parent_) {
            throw std::runtime_error{"missing config key: "s + key};
        }
        return parent_->get<ValueT>(key);
    }
    template <typename ValueT>
    ValueT get(const std::string& key, ValueT dfl) const noexcept
    {
        auto it = map_.find(key);
        if (it != map_.end()) {
            if constexpr (std::is_same_v<ValueT, std::string_view>) {
                // Explicitly allow conversion to std::string_view in this case, because we know
                // that the argument is not a temporary.
                return from_string<ValueT>(std::string_view{it->second});
            } else {
                return from_string<ValueT>(it->second);
            }
        }
        return parent_ ? parent_->get<ValueT>(key, dfl) : dfl;
    }
    std::size_t size() const noexcept { return map_.size(); }
    void clear() noexcept { map_.clear(); }
    std::istream& read_section(std::istream& is) { return read_section(is, nullptr); }
    std::istream& read_section(std::istream& is, std::string& next)
    {
        return read_section(is, &next);
    }
    void set(std::string key, std::string val) { map_.emplace(std::move(key), std::move(val)); }
    void set_parent(Config& parent) noexcept { parent_ = &parent; }

  private:
    std::istream& read_section(std::istream& is, std::string* next);
    boost::container::flat_map<std::string, std::string> map_;
    Config* parent_{nullptr};
};

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_CONFIG_HPP

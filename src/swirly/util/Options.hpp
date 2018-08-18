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
#ifndef SWIRLY_UTIL_OPTIONS_HPP
#define SWIRLY_UTIL_OPTIONS_HPP

#include <swirly/util/String.hpp>

#include <functional>
#include <map>
#include <variant>
#include <vector>

namespace swirly {
inline namespace util {

class Options;

class NoOp {
};

class Help {
};

template <typename CRTP>
class Presence {
  public:
    enum Type { Optional, Required };

    CRTP& required()
    {
        presence_ = Required;
        return *static_cast<CRTP*>(this);
    }

    Type presence() const noexcept { return presence_; }

  private:
    Type presence_{Optional};
};

class Value : public Presence<Value> {

  public:
    template <typename VarT>
    explicit Value(VarT& var)
    : func_{[&var](std::string_view arg) { var = from_string<VarT>(arg); }}
    {
    }

    template <typename VarT, typename AllocT>
    explicit Value(std::vector<VarT, AllocT>& var)
    : func_{[&var](std::string_view arg) { var.push_back(from_string<VarT>(arg)); }}
    {
    }

    template <typename ValueT>
    Value& default_value(const ValueT& value)
    {
        func_(std::to_string(value));
        return *this;
    }

    Value& multitoken()
    {
        multitoken_ = true;
        return *this;
    }

  private:
    friend class Options;

    void run(std::string_view arg)
    {
        func_(arg);
        set_ = true;
    }

    bool set() const noexcept { return set_; }

    std::function<void(std::string_view)> func_;
    bool set_{false};
    bool multitoken_{false};
};

class Switch : public Presence<Switch> {
  public:
    Switch(bool& flag)
    : flag_{flag}
    {
    }

  private:
    friend class Options;

    void run() { flag_ = true; }

    bool& flag_;
};

struct OptionKey {
    char short_opt{0};
    std::string long_opt;
};

inline bool operator<(const OptionKey& lhs, const OptionKey& rhs)
{
    // clang-format off
    return (lhs.short_opt != 0 && lhs.short_opt == rhs.short_opt)
        || (!lhs.long_opt.empty() && lhs.long_opt == rhs.long_opt)
        ? false
        : lhs.short_opt != rhs.short_opt ? lhs.short_opt < rhs.short_opt
                                         : lhs.long_opt < rhs.long_opt;
    // clang-format on
}
inline bool operator<(const OptionKey& lhs, const std::string& rhs)
{
    return lhs.long_opt < rhs;
}
inline bool operator<(const std::string& lhs, const OptionKey& rhs)
{
    return lhs < rhs.long_opt;
}
inline bool operator<(const OptionKey& lhs, std::string_view rhs)
{
    return lhs.long_opt < rhs;
}
inline bool operator<(std::string_view lhs, const OptionKey& rhs)
{
    return lhs < rhs.long_opt;
}
inline bool operator<(const OptionKey& lhs, char rhs)
{
    return lhs.short_opt < rhs;
}
inline bool operator<(char lhs, const OptionKey& rhs)
{
    return lhs < rhs.short_opt;
}

class SWIRLY_API Options {

    struct DataDesc {
        template <typename DataT>
        DataDesc(DataT data, std::string description)
        : data_{std::move(data)}
        , description_{std::move(description)}
        {
        }

        std::variant<Value, Help, NoOp, Switch> data_;
        std::string description_;
    };

  public:
    explicit Options(std::string description = "");

    template <typename DataT>
    Options& operator()(const std::string& long_opt, DataT&& option_data,
                        std::string description = "")
    {
        this->operator()(0, long_opt, std::forward<DataT>(option_data), std::move(description));
        return *this;
    }

    template <typename DataT>
    Options& operator()(char short_opt, const std::string& long_opt, DataT&& option_data,
                        std::string description = "")
    {
        const auto& result
            = opts_.emplace(OptionKey{short_opt, long_opt},
                            DataDesc{std::forward<DataT>(option_data), std::move(description)});
        if (!result.second) {
            throw std::runtime_error("attempting to register duplicate option " + long_opt);
        }
        return *this;
    }

    template <typename DataT>
    Options& operator()(DataT&& option_data, std::string description = "")
    {
        positional_.emplace_back(std::forward<DataT>(option_data), std::move(description));
        return *this;
    }

    template <typename DataT>
    Options& operator()(char short_opt, DataT&& option_data, std::string description = "")
    {
        this->operator()(short_opt, "", std::forward<DataT>(option_data), std::move(description));
        return *this;
    }

    bool operator[](const std::string& long_opt) const noexcept;
    bool operator[](char short_opt) const noexcept;
    void parse(int argc, char* argv[]);

  private:
    friend std::ostream& operator<<(std::ostream& out, const Options& options);

    std::string description_;
    using OptsMap = std::map<OptionKey, DataDesc, std::less<>>;
    OptsMap opts_;
    using OptsVec = std::vector<DataDesc>;
    OptsVec positional_;
};

SWIRLY_API std::ostream& operator<<(std::ostream& out, const Options& options);

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_OPTIONS_HPP

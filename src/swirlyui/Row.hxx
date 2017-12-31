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
#ifndef SWIRLYUI_ROW_HXX
#define SWIRLYUI_ROW_HXX

#include <cstdint>

namespace swirly {
namespace ui {

template <typename ValueT>
class Row {
  public:
    explicit Row(std::uint64_t tag = 0, const ValueT& value = ValueT{}) noexcept
      : value_{value}
    {
        tag_ = tag;
        checked_ = 0;
    }
    ~Row() noexcept = default;

    std::uint64_t tag() const noexcept { return tag_; }
    bool checked() const noexcept { return checked_ == 1; }
    const ValueT& value() const noexcept { return value_; }

    void setTag(std::uint64_t tag) noexcept { tag_ = tag; }
    void setChecked(bool checked = true) noexcept { checked_ = checked ? 1 : 0; }
    void setValue(const ValueT& value) noexcept { value_ = value; }

  private:
    struct {
        std::uint64_t tag_ : 63;
        std::uint64_t checked_ : 1;
    };
    ValueT value_;
};

} // namespace ui
} // namespace swirly

#endif // SWIRLYUI_ROW_HXX

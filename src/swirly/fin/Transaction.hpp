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
#ifndef SWIRLY_FIN_TRANSACTION_HPP
#define SWIRLY_FIN_TRANSACTION_HPP

#include <swirly/util/Log.hpp>

namespace swirly {
inline namespace fin {

template <typename TypeT>
class BasicTransaction {
  public:
    explicit BasicTransaction(TypeT& target)
    : target_(target)
    {
        target_.begin();
    }
    ~BasicTransaction()
    {
        static_assert(std::is_nothrow_invocable_v<decltype(&TypeT::rollback), TypeT>);
        if (!done_) {
            target_.rollback();
        }
    }

    // Copy.
    BasicTransaction(const BasicTransaction&) = delete;
    BasicTransaction& operator=(const BasicTransaction&) = delete;

    // Move.
    BasicTransaction(BasicTransaction&&) = delete;
    BasicTransaction& operator=(BasicTransaction&&) = delete;

    void commit()
    {
        target_.commit();
        done_ = true;
    }

  private:
    TypeT& target_;
    bool done_{false};
};

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_TRANSACTION_HPP

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
#ifndef SWIRLY_DB_CTX_HPP
#define SWIRLY_DB_CTX_HPP

#include <swirly/Config.h>

#include <memory>

namespace swirly {
inline namespace fin {
class Journ;
class Model;
} // namespace fin
inline namespace util {
class Config;
} // namespace util
inline namespace db {

class SWIRLY_API DbCtx {
  public:
    struct Impl {
        virtual ~Impl();
        virtual Model& do_model() = 0;
        virtual Journ& do_journ() = 0;
    };
    DbCtx(const Config& config);
    ~DbCtx();

    // Copy.
    DbCtx(const DbCtx&) = delete;
    DbCtx& operator=(const DbCtx&) = delete;

    // Move.
    DbCtx(DbCtx&&) noexcept = default;
    DbCtx& operator=(DbCtx&&) noexcept = default;

    Model& model() const { return impl_->do_model(); }
    Journ& journ() const { return impl_->do_journ(); }

  private:
    std::unique_ptr<Impl> impl_;
};

} // namespace db
} // namespace swirly

#endif // SWIRLY_DB_CTX_HPP

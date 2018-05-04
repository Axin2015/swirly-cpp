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
#ifndef SWIRLY_SQLITE_MODEL_HPP
#define SWIRLY_SQLITE_MODEL_HPP

#include <swirly/sqlite/Types.hpp>

#include <swirly/fin/Model.hpp>

namespace swirly {
inline namespace sqlite {

class SWIRLY_API SqlModel : public Model {
  public:
    explicit SqlModel(const Config& config);
    ~SqlModel() override;

    // Copy.
    SqlModel(const SqlModel&) = delete;
    SqlModel& operator=(const SqlModel&) = delete;

    // Move.
    SqlModel(SqlModel&&);
    SqlModel& operator=(SqlModel&&);

  protected:
    void doReadAsset(const ModelCallback<AssetPtr>& cb) const override;

    void doReadInstr(const ModelCallback<InstrPtr>& cb) const override;

    void doReadMarket(const ModelCallback<MarketPtr>& cb) const override;

    void doReadOrder(const ModelCallback<OrderPtr>& cb) const override;

    void doReadExec(Time since, const ModelCallback<ExecPtr>& cb) const override;

    void doReadTrade(const ModelCallback<ExecPtr>& cb) const override;

    void doReadPosn(JDay busDay, const ModelCallback<PosnPtr>& cb) const override;

  private:
    sqlite::DbPtr db_;
};

} // namespace sqlite
} // namespace swirly

#endif // SWIRLY_SQLITE_MODEL_HPP

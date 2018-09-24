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
#ifndef SWIRLY_DB_MYSQLMODEL_HPP
#define SWIRLY_DB_MYSQLMODEL_HPP

#include <swirly/db/Types.hpp>

#include <swirly/fin/Model.hpp>

namespace swirly {
inline namespace db {

class SWIRLY_API MySqlModel : public Model {
  public:
    explicit MySqlModel(const mysql::DbPtr& db);
    explicit MySqlModel(const Config& config);
    ~MySqlModel() override;

    // Copy.
    MySqlModel(const MySqlModel&) = delete;
    MySqlModel& operator=(const MySqlModel&) = delete;

    // Move.
    MySqlModel(MySqlModel&&);
    MySqlModel& operator=(MySqlModel&&);

  protected:
    void do_read_asset(const ModelCallback<AssetPtr>& cb) const override;

    void do_read_instr(const ModelCallback<InstrPtr>& cb) const override;

    void do_read_market(const ModelCallback<MarketPtr>& cb) const override;

    void do_read_order(const ModelCallback<OrderPtr>& cb) const override;

    void do_read_exec(WallTime since, const ModelCallback<ExecPtr>& cb) const override;

    void do_read_trade(const ModelCallback<ExecPtr>& cb) const override;

    void do_read_posn(JDay bus_day, const ModelCallback<PosnPtr>& cb) const override;

  private:
    mysql::DbPtr db_;
};

} // namespace db
} // namespace swirly

#endif // SWIRLY_DB_MYSQLMODEL_HPP

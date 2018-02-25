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
#ifndef SWIRLY_FIN_DSVMODEL_HPP
#define SWIRLY_FIN_DSVMODEL_HPP

#include <swirly/fin/Model.hpp>

namespace swirly {

class SWIRLY_API DsvModel : public Model {
  public:
    DsvModel() noexcept;
    ~DsvModel() noexcept override;

    // Copy.
    constexpr DsvModel(const DsvModel&) noexcept;
    DsvModel& operator=(const DsvModel&) noexcept;

    // Move.
    constexpr DsvModel(DsvModel&&) noexcept;
    DsvModel& operator=(DsvModel&&) noexcept;

  protected:
    void doReadAsset(const ModelCallback<AssetPtr>& cb) const override;

    void doReadInstr(const ModelCallback<InstrPtr>& cb) const override;

    void doReadMarket(const ModelCallback<MarketPtr>& cb) const override;

    void doReadOrder(const ModelCallback<OrderPtr>& cb) const override;

    void doReadExec(Time since, const ModelCallback<ExecPtr>& cb) const override;

    void doReadTrade(const ModelCallback<ExecPtr>& cb) const override;

    void doReadPosn(JDay busDay, const ModelCallback<PosnPtr>& cb) const override;
};

} // namespace swirly

#endif // SWIRLY_FIN_DSVMODEL_HPP

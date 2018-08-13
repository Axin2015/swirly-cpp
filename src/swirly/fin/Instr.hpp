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
#ifndef SWIRLY_FIN_INSTR_HPP
#define SWIRLY_FIN_INSTR_HPP

#include <swirly/fin/Types.hpp>

#include <swirly/util/Set.hpp>

namespace swirly {
inline namespace fin {

/**
 * A specification that stipulates the terms and conditions of sale.
 */
class SWIRLY_API Instr : protected Comparable<Instr> {
  public:
    Instr(Id32 id, Symbol symbol, std::string_view display, Symbol base_asset, Symbol term_ccy,
          int lot_numer, int lot_denom, int tick_numer, int tick_denom, int pip_dp, Lots min_lots,
          Lots max_lots) noexcept;

    ~Instr();

    // Copy.
    Instr(const Instr&);
    Instr& operator=(const Instr&) = delete;

    // Move.
    Instr(Instr&&);
    Instr& operator=(Instr&&) = delete;

    template <typename... ArgsT>
    static InstrPtr make(ArgsT&&... args)
    {
        return std::make_unique<Instr>(std::forward<ArgsT>(args)...);
    }

    void to_dsv(std::ostream& os, char delim = ',') const;
    void to_json(std::ostream& os) const;

    int compare(const Instr& rhs) const noexcept { return symbol_.compare(rhs.symbol_); }
    auto id() const noexcept { return id_; }
    auto symbol() const noexcept { return symbol_; }
    auto display() const noexcept { return +display_; }
    auto base_asset() const noexcept { return base_asset_; }
    auto term_ccy() const noexcept { return term_ccy_; }
    auto lot_numer() const noexcept { return lot_numer_; }
    auto lot_denom() const noexcept { return lot_denom_; }
    auto qty_inc() const noexcept { return qty_inc_; }
    auto tick_numer() const noexcept { return tick_numer_; }
    auto tick_denom() const noexcept { return tick_denom_; }
    auto price_inc() const noexcept { return price_inc_; }
    auto pip_dp() const noexcept { return pip_dp_; }
    auto qty_dp() const noexcept { return qty_dp_; }
    auto price_dp() const noexcept { return price_dp_; }
    auto min_lots() const noexcept { return min_lots_; }
    auto max_lots() const noexcept { return max_lots_; }
    boost::intrusive::set_member_hook<> symbol_hook;

  private:
    const Id32 id_;
    const Symbol symbol_;
    const Display display_;
    const Symbol base_asset_;
    const Symbol term_ccy_;
    const int lot_numer_;
    const int lot_denom_;
    // Transient.
    const double qty_inc_;
    const int tick_numer_;
    const int tick_denom_;
    // Transient.
    const double price_inc_;
    const int pip_dp_;
    // Transient.
    const int qty_dp_;
    // Transient.
    const int price_dp_;
    const Lots min_lots_;
    const Lots max_lots_;
};

inline std::ostream& operator<<(std::ostream& os, const Instr& instr)
{
    instr.to_json(os);
    return os;
}

using InstrSet = SymbolSet<Instr>;

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_INSTR_HPP

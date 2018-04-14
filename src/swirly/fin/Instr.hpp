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
class SWIRLY_API Instr : public Comparable<Instr> {
  public:
    Instr(Id32 id, Symbol symbol, std::string_view display, Symbol baseAsset, Symbol termCcy,
          Symbol broker, int lotNumer, int lotDenom, int tickNumer, int tickDenom, int pipDp,
          Lots minLots, Lots maxLots) noexcept;

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

    void toDsv(std::ostream& os, char delim = ',') const;
    void toJson(std::ostream& os) const;

    int compare(const Instr& rhs) const noexcept { return symbol_.compare(rhs.symbol_); }
    auto id() const noexcept { return id_; }
    auto symbol() const noexcept { return symbol_; }
    auto display() const noexcept { return +display_; }
    auto baseAsset() const noexcept { return baseAsset_; }
    auto termCcy() const noexcept { return termCcy_; }
    auto broker() const noexcept { return broker_; }
    auto lotNumer() const noexcept { return lotNumer_; }
    auto lotDenom() const noexcept { return lotDenom_; }
    auto qtyInc() const noexcept { return qtyInc_; }
    auto tickNumer() const noexcept { return tickNumer_; }
    auto tickDenom() const noexcept { return tickDenom_; }
    auto priceInc() const noexcept { return priceInc_; }
    auto pipDp() const noexcept { return pipDp_; }
    auto qtyDp() const noexcept { return qtyDp_; }
    auto priceDp() const noexcept { return priceDp_; }
    auto minLots() const noexcept { return minLots_; }
    auto maxLots() const noexcept { return maxLots_; }
    boost::intrusive::set_member_hook<> symbolHook;

  private:
    const Id32 id_;
    const Symbol symbol_;
    const Display display_;
    const Symbol baseAsset_;
    const Symbol termCcy_;
    const Symbol broker_;
    const int lotNumer_;
    const int lotDenom_;
    // Transient.
    const double qtyInc_;
    const int tickNumer_;
    const int tickDenom_;
    // Transient.
    const double priceInc_;
    const int pipDp_;
    // Transient.
    const int qtyDp_;
    // Transient.
    const int priceDp_;
    const Lots minLots_;
    const Lots maxLots_;
};

inline std::ostream& operator<<(std::ostream& os, const Instr& instr)
{
    instr.toJson(os);
    return os;
}

using InstrSet = SymbolSet<Instr>;

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_INSTR_HPP

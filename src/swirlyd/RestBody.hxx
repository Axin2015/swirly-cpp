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
#ifndef SWIRLYD_RESTBODY_HXX
#define SWIRLYD_RESTBODY_HXX

#include <swirly/fin/Types.hpp>

#include <swirly/util/Date.hpp>
#include <swirly/util/Symbol.hpp>

namespace swirly {

template <std::size_t MaxN>
struct StringData {

    // Length in the first cache-line.
    // Use int to save space.
    int len;
    char buf[MaxN];
};

template <std::size_t MaxN>
constexpr std::string_view operator+(const StringData<MaxN>& s) noexcept
{
    return {s.buf, static_cast<std::size_t>(s.len)};
}

class SWIRLY_API RestBody {
  public:
    enum : unsigned {
        Symbol = 1 << 0,
        Accnt = 1 << 1,
        Instr = 1 << 2,
        SettlDate = 1 << 3,
        Ref = 1 << 4,
        State = 1 << 5,
        Side = 1 << 6,
        Lots = 1 << 7,
        Ticks = 1 << 8,
        MinLots = 1 << 9,
        LiqInd = 1 << 10,
        Cpty = 1 << 11
    };

    RestBody() noexcept { reset(); }
    ~RestBody();

    // Copy.
    RestBody(const RestBody&) = delete;
    RestBody& operator=(const RestBody&) = delete;

    // Move.
    RestBody(RestBody&&) = delete;
    RestBody& operator=(RestBody&&) = delete;

    unsigned fields() const noexcept { return fields_; }
    swirly::Symbol symbol() const noexcept { return +symbol_; }
    swirly::Symbol accnt() const noexcept { return +accnt_; }
    swirly::Symbol instr() const noexcept { return +instr_; }
    IsoDate settl_date() const noexcept { return settl_date_; }
    std::string_view ref() const noexcept { return +ref_; }
    MarketState state() const noexcept { return state_; }
    swirly::Side side() const noexcept { return side_; }
    swirly::Lots lots() const noexcept { return lots_; }
    swirly::Ticks ticks() const noexcept { return ticks_; }
    swirly::Lots min_lots() const noexcept { return min_lots_; }
    swirly::LiqInd liq_ind() const noexcept { return liq_ind_; }
    swirly::Symbol cpty() const noexcept { return +cpty_; }
    /**
     * Validate fields.
     *
     * @param required Required fields.
     *
     * @param optional Optional fields.
     *
     * @return true if fields are value.
     */
    bool valid(unsigned required, unsigned optional = 0x0) const noexcept
    {
        return (fields_ & required) == required && (fields_ & ~(required | optional)) == 0;
    }
    void reset(bool clear = true) noexcept;

    bool parse(std::string_view buf);

  private:
    int cs_;
    union {
        struct {
            int sign;
            std::size_t digits;
        } num_;
        struct {
            int* len;
            char* buf;
            int max;
        } str_;
    };
    unsigned fields_;

    StringData<MaxSymbol> symbol_;
    StringData<MaxSymbol> accnt_;
    StringData<MaxSymbol> instr_;
    IsoDate settl_date_;
    StringData<MaxRef> ref_;
    MarketState state_;
    swirly::Side side_;
    swirly::Lots lots_;
    swirly::Ticks ticks_;
    swirly::Lots min_lots_;
    swirly::LiqInd liq_ind_;
    StringData<MaxSymbol> cpty_;

    long num() const noexcept { return num_.sign * num_.digits; }
};

} // namespace swirly

#endif // SWIRLYD_RESTBODY_HXX

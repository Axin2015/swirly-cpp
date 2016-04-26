/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
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
#include "Journ.hpp"

using namespace std;

namespace swirly {
namespace sqlite {

Journ::Journ() : sqlite_ {open(":memory:")}
{
}

Journ::~Journ() noexcept = default;

Journ::Journ(Journ&&) = default;

Journ& Journ::operator=(Journ&&) = default;

void Journ::doCreateMarket(string_view mnem, string_view display, string_view contr, Jday settlDay,
                           Jday expiryDay, MarketState state)
{
}

void Journ::doUpdateMarket(string_view mnem, string_view display, MarketState state)
{
}

void Journ::doCreateTrader(string_view mnem, string_view display, string_view email)
{
}

void Journ::doUpdateTrader(string_view mnem, string_view display)
{
}

void Journ::doCreateExec(const Exec& exec)
{
}

void Journ::doCreateExec(string_view market, ArrayView<ConstExecPtr> execs)
{
}

void Journ::doCreateExec(ArrayView<ConstExecPtr> execs)
{
}

void Journ::doArchiveOrder(string_view market, ArrayView<Iden> ids, Millis modified)
{
}

void Journ::doArchiveTrade(string_view market, ArrayView<Iden> ids, Millis modified)
{
}

} // sqlite
} // swirly

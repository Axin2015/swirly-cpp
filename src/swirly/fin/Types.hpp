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
#ifndef SWIRLY_FIN_TYPES_HPP
#define SWIRLY_FIN_TYPES_HPP

#include <swirly/fin/BasicTypes.hpp>
#include <swirly/fin/Limits.hpp>

#include <swirly/util/StringBuf.hpp>

#include <boost/intrusive_ptr.hpp>

#include <memory>

namespace swirly {

/**
 * Description suitable for display on user-interface.
 */
using Display = StringBuf<MaxDisplay>;

/**
 * Reference.
 */
using Ref = StringBuf<MaxRef>;

class Asset;
using AssetPtr = std::unique_ptr<Asset>;
using ConstAssetPtr = std::unique_ptr<const Asset>;

class Instr;
using InstrPtr = std::unique_ptr<Instr>;
using ConstInstrPtr = std::unique_ptr<const Instr>;

class Market;
using MarketPtr = boost::intrusive_ptr<Market>;
using ConstMarketPtr = boost::intrusive_ptr<const Market>;

class Market;
using MarketPtr = boost::intrusive_ptr<Market>;
using ConstMarketPtr = boost::intrusive_ptr<const Market>;

class Request;
using RequestPtr = boost::intrusive_ptr<Request>;
using ConstRequestPtr = boost::intrusive_ptr<const Request>;

class Exec;
using ExecPtr = boost::intrusive_ptr<Exec>;
using ConstExecPtr = boost::intrusive_ptr<const Exec>;

class Order;
using OrderPtr = boost::intrusive_ptr<Order>;
using ConstOrderPtr = boost::intrusive_ptr<const Order>;

class Posn;
using PosnPtr = boost::intrusive_ptr<Posn>;
using ConstPosnPtr = boost::intrusive_ptr<const Posn>;

enum class More : int { No, Yes };

} // namespace swirly

#endif // SWIRLY_FIN_TYPES_HPP

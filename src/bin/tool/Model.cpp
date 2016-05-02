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
#include <swirly/elm/Factory.hpp>
#include <swirly/elm/Model.hpp>

#include <swirly/ash/Log.hpp>

#include <iostream>

using namespace std;
using namespace swirly;

int main(int argc, char* argv[])
{
  int ret = 1;
  try {

    const BasicFactory factory{};
    auto model = makeModel(argc > 1 ? argv[1] : "swirly.db");

    cout << "{\"assets\":[";
    auto assets = model->readAsset(factory);
    copy(assets.begin(), assets.end(), OStreamJoiner(cout, ','));

    cout << "],\"contrs\":[";
    auto contrs = model->readContr(factory);
    copy(contrs.begin(), contrs.end(), OStreamJoiner(cout, ','));

    cout << "],\"markets\":[";
    auto markets = model->readMarket(factory);
    copy(markets.begin(), markets.end(), OStreamJoiner(cout, ','));

    cout << "],\"traders\":[";
    auto traders = model->readTrader(factory);
    copy(traders.begin(), traders.end(), OStreamJoiner(cout, ','));

    cout << "]}\n";

    ret = 0;
  } catch (const exception& e) {
    cerr << "exception: " << e.what() << endl;
  }
  return ret;
}

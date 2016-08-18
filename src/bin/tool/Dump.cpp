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
#include <swirly/elm/Asset.hpp>
#include <swirly/elm/Contr.hpp>
#include <swirly/elm/Date.hpp>
#include <swirly/elm/Exec.hpp>
#include <swirly/elm/Market.hpp>
#include <swirly/elm/Model.hpp>
#include <swirly/elm/Order.hpp>
#include <swirly/elm/Posn.hpp>

#include <swirly/ash/Conf.hpp>
#include <swirly/ash/Stream.hpp>
#include <swirly/ash/Time.hpp>

#include <iostream>

using namespace std;
using namespace swirly;

int main(int argc, char* argv[])
{
  int ret = 1;
  try {

    Conf conf;
    if (argc > 1) {
      conf.set("sqlite_model", argv[1]);
    }
    const BusinessDay busDay{RollHour, NewYork};

    const auto now = getTimeOfDay();
    auto model = makeModel(conf);

    cout << "{\"assets\":[";
    {
      OStreamJoiner it(cout, ',');
      model->readAsset([&it](auto ptr) { it = *ptr; });
    }
    cout << "],\"contrs\":[";
    {
      OStreamJoiner it(cout, ',');
      model->readContr([&it](auto ptr) { it = *ptr; });
    }
    cout << "],\"markets\":[";
    {
      OStreamJoiner it(cout, ',');
      model->readMarket([&it](MarketPtr ptr) { it = *ptr; });
    }
    cout << "],\"orders\":[";
    {
      OStreamJoiner it(cout, ',');
      model->readOrder([&it](auto ptr) { it = *ptr; });
    }
    cout << "],\"execs\":[";
    {
      OStreamJoiner it(cout, ',');
      model->readAccnt(now, [&model, &it](auto mnem) {
        model->readExec(mnem, 1 << 4, [&it](auto ptr) { it = *ptr; });
      });
    }
    cout << "],\"trades\":[";
    {
      OStreamJoiner it(cout, ',');
      model->readTrade([&it](auto ptr) { it = *ptr; });
    }
    cout << "],\"posns\":[";
    {
      OStreamJoiner it(cout, ',');
      model->readPosn(busDay(now), [&it](auto ptr) { it = *ptr; });
    }
    cout << "]}\n";

    ret = 0;
  } catch (const exception& e) {
    cerr << "exception: " << e.what() << endl;
  }
  return ret;
}

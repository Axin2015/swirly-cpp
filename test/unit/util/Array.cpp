/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
#include <swirly/util/Array.hpp>

#include <swirly/unit/Test.hpp>

#include <algorithm> // equal()
#include <numeric> // accumulate()

using namespace std;
using namespace swirly;

SWIRLY_TEST_CASE(ArrayView)
{
  SWIRLY_CHECK(!ArrayView<int>{}.data());
  SWIRLY_CHECK(ArrayView<int>{}.empty());
  SWIRLY_CHECK(ArrayView<int>{}.size() == 0UL);

  const int arr[] = {101, 202, 303};
  ArrayView<int> av{arr};

  SWIRLY_CHECK(av.data());
  SWIRLY_CHECK(!av.empty());
  SWIRLY_CHECK(av.size() == 3UL);

  SWIRLY_CHECK(av[0] == arr[0]);
  SWIRLY_CHECK(av[1] == arr[1]);
  SWIRLY_CHECK(av[2] == arr[2]);

  SWIRLY_CHECK(av.front() == arr[0]);
  SWIRLY_CHECK(av.back() == arr[2]);

  SWIRLY_CHECK(equal(av.begin(), av.end(), arr));
  int rev[] = {303, 202, 101};

  SWIRLY_CHECK(equal(av.rbegin(), av.rend(), rev));

  SWIRLY_CHECK(makeArrayView(arr, 2).size() == 2UL);
  SWIRLY_CHECK(makeArrayView(arr).size() == 3UL);
}

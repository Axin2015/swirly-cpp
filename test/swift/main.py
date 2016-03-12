#!/usr/bin/env python

# The Restful Matching-Engine.
# Copyright (C) 2013, 2016 Swirly Cloud Limited.
#
# This program is free software; you can redistribute it and/or modify it under the terms of the
# GNU General Public License as published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with this program; if
# not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.

# This script is intended as the basis for automated functional testing.

from swift import *

import unittest

class TestSwirly(unittest.TestCase):

  @classmethod
  def setUpClass(cls):
    cls.fixture = Fixture()

  @classmethod
  def tearDownClass(cls):
    cls.fixture.close()
    cls = None

  def test_rec(self):
    with Connection() as conn:
      self.assertEqual(conn.send('POST', '/api/rec/market').status, 200)
      self.assertEqual(conn.send('PUT', '/api/rec/market/EURUSD').status, 200)
      self.assertEqual(conn.send('POST', '/api/rec/trader').status, 200)
      self.assertEqual(conn.send('PUT', '/api/rec/trader/MARAYL').status, 200)

  def test_sess(self):
    with Connection() as conn:
      self.assertEqual(conn.send('POST', '/api/sess/order/EURUSD').status, 200)
      self.assertEqual(conn.send('PUT', '/api/sess/order/EURUSD/1,2,3').status, 200)
      self.assertEqual(conn.send('DELETE', '/api/sess/order/EURUSD/1,2,3').status, 200)
      self.assertEqual(conn.send('POST', '/api/sess/trade/EURUSD').status, 200)
      self.assertEqual(conn.send('DELETE', '/api/sess/trade/EURUSD/1,2,3').status, 200)

if __name__ == '__main__':
  unittest.main()

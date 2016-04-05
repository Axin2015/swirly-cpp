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

class MarketTest(unittest.TestCase):

  @classmethod
  def setUpClass(cls):
    cls.fixture = Fixture()

  @classmethod
  def tearDownClass(cls):
    cls.fixture.close()
    cls = None

  def testPostMarket(self):
    with Connection() as conn:
      resp = conn.send('POST', '/api/rec/market',
                       mnem = 'USDJPY.MAR14',
                       display = 'first',
                       contr = 'USDJPY',
                       settlDate = 20170102,
                       expiryDate = 20170101,
                       state = 1)
      self.assertEqual(resp.status, 200)
      self.assertEqual(resp.reason, 'OK')
      self.assertEqual(resp.content['mnem'], 'USDJPY.MAR14')
      self.assertEqual(resp.content['display'], 'first')
      self.assertEqual(resp.content['contr'], 'USDJPY')
      self.assertEqual(resp.content['settlDate'], 20170102)
      self.assertEqual(resp.content['expiryDate'], 20170101)
      self.assertEqual(resp.content['state'], 1)

  def testPutMarket(self):
    with Connection() as conn:

      # Update display and state.
      resp = conn.send('PUT', '/api/rec/market/USDJPY.MAR14',
                       display = 'second',
                       state = 2)
      self.assertEqual(resp.status, 200)
      self.assertEqual(resp.reason, 'OK')
      self.assertEqual(resp.content['mnem'], 'USDJPY.MAR14')
      self.assertEqual(resp.content['display'], 'second')
      self.assertEqual(resp.content['contr'], 'USDJPY')
      self.assertEqual(resp.content['settlDate'], 20170102)
      self.assertEqual(resp.content['expiryDate'], 20170101)
      self.assertEqual(resp.content['state'], 2)

      # Update display only.
      resp = conn.send('PUT', '/api/rec/market/USDJPY.MAR14',
                       display = 'third',
                       state = None)
      self.assertEqual(resp.status, 200)
      self.assertEqual(resp.reason, 'OK')
      self.assertEqual(resp.content['mnem'], 'USDJPY.MAR14')
      self.assertEqual(resp.content['display'], 'third')
      self.assertEqual(resp.content['contr'], 'USDJPY')
      self.assertEqual(resp.content['settlDate'], 20170102)
      self.assertEqual(resp.content['expiryDate'], 20170101)
      self.assertEqual(resp.content['state'], 2)

      # Update state only.
      resp = conn.send('PUT', '/api/rec/market/USDJPY.MAR14',
                       display = None,
                       state = 3)
      self.assertEqual(resp.status, 200)
      self.assertEqual(resp.reason, 'OK')
      self.assertEqual(resp.content['mnem'], 'USDJPY.MAR14')
      self.assertEqual(resp.content['display'], 'third')
      self.assertEqual(resp.content['contr'], 'USDJPY')
      self.assertEqual(resp.content['settlDate'], 20170102)
      self.assertEqual(resp.content['expiryDate'], 20170101)
      self.assertEqual(resp.content['state'], 3)

class TraderTest(unittest.TestCase):

  @classmethod
  def setUpClass(cls):
    cls.fixture = Fixture()

  @classmethod
  def tearDownClass(cls):
    cls.fixture.close()
    cls = None

  def testPostTrader(self):
    with Connection() as conn:
      resp = conn.send('POST', '/api/rec/trader',
                       mnem = 'MARAYL2',
                       display = 'Mark Aylettx',
                       email = 'mark.aylett@swirlycloud.com')
      self.assertEqual(resp.status, 200)
      self.assertEqual(resp.reason, 'OK')
      self.assertEqual(resp.content['mnem'], 'MARAYL2')
      self.assertEqual(resp.content['display'], 'Mark Aylettx')
      self.assertEqual(resp.content['email'], 'mark.aylett@swirlycloud.com')

  def testPutTrader(self):
    with Connection() as conn:

      resp = conn.send('PUT', '/api/rec/trader/MARAYL2',
                       display = 'Mark Aylett')
      self.assertEqual(resp.status, 200)
      self.assertEqual(resp.reason, 'OK')
      self.assertEqual(resp.content['mnem'], 'MARAYL2')
      self.assertEqual(resp.content['display'], 'Mark Aylett')
      self.assertEqual(resp.content['email'], 'mark.aylett@swirlycloud.com')

class OrderTest(unittest.TestCase):

  @classmethod
  def setUpClass(cls):
    cls.fixture = Fixture()
    with Connection() as conn:
      conn.send('POST', '/api/rec/market',
                mnem = 'USDJPY.MAR14',
                display = 'first',
                contr = 'USDJPY',
                settlDate = 20170102,
                expiryDate = 20170101,
                state = 1)

  @classmethod
  def tearDownClass(cls):
    cls.fixture.close()
    cls = None

  def testPostOrder(self):
    with Connection() as conn:
      conn.login('MARAYL')
      resp = conn.send('POST', '/api/sess/order/USDJPY.MAR14',
                       side = 'BUY',
                       lots = 1,
                       ticks = 12345
      )
      print(resp)
      self.assertEqual(resp.status, 200)
      self.assertEqual(resp.reason, 'OK')

class SwirlyTest(unittest.TestCase):

  @classmethod
  def setUpClass(cls):
    cls.fixture = Fixture()

  @classmethod
  def tearDownClass(cls):
    cls.fixture.close()
    cls = None

  def testSess(self):
    with Connection() as conn:
      #self.assertEqual(conn.send('POST', '/api/sess/order/EURUSD').status, 200)
      self.assertEqual(conn.send('PUT', '/api/sess/order/EURUSD/1,2,3').status, 200)
      self.assertEqual(conn.send('DELETE', '/api/sess/order/EURUSD/1,2,3').status, 200)
      self.assertEqual(conn.send('POST', '/api/sess/trade/EURUSD').status, 200)
      self.assertEqual(conn.send('DELETE', '/api/sess/trade/EURUSD/1,2,3').status, 200)

if __name__ == '__main__':
  unittest.main()

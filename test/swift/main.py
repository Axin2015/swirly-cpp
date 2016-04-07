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
      self.assertEqual(200, resp.status)
      self.assertEqual('OK', resp.reason)
      self.assertDictEqual({
        u'contr': u'USDJPY',
        u'display': u'first',
        u'expiryDate': 20170101,
        u'mnem': u'USDJPY.MAR14',
        u'settlDate': 20170102,
        u'state': 1
      }, resp.content)

  def testPutMarket(self):
    with Connection() as conn:

      # Update display and state.
      resp = conn.send('PUT', '/api/rec/market/USDJPY.MAR14',
                       display = 'second',
                       state = 2)
      self.assertEqual(200, resp.status)
      self.assertEqual('OK', resp.reason)
      self.assertDictEqual({
        u'contr': u'USDJPY',
        u'display': u'second',
        u'expiryDate': 20170101,
        u'mnem': u'USDJPY.MAR14',
        u'settlDate': 20170102,
        u'state': 2
      }, resp.content)

      # Update display only.
      resp = conn.send('PUT', '/api/rec/market/USDJPY.MAR14',
                       display = 'third',
                       state = None)
      self.assertEqual(200, resp.status)
      self.assertEqual('OK', resp.reason)
      self.assertDictEqual({
        u'contr': u'USDJPY',
        u'display': u'third',
        u'expiryDate': 20170101,
        u'mnem': u'USDJPY.MAR14',
        u'settlDate': 20170102,
        u'state': 2
      }, resp.content)

      # Update state only.
      resp = conn.send('PUT', '/api/rec/market/USDJPY.MAR14',
                       display = None,
                       state = 3)
      self.assertEqual(200, resp.status)
      self.assertEqual('OK', resp.reason)
      self.assertDictEqual({
        u'contr': u'USDJPY',
        u'display': u'third',
        u'expiryDate': 20170101,
        u'mnem': u'USDJPY.MAR14',
        u'settlDate': 20170102,
        u'state': 3
      }, resp.content)

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
      self.assertEqual(200, resp.status)
      self.assertEqual('OK', resp.reason)
      self.assertDictEqual({
        u'display': u'Mark Aylettx',
        u'email': u'mark.aylett@swirlycloud.com',
        u'mnem': u'MARAYL2'
      }, resp.content)

  def testPutTrader(self):
    with Connection() as conn:

      resp = conn.send('PUT', '/api/rec/trader/MARAYL2',
                       display = 'Mark Aylett')
      self.assertEqual(200, resp.status)
      self.assertEqual('OK', resp.reason)
      self.assertDictEqual({
        u'display': u'Mark Aylett',
        u'email': u'mark.aylett@swirlycloud.com',
        u'mnem': u'MARAYL2'
      }, resp.content)

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
    self.maxDiff = None
    with Connection() as conn:
      conn.setUser('MARAYL')
      conn.setTime(1459974268204)
      resp = conn.send('POST', '/api/sess/order/USDJPY.MAR14',
                       side = 'BUY',
                       lots = 1,
                       ticks = 12345
      )
      self.assertEqual(200, resp.status)
      self.assertEqual('OK', resp.reason)
      self.assertDictEqual({
        u'execs': [{
          u'contr': u'USDJPY',
          u'cost': 0,
          u'cpty': None,
          u'created': 1459974268204,
          u'exec': 0,
          u'id': 1,
          u'lastLots': None,
          u'lastTicks': None,
          u'lots': 1,
          u'market': u'USDJPY.MAR14',
          u'matchId': None,
          u'minLots': None,
          u'orderId': 1,
          u'ref': None,
          u'resd': 1,
          u'role': None,
          u'settlDate': 20170102,
          u'side': u'BUY',
          u'state': u'NEW',
          u'ticks': 12345,
          u'trader': u'MARAYL'
        }],
        u'orders': [{
          u'contr': u'USDJPY',
          u'cost': 0,
          u'created': 1459974268204,
          u'exec': 0,
          u'id': 1,
          u'lastLots': None,
          u'lastTicks': None,
          u'lots': 1,
          u'market': u'USDJPY.MAR14',
          u'minLots': None,
          u'modified': 1459974268204,
          u'ref': None,
          u'resd': 1,
          u'settlDate': 20170102,
          u'side': u'BUY',
          u'state': u'NEW',
          u'ticks': 12345,
          u'trader': u'MARAYL'
        }],
        u'posn': None,
        u'view': {
          u'bidCount': [1, None, None],
          u'bidResd': [1, None, None],
          u'bidTicks': [12345, None, None],
          u'contr': u'USDJPY',
          u'lastLots': None,
          u'lastTicks': None,
          u'lastTime': None,
          u'market': u'USDJPY.MAR14',
          u'offerCount': [None, None, None],
          u'offerResd': [None, None, None],
          u'offerTicks': [None, None, None],
          u'settlDate': 20170102
        }
      }, resp.content)

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
  unittest.TestCase.maxDiff = None
  unittest.main()

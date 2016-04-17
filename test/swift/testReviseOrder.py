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

from swift import *

import unittest

class TestCase(unittest.TestCase):

  def test(self):
    self.maxDiff = None
    self.now = 1459974268204
    with Fixture() as fixture:
      with Connection() as conn:
        self.createMarket(conn)
        self.createOrder(conn)
        self.reviseDown(conn)

  def createMarket(self, conn):
    conn.setTime(self.now)
    conn.setUser('MARAYL')
    resp = conn.send('POST', '/api/rec/market',
                     mnem = 'USDJPY.MAR14',
                     display = 'first',
                     contr = 'USDJPY',
                     settlDate = 20170102,
                     expiryDate = 20170101,
                     state = 1)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)

  def createOrder(self, conn):
    conn.setTime(self.now)
    conn.setUser('MARAYL')
    resp = conn.send('POST', '/api/sess/order/USDJPY.MAR14',
                     side = 'BUY',
                     lots = 5,
                     ticks = 12345)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'execs': [{
        u'contr': u'USDJPY',
        u'cost': 0,
        u'cpty': None,
        u'created': self.now,
        u'exec': 0,
        u'id': 1,
        u'lastLots': None,
        u'lastTicks': None,
        u'lots': 5,
        u'market': u'USDJPY.MAR14',
        u'matchId': None,
        u'minLots': None,
        u'orderId': 1,
        u'ref': None,
        u'resd': 5,
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
        u'created': self.now,
        u'exec': 0,
        u'id': 1,
        u'lastLots': None,
        u'lastTicks': None,
        u'lots': 5,
        u'market': u'USDJPY.MAR14',
        u'minLots': None,
        u'modified': self.now,
        u'ref': None,
        u'resd': 5,
        u'settlDate': 20170102,
        u'side': u'BUY',
        u'state': u'NEW',
        u'ticks': 12345,
        u'trader': u'MARAYL'
      }],
      u'posn': None,
      u'view': {
        u'bidCount': [1, None, None],
        u'bidResd': [5, None, None],
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

  def reviseDown(self, conn):
    conn.setTime(self.now)
    conn.setUser('MARAYL')
    resp = conn.send('PUT', '/api/sess/order/USDJPY.MAR14/1',
                     side = 'SELL',
                     lots = 4,
                     ticks = 12345)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'execs': [{
        u'contr': u'USDJPY',
        u'cost': 0,
        u'cpty': None,
        u'created': self.now,
        u'exec': 0,
        u'id': 2,
        u'lastLots': None,
        u'lastTicks': None,
        u'lots': 4,
        u'market': u'USDJPY.MAR14',
        u'matchId': None,
        u'minLots': None,
        u'orderId': 1,
        u'ref': None,
        u'resd': 4,
        u'role': None,
        u'settlDate': 20170102,
        u'side': u'BUY',
        u'state': u'REVISE',
        u'ticks': 12345,
        u'trader': u'MARAYL'
      }],
      u'orders': [{
        u'contr': u'USDJPY',
        u'cost': 0,
        u'created': self.now,
        u'exec': 0,
        u'id': 1,
        u'lastLots': None,
        u'lastTicks': None,
        u'lots': 4,
        u'market': u'USDJPY.MAR14',
        u'minLots': None,
        u'modified': self.now,
        u'ref': None,
        u'resd': 4,
        u'settlDate': 20170102,
        u'side': u'BUY',
        u'state': u'REVISE',
        u'ticks': 12345,
        u'trader': u'MARAYL'
      }],
      u'posn': None,
      u'view': {
        u'bidCount': [1, None, None],
        u'bidResd': [4, None, None],
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

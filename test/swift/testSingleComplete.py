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

class TestCase(RestTestCase):

  def test(self):
    self.maxDiff = None
    self.now = 1388534400000
    with Fixture() as fixture:
      with Connection() as conn:
        conn.setTime(self.now)

        conn.setAuth('ADMIN', 0x1)
        self.createMarket(conn, 'EURUSD.MAR14', 'EURUSD', 20140302, 20140301)

        conn.setAuth('MARAYL', 0x2)
        self.createOrder(conn, 'EURUSD.MAR14', 'BUY', 5, 12345)

        conn.setAuth('GOSAYL', 0x2)
        self.takeOrder(conn)

        conn.setAuth('MARAYL', 0x2)
        self.makerOrder(conn)
        self.makerExec(conn)
        self.makerTrade(conn)
        self.makerPosn(conn)

  def takeOrder(self, conn):
    resp = conn.send('POST', '/accnt/order/EURUSD.MAR14',
                     side = 'SELL',
                     lots = 5,
                     ticks = 12345)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'execs': [{
        u'accnt': u'GOSAYL',
        u'contr': u'EURUSD',
        u'cost': 0,
        u'cpty': None,
        u'created': self.now,
        u'exec': 0,
        u'id': 2,
        u'lastLots': None,
        u'lastTicks': None,
        u'lots': 5,
        u'market': u'EURUSD.MAR14',
        u'matchId': None,
        u'minLots': None,
        u'orderId': 2,
        u'ref': None,
        u'resd': 5,
        u'liqInd': None,
        u'settlDate': 20140302,
        u'side': u'SELL',
        u'state': u'NEW',
        u'ticks': 12345
      }, {
        u'accnt': u'GOSAYL',
        u'contr': u'EURUSD',
        u'cost': 61725,
        u'cpty': u'MARAYL',
        u'created': self.now,
        u'exec': 5,
        u'id': 4,
        u'lastLots': 5,
        u'lastTicks': 12345,
        u'lots': 5,
        u'market': u'EURUSD.MAR14',
        u'matchId': 3,
        u'minLots': None,
        u'orderId': 2,
        u'ref': None,
        u'resd': 0,
        u'liqInd': u'TAKER',
        u'settlDate': 20140302,
        u'side': u'SELL',
        u'state': u'TRADE',
        u'ticks': 12345
      }],
      u'orders': [{
        u'accnt': u'GOSAYL',
        u'contr': u'EURUSD',
        u'cost': 61725,
        u'created': self.now,
        u'exec': 5,
        u'id': 2,
        u'lastLots': 5,
        u'lastTicks': 12345,
        u'lots': 5,
        u'market': u'EURUSD.MAR14',
        u'minLots': None,
        u'modified': self.now,
        u'ref': None,
        u'resd': 0,
        u'settlDate': 20140302,
        u'side': u'SELL',
        u'state': u'TRADE',
        u'ticks': 12345
      }],
      u'posn': {
        u'accnt': u'GOSAYL',
        u'buyCost': 0,
        u'buyLots': 0,
        u'contr': u'EURUSD',
        u'sellCost': 61725,
        u'sellLots': 5,
        u'settlDate': 20140302
      },
      u'view': {
        u'bidCount': [None, None, None],
        u'bidResd': [None, None, None],
        u'bidTicks': [None, None, None],
        u'contr': u'EURUSD',
        u'lastLots': 5,
        u'lastTicks': 12345,
        u'lastTime': self.now,
        u'market': u'EURUSD.MAR14',
        u'offerCount': [None, None, None],
        u'offerResd': [None, None, None],
        u'offerTicks': [None, None, None],
        u'settlDate': 20140302
      }
    }, resp.content)

  def makerOrder(self, conn):
    resp = conn.send('GET', '/accnt/order')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([], resp.content)

  def makerExec(self, conn):
    resp = conn.send('GET', '/accnt/exec')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 61725,
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec': 5,
      u'id': 3,
      u'lastLots': 5,
      u'lastTicks': 12345,
      u'liqInd': u'MAKER',
      u'lots': 5,
      u'market': u'EURUSD.MAR14',
      u'matchId': 4,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resd': 0,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12345
    }, {
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 1,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 5,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resd': 5,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'NEW',
      u'ticks': 12345
    }], resp.content)

  def makerTrade(self, conn):
    resp = conn.send('GET', '/accnt/trade')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 61725,
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec': 5,
      u'id': 3,
      u'lastLots': 5,
      u'lastTicks': 12345,
      u'lots': 5,
      u'market': u'EURUSD.MAR14',
      u'matchId': 4,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resd': 0,
      u'liqInd': u'MAKER',
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12345
    }], resp.content)

  def makerPosn(self, conn):
    resp = conn.send('GET', '/accnt/posn')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'buyCost': 61725,
      u'buyLots': 5,
      u'contr': u'EURUSD',
      u'sellCost': 0,
      u'sellLots': 0,
      u'settlDate': 20140302
    }], resp.content)

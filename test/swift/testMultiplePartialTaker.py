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
    self.now = 1459974268204
    with Fixture() as fixture:
      with Connection() as conn:
        conn.setTime(self.now)

        conn.setAuth('ADMIN', 0x1)
        self.createMarket(conn, 'EURUSD.MAR14', 'EURUSD', 20170102, 20170101)

        conn.setAuth('MARAYL', 0x2)
        self.createOrder(conn, 'EURUSD.MAR14', 'BUY', 3, 12345)
        self.createOrder(conn, 'EURUSD.MAR14', 'BUY', 5, 12344)
        self.createOrder(conn, 'EURUSD.MAR14', 'BUY', 7, 12344)

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
                     lots = 17,
                     ticks = 12344)

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
        u'id': 4,
        u'lastLots': None,
        u'lastTicks': None,
        u'liqInd': None,
        u'lots': 17,
        u'market': u'EURUSD.MAR14',
        u'matchId': None,
        u'minLots': None,
        u'orderId': 4,
        u'ref': None,
        u'resd': 17,
        u'settlDate': 20170102,
        u'side': u'SELL',
        u'state': u'NEW',
        u'ticks': 12344
      }, {
        u'accnt': u'GOSAYL',
        u'contr': u'EURUSD',
        u'cost': 37035,
        u'cpty': u'MARAYL',
        u'created': self.now,
        u'exec': 3,
        u'id': 6,
        u'lastLots': 3,
        u'lastTicks': 12345,
        u'liqInd': u'TAKER',
        u'lots': 17,
        u'market': u'EURUSD.MAR14',
        u'matchId': 5,
        u'minLots': None,
        u'orderId': 4,
        u'ref': None,
        u'resd': 14,
        u'settlDate': 20170102,
        u'side': u'SELL',
        u'state': u'TRADE',
        u'ticks': 12344
      }, {
        u'accnt': u'GOSAYL',
        u'contr': u'EURUSD',
        u'cost': 98755,
        u'cpty': u'MARAYL',
        u'created': self.now,
        u'exec': 8,
        u'id': 8,
        u'lastLots': 5,
        u'lastTicks': 12344,
        u'liqInd': u'TAKER',
        u'lots': 17,
        u'market': u'EURUSD.MAR14',
        u'matchId': 7,
        u'minLots': None,
        u'orderId': 4,
        u'ref': None,
        u'resd': 9,
        u'settlDate': 20170102,
        u'side': u'SELL',
        u'state': u'TRADE',
        u'ticks': 12344
      }, {
        u'accnt': u'GOSAYL',
        u'contr': u'EURUSD',
        u'cost': 185163,
        u'cpty': u'MARAYL',
        u'created': self.now,
        u'exec': 15,
        u'id': 10,
        u'lastLots': 7,
        u'lastTicks': 12344,
        u'liqInd': u'TAKER',
        u'lots': 17,
        u'market': u'EURUSD.MAR14',
        u'matchId': 9,
        u'minLots': None,
        u'orderId': 4,
        u'ref': None,
        u'resd': 2,
        u'settlDate': 20170102,
        u'side': u'SELL',
        u'state': u'TRADE',
        u'ticks': 12344
      }],
      u'orders': [{
        u'accnt': u'GOSAYL',
        u'contr': u'EURUSD',
        u'cost': 185163,
        u'created': self.now,
        u'exec': 15,
        u'id': 4,
        u'lastLots': 7,
        u'lastTicks': 12344,
        u'lots': 17,
        u'market': u'EURUSD.MAR14',
        u'minLots': None,
        u'modified': self.now,
        u'ref': None,
        u'resd': 2,
        u'settlDate': 20170102,
        u'side': u'SELL',
        u'state': u'TRADE',
        u'ticks': 12344
      }],
      u'posn': {
        u'accnt': u'GOSAYL',
        u'buyCost': 0,
        u'buyLots': 0,
        u'contr': u'EURUSD',
        u'sellCost': 185163,
        u'sellLots': 15,
        u'settlDate': 20170102
      },
      u'view': {
        u'bidCount': [None, None, None],
        u'bidResd': [None, None, None],
        u'bidTicks': [None, None, None],
        u'contr': u'EURUSD',
        u'lastLots': 7,
        u'lastTicks': 12344,
        u'lastTime': self.now,
        u'market': u'EURUSD.MAR14',
        u'offerCount': [1, None, None],
        u'offerResd': [2, None, None],
        u'offerTicks': [12344, None, None],
        u'settlDate': 20170102
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
      u'cost': 86408,
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec': 7,
      u'id': 9,
      u'lastLots': 7,
      u'lastTicks': 12344,
      u'liqInd': u'MAKER',
      u'lots': 7,
      u'market': u'EURUSD.MAR14',
      u'matchId': 10,
      u'minLots': None,
      u'orderId': 3,
      u'ref': None,
      u'resd': 0,
      u'settlDate': 20170102,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12344
    }, {
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 61720,
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec': 5,
      u'id': 7,
      u'lastLots': 5,
      u'lastTicks': 12344,
      u'liqInd': u'MAKER',
      u'lots': 5,
      u'market': u'EURUSD.MAR14',
      u'matchId': 8,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resd': 0,
      u'settlDate': 20170102,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12344
    }, {
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 37035,
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec': 3,
      u'id': 5,
      u'lastLots': 3,
      u'lastTicks': 12345,
      u'liqInd': u'MAKER',
      u'lots': 3,
      u'market': u'EURUSD.MAR14',
      u'matchId': 6,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resd': 0,
      u'settlDate': 20170102,
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
      u'id': 3,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 7,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 3,
      u'ref': None,
      u'resd': 7,
      u'settlDate': 20170102,
      u'side': u'BUY',
      u'state': u'NEW',
      u'ticks': 12344
    }, {
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 2,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 5,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resd': 5,
      u'settlDate': 20170102,
      u'side': u'BUY',
      u'state': u'NEW',
      u'ticks': 12344
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
      u'lots': 3,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resd': 3,
      u'settlDate': 20170102,
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
      u'cost': 37035,
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec': 3,
      u'id': 5,
      u'lastLots': 3,
      u'lastTicks': 12345,
      u'liqInd': u'MAKER',
      u'lots': 3,
      u'market': u'EURUSD.MAR14',
      u'matchId': 6,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resd': 0,
      u'settlDate': 20170102,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12345
    }, {
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 61720,
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec': 5,
      u'id': 7,
      u'lastLots': 5,
      u'lastTicks': 12344,
      u'liqInd': u'MAKER',
      u'lots': 5,
      u'market': u'EURUSD.MAR14',
      u'matchId': 8,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resd': 0,
      u'settlDate': 20170102,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12344
    }, {
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 86408,
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec': 7,
      u'id': 9,
      u'lastLots': 7,
      u'lastTicks': 12344,
      u'liqInd': u'MAKER',
      u'lots': 7,
      u'market': u'EURUSD.MAR14',
      u'matchId': 10,
      u'minLots': None,
      u'orderId': 3,
      u'ref': None,
      u'resd': 0,
      u'settlDate': 20170102,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12344
    }], resp.content)

  def makerPosn(self, conn):
    resp = conn.send('GET', '/accnt/posn')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'buyCost': 185163,
      u'buyLots': 15,
      u'contr': u'EURUSD',
      u'sellCost': 0,
      u'sellLots': 0,
      u'settlDate': 20170102
    }], resp.content)

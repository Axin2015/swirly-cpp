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
        self.createMarket(conn, 'GBPUSD.MAR14', 'GBPUSD', 20140302, 20140301)

        conn.setAuth('MARAYL', 0x2)
        self.createOrder(conn, 'EURUSD.MAR14', 'SELL', 5, 12347)
        self.createOrder(conn, 'EURUSD.MAR14', 'SELL', 3, 12346)
        self.createOrder(conn, 'EURUSD.MAR14', 'BUY', 3, 12344)
        self.createOrder(conn, 'EURUSD.MAR14', 'BUY', 5, 12343)

        self.createOrder(conn, 'GBPUSD.MAR14', 'SELL', 3, 15346)
        self.createOrder(conn, 'GBPUSD.MAR14', 'BUY', 3, 15344)

        self.getAll(conn)
        self.getOffset(conn)
        self.getLimit(conn)
        self.getOffsetAndLimit(conn)

        self.exceedOffset(conn)
        self.exceedLimit(conn)

  def getAll(self, conn):
    resp = conn.send('GET', '/accnt/exec')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'contr': u'GBPUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 2,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'market': u'GBPUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resd': 3,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'NEW',
      u'ticks': 15344
    }, {
      u'accnt': u'MARAYL',
      u'contr': u'GBPUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 1,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'market': u'GBPUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resd': 3,
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'NEW',
      u'ticks': 15346
    }, {
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 4,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 5,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 4,
      u'ref': None,
      u'resd': 5,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'NEW',
      u'ticks': 12343
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
      u'lots': 3,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 3,
      u'ref': None,
      u'resd': 3,
      u'settlDate': 20140302,
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
      u'lots': 3,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resd': 3,
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'NEW',
      u'ticks': 12346
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
      u'side': u'SELL',
      u'state': u'NEW',
      u'ticks': 12347
    }], resp.content)

  def getOffset(self, conn):
    resp = conn.send('GET', '/accnt/exec?offset=2')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 4,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 5,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 4,
      u'ref': None,
      u'resd': 5,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'NEW',
      u'ticks': 12343
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
      u'lots': 3,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 3,
      u'ref': None,
      u'resd': 3,
      u'settlDate': 20140302,
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
      u'lots': 3,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resd': 3,
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'NEW',
      u'ticks': 12346
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
      u'side': u'SELL',
      u'state': u'NEW',
      u'ticks': 12347
    }], resp.content)

  def getLimit(self, conn):
    resp = conn.send('GET', '/accnt/exec?limit=3')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'contr': u'GBPUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 2,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'market': u'GBPUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resd': 3,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'NEW',
      u'ticks': 15344
    }, {
      u'accnt': u'MARAYL',
      u'contr': u'GBPUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 1,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'market': u'GBPUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resd': 3,
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'NEW',
      u'ticks': 15346
    }, {
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 4,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 5,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 4,
      u'ref': None,
      u'resd': 5,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'NEW',
      u'ticks': 12343
    }], resp.content)

  def getOffsetAndLimit(self, conn):
    resp = conn.send('GET', '/accnt/exec?offset=2&limit=3')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 4,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 5,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 4,
      u'ref': None,
      u'resd': 5,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'NEW',
      u'ticks': 12343
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
      u'lots': 3,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 3,
      u'ref': None,
      u'resd': 3,
      u'settlDate': 20140302,
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
      u'lots': 3,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resd': 3,
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'NEW',
      u'ticks': 12346
    }], resp.content)

  def exceedOffset(self, conn):
    resp = conn.send('GET', '/accnt/exec?offset=99&limit=3')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([
    ], resp.content)

  def exceedLimit(self, conn):
    resp = conn.send('GET', '/accnt/exec?offset=0&limit=99')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'contr': u'GBPUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 2,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'market': u'GBPUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resd': 3,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'NEW',
      u'ticks': 15344
    }, {
      u'accnt': u'MARAYL',
      u'contr': u'GBPUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 1,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'market': u'GBPUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resd': 3,
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'NEW',
      u'ticks': 15346
    }, {
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 4,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 5,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 4,
      u'ref': None,
      u'resd': 5,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'NEW',
      u'ticks': 12343
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
      u'lots': 3,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 3,
      u'ref': None,
      u'resd': 3,
      u'settlDate': 20140302,
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
      u'lots': 3,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resd': 3,
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'NEW',
      u'ticks': 12346
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
      u'side': u'SELL',
      u'state': u'NEW',
      u'ticks': 12347
    }], resp.content)

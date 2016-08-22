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
        self.createOrder(conn, 'EURUSD.MAR14', 'BUY', 3, 12345)
        self.createOrder(conn, 'EURUSD.MAR14', 'BUY', 5, 12345)
        self.createOrder(conn, 'EURUSD.MAR14', 'BUY', 7, 12345)
        self.createOrder(conn, 'EURUSD.MAR14', 'BUY', 11, 12345)
        self.cancelSingle(conn)
        self.cancelMulti(conn)

  def cancelSingle(self, conn):
    resp = conn.send('PUT', '/accnt/order/EURUSD.MAR14/2', lots = 0);

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'execs': [{
        u'accnt': u'MARAYL',
        u'contr': u'EURUSD',
        u'cost': 0,
        u'cpty': None,
        u'created': self.now,
        u'exec': 0,
        u'id': 5,
        u'lastLots': None,
        u'lastTicks': None,
        u'liqInd': None,
        u'lots': 5,
        u'market': u'EURUSD.MAR14',
        u'matchId': None,
        u'minLots': None,
        u'orderId': 2,
        u'ref': None,
        u'resd': 0,
        u'settlDate': 20140302,
        u'side': u'BUY',
        u'state': u'CANCEL',
        u'ticks': 12345
      }],
      u'orders': [{
        u'accnt': u'MARAYL',
        u'contr': u'EURUSD',
        u'cost': 0,
        u'created': self.now,
        u'exec': 0,
        u'id': 2,
        u'lastLots': None,
        u'lastTicks': None,
        u'lots': 5,
        u'market': u'EURUSD.MAR14',
        u'minLots': None,
        u'modified': self.now,
        u'ref': None,
        u'resd': 0,
        u'settlDate': 20140302,
        u'side': u'BUY',
        u'state': u'CANCEL',
        u'ticks': 12345
      }],
      u'posn': None,
      u'view': {
        u'bidCount': [3, None, None],
        u'bidResd': [21, None, None],
        u'bidTicks': [12345, None, None],
        u'contr': u'EURUSD',
        u'lastLots': None,
        u'lastTicks': None,
        u'lastTime': None,
        u'market': u'EURUSD.MAR14',
        u'offerCount': [None, None, None],
        u'offerResd': [None, None, None],
        u'offerTicks': [None, None, None],
        u'settlDate': 20140302
      }
    }, resp.content)

  def cancelMulti(self, conn):
    resp = conn.send('PUT', '/accnt/order/EURUSD.MAR14/1,3', lots = 0);

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'execs': [{
        u'accnt': u'MARAYL',
        u'contr': u'EURUSD',
        u'cost': 0,
        u'cpty': None,
        u'created': self.now,
        u'exec': 0,
        u'id': 6,
        u'lastLots': None,
        u'lastTicks': None,
        u'lots': 3,
        u'market': u'EURUSD.MAR14',
        u'matchId': None,
        u'minLots': None,
        u'orderId': 1,
        u'ref': None,
        u'resd': 0,
        u'liqInd': None,
        u'settlDate': 20140302,
        u'side': u'BUY',
        u'state': u'CANCEL',
        u'ticks': 12345
      }, {
        u'accnt': u'MARAYL',
        u'contr': u'EURUSD',
        u'cost': 0,
        u'cpty': None,
        u'created': self.now,
        u'exec': 0,
        u'id': 7,
        u'lastLots': None,
        u'lastTicks': None,
        u'lots': 7,
        u'market': u'EURUSD.MAR14',
        u'matchId': None,
        u'minLots': None,
        u'orderId': 3,
        u'ref': None,
        u'resd': 0,
        u'liqInd': None,
        u'settlDate': 20140302,
        u'side': u'BUY',
        u'state': u'CANCEL',
        u'ticks': 12345
      }],
      u'orders': [{
        u'accnt': u'MARAYL',
        u'contr': u'EURUSD',
        u'cost': 0,
        u'created': self.now,
        u'exec': 0,
        u'id': 1,
        u'lastLots': None,
        u'lastTicks': None,
        u'lots': 3,
        u'market': u'EURUSD.MAR14',
        u'minLots': None,
        u'modified': self.now,
        u'ref': None,
        u'resd': 0,
        u'settlDate': 20140302,
        u'side': u'BUY',
        u'state': u'CANCEL',
        u'ticks': 12345
      }, {
        u'accnt': u'MARAYL',
        u'contr': u'EURUSD',
        u'cost': 0,
        u'created': self.now,
        u'exec': 0,
        u'id': 3,
        u'lastLots': None,
        u'lastTicks': None,
        u'lots': 7,
        u'market': u'EURUSD.MAR14',
        u'minLots': None,
        u'modified': self.now,
        u'ref': None,
        u'resd': 0,
        u'settlDate': 20140302,
        u'side': u'BUY',
        u'state': u'CANCEL',
        u'ticks': 12345
      }],
      u'posn': None,
      u'view': {
        u'bidCount': [1, None, None],
        u'bidResd': [11, None, None],
        u'bidTicks': [12345, None, None],
        u'contr': u'EURUSD',
        u'lastLots': None,
        u'lastTicks': None,
        u'lastTime': None,
        u'market': u'EURUSD.MAR14',
        u'offerCount': [None, None, None],
        u'offerResd': [None, None, None],
        u'offerTicks': [None, None, None],
        u'settlDate': 20140302
      }
    }, resp.content)

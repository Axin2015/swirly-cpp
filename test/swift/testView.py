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
        conn.setUser('MARAYL')

        self.createMarket(conn, 'EURUSD.MAR14', 'EURUSD', 20170102, 20170101)
        self.createOrder(conn, 'EURUSD.MAR14', 'SELL', 7, 12348)
        self.createOrder(conn, 'EURUSD.MAR14', 'SELL', 5, 12347)
        self.createOrder(conn, 'EURUSD.MAR14', 'SELL', 3, 12346)
        self.createOrder(conn, 'EURUSD.MAR14', 'BUY', 3, 12344)
        self.createOrder(conn, 'EURUSD.MAR14', 'BUY', 5, 12343)
        self.createOrder(conn, 'EURUSD.MAR14', 'BUY', 7, 12343)

        self.createMarket(conn, 'GBPUSD.MAR14', 'GBPUSD', 20170102, 20170101)
        self.createOrder(conn, 'GBPUSD.MAR14', 'SELL', 5, 15346)
        self.createOrder(conn, 'GBPUSD.MAR14', 'SELL', 3, 15346)
        self.createOrder(conn, 'GBPUSD.MAR14', 'BUY', 3, 15344)
        self.createOrder(conn, 'GBPUSD.MAR14', 'BUY', 5, 15343)

        self.createMarket(conn, 'USDCHF.MAR14', 'USDCHF', 20170102, 20170101)
        self.createOrder(conn, 'USDCHF.MAR14', 'SELL', 3, 9346)
        self.createOrder(conn, 'USDCHF.MAR14', 'BUY', 3, 9344)

        self.getViewAll(conn)
        self.getViewSingle(conn)

  def getViewAll(self, conn):
    resp = conn.send('GET', '/api/view')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'bidCount': [1, 2, None],
      u'bidResd': [3, 12, None],
      u'bidTicks': [12344, 12343, None],
      u'contr': u'EURUSD',
      u'lastLots': None,
      u'lastTicks': None,
      u'lastTime': None,
      u'market': u'EURUSD.MAR14',
      u'offerCount': [1, 1, 1],
      u'offerResd': [3, 5, 7],
      u'offerTicks': [12346, 12347, 12348],
      u'settlDate': 20170102
    }, {
      u'bidCount': [1, 1, None],
      u'bidResd': [3, 5, None],
      u'bidTicks': [15344, 15343, None],
      u'contr': u'GBPUSD',
      u'lastLots': None,
      u'lastTicks': None,
      u'lastTime': None,
      u'market': u'GBPUSD.MAR14',
      u'offerCount': [2, None, None],
      u'offerResd': [8, None, None],
      u'offerTicks': [15346, None, None],
      u'settlDate': 20170102
    }, {
      u'bidCount': [1, None, None],
      u'bidResd': [3, None, None],
      u'bidTicks': [9344, None, None],
      u'contr': u'USDCHF',
      u'lastLots': None,
      u'lastTicks': None,
      u'lastTime': None,
      u'market': u'USDCHF.MAR14',
      u'offerCount': [1, None, None],
      u'offerResd': [3, None, None],
      u'offerTicks': [9346, None, None],
      u'settlDate': 20170102
    }], resp.content)

  def getViewSingle(self, conn):
    resp = conn.send('GET', '/api/view/EURUSD.MAR14')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'bidCount': [1, 2, None],
      u'bidResd': [3, 12, None],
      u'bidTicks': [12344, 12343, None],
      u'contr': u'EURUSD',
      u'lastLots': None,
      u'lastTicks': None,
      u'lastTime': None,
      u'market': u'EURUSD.MAR14',
      u'offerCount': [1, 1, 1],
      u'offerResd': [3, 5, 7],
      u'offerTicks': [12346, 12347, 12348],
      u'settlDate': 20170102
    }, resp.content)

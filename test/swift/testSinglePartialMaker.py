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
        conn.setAuth('emailAddress=mark.aylett@swirlycloud.com')

        self.createMarket(conn, 'EURUSD.MAR14', 'EURUSD', 20170102, 20170101)
        self.createOrder(conn, 'EURUSD.MAR14', 'BUY', 5, 12345)

        conn.setAuth('emailAddress=goska.aylett@swirlycloud.com')
        self.takeOrder(conn)

        conn.setAuth('emailAddress=mark.aylett@swirlycloud.com')
        self.makerOrder(conn)
        self.makerTrade(conn)
        self.makerPosn(conn)

  def takeOrder(self, conn):
    resp = conn.send('POST', '/sess/order/EURUSD.MAR14',
                     side = 'SELL',
                     lots = 2,
                     ticks = 12345)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'execs': [{
        u'contr': u'EURUSD',
        u'cost': 0,
        u'cpty': None,
        u'created': self.now,
        u'exec': 0,
        u'id': 2,
        u'lastLots': None,
        u'lastTicks': None,
        u'lots': 2,
        u'market': u'EURUSD.MAR14',
        u'matchId': None,
        u'minLots': None,
        u'orderId': 2,
        u'ref': None,
        u'resd': 2,
        u'liqInd': None,
        u'settlDate': 20170102,
        u'side': u'SELL',
        u'state': u'NEW',
        u'ticks': 12345,
        u'trader': u'GOSAYL'
      }, {
        u'contr': u'EURUSD',
        u'cost': 24690,
        u'cpty': u'MARAYL',
        u'created': self.now,
        u'exec': 2,
        u'id': 4,
        u'lastLots': 2,
        u'lastTicks': 12345,
        u'lots': 2,
        u'market': u'EURUSD.MAR14',
        u'matchId': 3,
        u'minLots': None,
        u'orderId': 2,
        u'ref': None,
        u'resd': 0,
        u'liqInd': u'TAKER',
        u'settlDate': 20170102,
        u'side': u'SELL',
        u'state': u'TRADE',
        u'ticks': 12345,
        u'trader': u'GOSAYL'
      }],
      u'orders': [{
        u'contr': u'EURUSD',
        u'cost': 24690,
        u'created': self.now,
        u'exec': 2,
        u'id': 2,
        u'lastLots': 2,
        u'lastTicks': 12345,
        u'lots': 2,
        u'market': u'EURUSD.MAR14',
        u'minLots': None,
        u'modified': self.now,
        u'ref': None,
        u'resd': 0,
        u'settlDate': 20170102,
        u'side': u'SELL',
        u'state': u'TRADE',
        u'ticks': 12345,
        u'trader': u'GOSAYL'
      }],
      u'posn': {
        u'buyCost': 0,
        u'buyLots': 0,
        u'contr': u'EURUSD',
        u'sellCost': 24690,
        u'sellLots': 2,
        u'settlDate': 20170102,
        u'trader': u'GOSAYL'
      },
      u'view': {
        u'bidCount': [1, None, None],
        u'bidResd': [3, None, None],
        u'bidTicks': [12345, None, None],
        u'contr': u'EURUSD',
        u'lastLots': 2,
        u'lastTicks': 12345,
        u'lastTime': self.now,
        u'market': u'EURUSD.MAR14',
        u'offerCount': [None, None, None],
        u'offerResd': [None, None, None],
        u'offerTicks': [None, None, None],
        u'settlDate': 20170102
      }
    }, resp.content)

  def makerOrder(self, conn):
    resp = conn.send('GET', '/sess/order')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'contr': u'EURUSD',
      u'cost': 24690,
      u'created': self.now,
      u'exec': 2,
      u'id': 1,
      u'lastLots': 2,
      u'lastTicks': 12345,
      u'lots': 5,
      u'market': u'EURUSD.MAR14',
      u'minLots': None,
      u'modified': self.now,
      u'ref': None,
      u'resd': 3,
      u'settlDate': 20170102,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12345,
      u'trader': u'MARAYL'
    }], resp.content)

  def makerTrade(self, conn):
    resp = conn.send('GET', '/sess/trade')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'contr': u'EURUSD',
      u'cost': 24690,
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec': 2,
      u'id': 3,
      u'lastLots': 2,
      u'lastTicks': 12345,
      u'lots': 5,
      u'market': u'EURUSD.MAR14',
      u'matchId': 4,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resd': 3,
      u'liqInd': u'MAKER',
      u'settlDate': 20170102,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12345,
      u'trader': u'MARAYL'
    }], resp.content)

  def makerPosn(self, conn):
    resp = conn.send('GET', '/sess/posn')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'buyCost': 24690,
      u'buyLots': 2,
      u'contr': u'EURUSD',
      u'sellCost': 0,
      u'sellLots': 0,
      u'settlDate': 20170102,
      u'trader': u'MARAYL'
    }], resp.content)

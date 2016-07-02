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
        self.createBid(conn)
        self.createOffer(conn)

  def createBid(self, conn):
    resp = conn.send('POST', '/sess/order/EURUSD.MAR14',
                     side = 'BUY',
                     lots = 5,
                     ticks = 12344)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'execs': [{
        u'contr': u'EURUSD',
        u'cost': 0,
        u'cpty': None,
        u'created': self.now,
        u'exec': 0,
        u'id': 1,
        u'lastLots': None,
        u'lastTicks': None,
        u'lots': 5,
        u'market': u'EURUSD.MAR14',
        u'matchId': None,
        u'minLots': None,
        u'orderId': 1,
        u'ref': None,
        u'resd': 5,
        u'liqInd': None,
        u'settlDate': 20170102,
        u'side': u'BUY',
        u'state': u'NEW',
        u'ticks': 12344,
        u'trader': u'MARAYL'
      }],
      u'orders': [{
        u'contr': u'EURUSD',
        u'cost': 0,
        u'created': self.now,
        u'exec': 0,
        u'id': 1,
        u'lastLots': None,
        u'lastTicks': None,
        u'lots': 5,
        u'market': u'EURUSD.MAR14',
        u'minLots': None,
        u'modified': self.now,
        u'ref': None,
        u'resd': 5,
        u'settlDate': 20170102,
        u'side': u'BUY',
        u'state': u'NEW',
        u'ticks': 12344,
        u'trader': u'MARAYL'
      }],
      u'posn': None,
      u'view': {
        u'bidCount': [1, None, None],
        u'bidResd': [5, None, None],
        u'bidTicks': [12344, None, None],
        u'contr': u'EURUSD',
        u'lastLots': None,
        u'lastTicks': None,
        u'lastTime': None,
        u'market': u'EURUSD.MAR14',
        u'offerCount': [None, None, None],
        u'offerResd': [None, None, None],
        u'offerTicks': [None, None, None],
        u'settlDate': 20170102
      }
    }, resp.content)

  def createOffer(self, conn):
    resp = conn.send('POST', '/sess/order/EURUSD.MAR14',
                     side = 'SELL',
                     lots = 5,
                     ticks = 12346)

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
        u'lots': 5,
        u'market': u'EURUSD.MAR14',
        u'matchId': None,
        u'minLots': None,
        u'orderId': 2,
        u'ref': None,
        u'resd': 5,
        u'liqInd': None,
        u'settlDate': 20170102,
        u'side': u'SELL',
        u'state': u'NEW',
        u'ticks': 12346,
        u'trader': u'MARAYL'
      }],
      u'orders': [{
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
        u'resd': 5,
        u'settlDate': 20170102,
        u'side': u'SELL',
        u'state': u'NEW',
        u'ticks': 12346,
        u'trader': u'MARAYL'
      }],
      u'posn': None,
      u'view': {
        u'bidCount': [1, None, None],
        u'bidResd': [5, None, None],
        u'bidTicks': [12344, None, None],
        u'contr': u'EURUSD',
        u'lastLots': None,
        u'lastTicks': None,
        u'lastTime': None,
        u'market': u'EURUSD.MAR14',
        u'offerCount': [1, None, None],
        u'offerResd': [5, None, None],
        u'offerTicks': [12346, None, None],
        u'settlDate': 20170102
      }
    }, resp.content)

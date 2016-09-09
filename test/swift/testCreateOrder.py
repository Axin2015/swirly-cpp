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
    with DbFile() as dbFile:
      with Server(dbFile, self.now) as server:
        with Client() as client:
          client.setTime(self.now)

          self.createMarket(client, 'EURUSD', 20140302)

          self.checkAuth(client)

          self.createBid(client)
          self.createOffer(client)

  def checkAuth(self, client):
    client.setAuth(None, 0x2)

    resp = client.send('POST', '/accnt/order/EURUSD/20140302')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    client.setAuth('MARAYL', ~0x2 & 0x7fffffff)

    resp = client.send('POST', '/accnt/order/EURUSD/20140302')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def createBid(self, client):
    client.setTrader('MARAYL')
    resp = client.send('POST', '/accnt/order/EURUSD/20140302',
                       side = 'BUY',
                       lots = 5,
                       ticks = 12344)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'market': {
        u'bidCount': [1, None, None],
        u'bidResd': [5, None, None],
        u'bidTicks': [12344, None, None],
        u'contr': u'EURUSD',
        u'lastLots': None,
        u'lastTicks': None,
        u'lastTime': None,
        u'offerCount': [None, None, None],
        u'offerResd': [None, None, None],
        u'offerTicks': [None, None, None],
        u'settlDate': 20140302,
        u'state': 0
      },
      u'execs': [{
        u'accnt': u'MARAYL',
        u'contr': u'EURUSD',
        u'cost': 0,
        u'cpty': None,
        u'created': self.now,
        u'exec': 0,
        u'id': 1,
        u'lastLots': None,
        u'lastTicks': None,
        u'lots': 5,
        u'matchId': None,
        u'minLots': None,
        u'orderId': 1,
        u'ref': None,
        u'resd': 5,
        u'liqInd': None,
        u'settlDate': 20140302,
        u'side': u'BUY',
        u'state': u'NEW',
        u'ticks': 12344
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
        u'lots': 5,
        u'minLots': None,
        u'modified': self.now,
        u'ref': None,
        u'resd': 5,
        u'settlDate': 20140302,
        u'side': u'BUY',
        u'state': u'NEW',
        u'ticks': 12344
      }],
      u'posn': None
    }, resp.content)

  def createOffer(self, client):
    client.setTrader('MARAYL')
    resp = client.send('POST', '/accnt/order/EURUSD/20140302',
                       side = 'SELL',
                       lots = 5,
                       ticks = 12346)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'market': {
        u'bidCount': [1, None, None],
        u'bidResd': [5, None, None],
        u'bidTicks': [12344, None, None],
        u'contr': u'EURUSD',
        u'lastLots': None,
        u'lastTicks': None,
        u'lastTime': None,
        u'offerCount': [1, None, None],
        u'offerResd': [5, None, None],
        u'offerTicks': [12346, None, None],
        u'settlDate': 20140302,
        u'state': 0
      },
      u'execs': [{
        u'accnt': u'MARAYL',
        u'contr': u'EURUSD',
        u'cost': 0,
        u'cpty': None,
        u'created': self.now,
        u'exec': 0,
        u'id': 2,
        u'lastLots': None,
        u'lastTicks': None,
        u'lots': 5,
        u'matchId': None,
        u'minLots': None,
        u'orderId': 2,
        u'ref': None,
        u'resd': 5,
        u'liqInd': None,
        u'settlDate': 20140302,
        u'side': u'SELL',
        u'state': u'NEW',
        u'ticks': 12346
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
        u'minLots': None,
        u'modified': self.now,
        u'ref': None,
        u'resd': 5,
        u'settlDate': 20140302,
        u'side': u'SELL',
        u'state': u'NEW',
        u'ticks': 12346
      }],
      u'posn': None
    }, resp.content)

# The Restful Matching-Engine.
# Copyright (C) 2013, 2017 Swirly Cloud Limited.
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

          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'BUY', 3, 12345)
          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'BUY', 5, 12345)
          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'BUY', 7, 12345)
          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'BUY', 11, 12345)

          self.checkAuth(client)

          self.cancelSingle(client)
          self.cancelMulti(client)

  def checkAuth(self, client):
    client.setAuth(None, 0x2)

    resp = client.send('PUT', '/accnt/orders/EURUSD/20140302/1')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    client.setAuth('MARAYL', ~0x2 & 0x7fffffff)

    resp = client.send('PUT', '/accnt/orders/EURUSD/20140302/1')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def cancelSingle(self, client):
    client.setTrader('MARAYL')
    resp = client.send('PUT', '/accnt/orders/EURUSD/20140302/2', lots = 0)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'market': {
        u'bidCount': [3, None, None],
        u'bidLots': [21, None, None],
        u'bidTicks': [12345, None, None],
        u'instr': u'EURUSD',
        u'id': 82255,
        u'lastLots': None,
        u'lastTicks': None,
        u'lastTime': None,
        u'offerCount': [None, None, None],
        u'offerLots': [None, None, None],
        u'offerTicks': [None, None, None],
        u'settlDate': 20140302,
        u'state': 0
      },
      u'execs': [{
        u'accnt': u'MARAYL',
        u'instr': u'EURUSD',
        u'cpty': None,
        u'created': self.now,
        u'execCost': 0,
        u'execLots': 0,
        u'id': 5,
        u'lastLots': None,
        u'lastTicks': None,
        u'liqInd': None,
        u'lots': 5,
        u'marketId': 82255,
        u'matchId': None,
        u'minLots': None,
        u'orderId': 2,
        u'ref': None,
        u'resdLots': 0,
        u'settlDate': 20140302,
        u'side': u'BUY',
        u'state': u'CANCEL',
        u'ticks': 12345
      }],
      u'orders': [{
        u'accnt': u'MARAYL',
        u'instr': u'EURUSD',
        u'created': self.now,
        u'execCost': 0,
        u'execLots': 0,
        u'id': 2,
        u'lastLots': None,
        u'lastTicks': None,
        u'lots': 5,
        u'marketId': 82255,
        u'minLots': None,
        u'modified': self.now,
        u'ref': None,
        u'resdLots': 0,
        u'settlDate': 20140302,
        u'side': u'BUY',
        u'state': u'CANCEL',
        u'ticks': 12345
      }],
      u'posn': None
    }, resp.content)

  def cancelMulti(self, client):
    client.setTrader('MARAYL')
    resp = client.send('PUT', '/accnt/orders/EURUSD/20140302/1,3', lots = 0)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'market': {
        u'bidCount': [1, None, None],
        u'bidLots': [11, None, None],
        u'bidTicks': [12345, None, None],
        u'instr': u'EURUSD',
        u'id': 82255,
        u'lastLots': None,
        u'lastTicks': None,
        u'lastTime': None,
        u'offerCount': [None, None, None],
        u'offerLots': [None, None, None],
        u'offerTicks': [None, None, None],
        u'settlDate': 20140302,
        u'state': 0
      },
      u'execs': [{
        u'accnt': u'MARAYL',
        u'instr': u'EURUSD',
        u'cpty': None,
        u'created': self.now,
        u'execCost': 0,
        u'execLots': 0,
        u'id': 7,
        u'lastLots': None,
        u'lastTicks': None,
        u'lots': 7,
        u'marketId': 82255,
        u'matchId': None,
        u'minLots': None,
        u'orderId': 3,
        u'ref': None,
        u'resdLots': 0,
        u'liqInd': None,
        u'settlDate': 20140302,
        u'side': u'BUY',
        u'state': u'CANCEL',
        u'ticks': 12345
      }, {
        u'accnt': u'MARAYL',
        u'instr': u'EURUSD',
        u'cpty': None,
        u'created': self.now,
        u'execCost': 0,
        u'execLots': 0,
        u'id': 6,
        u'lastLots': None,
        u'lastTicks': None,
        u'lots': 3,
        u'marketId': 82255,
        u'matchId': None,
        u'minLots': None,
        u'orderId': 1,
        u'ref': None,
        u'resdLots': 0,
        u'liqInd': None,
        u'settlDate': 20140302,
        u'side': u'BUY',
        u'state': u'CANCEL',
        u'ticks': 12345
      }],
      u'orders': [{
        u'accnt': u'MARAYL',
        u'instr': u'EURUSD',
        u'created': self.now,
        u'execCost': 0,
        u'execLots': 0,
        u'id': 1,
        u'lastLots': None,
        u'lastTicks': None,
        u'lots': 3,
        u'marketId': 82255,
        u'minLots': None,
        u'modified': self.now,
        u'ref': None,
        u'resdLots': 0,
        u'settlDate': 20140302,
        u'side': u'BUY',
        u'state': u'CANCEL',
        u'ticks': 12345
      }, {
        u'accnt': u'MARAYL',
        u'instr': u'EURUSD',
        u'created': self.now,
        u'execCost': 0,
        u'execLots': 0,
        u'id': 3,
        u'lastLots': None,
        u'lastTicks': None,
        u'lots': 7,
        u'marketId': 82255,
        u'minLots': None,
        u'modified': self.now,
        u'ref': None,
        u'resdLots': 0,
        u'settlDate': 20140302,
        u'side': u'BUY',
        u'state': u'CANCEL',
        u'ticks': 12345
      }],
      u'posn': None
    }, resp.content)

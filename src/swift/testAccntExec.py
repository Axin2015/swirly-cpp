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
          self.createMarket(client, 'GBPUSD', 20140302)

          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'SELL', 5, 12347)
          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'SELL', 3, 12346)
          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'BUY', 3, 12344)
          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'BUY', 5, 12343)

          self.createOrder(client, 'MARAYL', 'GBPUSD', 20140302, 'SELL', 3, 15346)
          self.createOrder(client, 'MARAYL', 'GBPUSD', 20140302, 'BUY', 3, 15344)

          self.checkAuth(client)

          self.getAll(client)
          self.getOffset(client)
          self.getLimit(client)
          self.getOffsetAndLimit(client)

          self.exceedOffset(client)
          self.exceedLimit(client)

      with Server(dbFile, self.now) as server:
        with Client() as client:
          client.setTime(self.now)

          self.getAll(client)
          self.getOffset(client)
          self.getLimit(client)
          self.getOffsetAndLimit(client)

          self.exceedOffset(client)
          self.exceedLimit(client)

  def checkAuth(self, client):
    client.setAuth(None, 0x2)

    resp = client.send('GET', '/accnt/exec')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    client.setAuth('MARAYL', ~0x2 & 0x7fffffff)

    resp = client.send('GET', '/accnt/exec')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def getAll(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/exec')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'GBPUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 2,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'marketId': 147791,
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
      u'instr': u'GBPUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 1,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'marketId': 147791,
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
      u'instr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 4,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 5,
      u'marketId': 82255,
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
      u'instr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 3,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'marketId': 82255,
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
      u'instr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 2,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'marketId': 82255,
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
      u'instr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 1,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 5,
      u'marketId': 82255,
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

  def getOffset(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/exec?offset=2')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 4,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 5,
      u'marketId': 82255,
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
      u'instr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 3,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'marketId': 82255,
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
      u'instr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 2,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'marketId': 82255,
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
      u'instr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 1,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 5,
      u'marketId': 82255,
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

  def getLimit(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/exec?limit=3')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'GBPUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 2,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'marketId': 147791,
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
      u'instr': u'GBPUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 1,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'marketId': 147791,
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
      u'instr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 4,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 5,
      u'marketId': 82255,
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

  def getOffsetAndLimit(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/exec?offset=2&limit=3')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 4,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 5,
      u'marketId': 82255,
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
      u'instr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 3,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'marketId': 82255,
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
      u'instr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 2,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'marketId': 82255,
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

  def exceedOffset(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/exec?offset=99&limit=3')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([
    ], resp.content)

  def exceedLimit(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/exec?offset=0&limit=99')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'GBPUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 2,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'marketId': 147791,
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
      u'instr': u'GBPUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 1,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'marketId': 147791,
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
      u'instr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 4,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 5,
      u'marketId': 82255,
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
      u'instr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 3,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'marketId': 82255,
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
      u'instr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 2,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'marketId': 82255,
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
      u'instr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 0,
      u'id': 1,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 5,
      u'marketId': 82255,
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

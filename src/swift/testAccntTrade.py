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
          self.createMarket(client, 'EURUSD', 20140402)
          self.createMarket(client, 'GBPUSD', 20140302)

          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'SELL', 3, 12346)
          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'BUY', 3, 12346)

          self.createOrder(client, 'MARAYL', 'EURUSD', 20140402, 'SELL', 5, 12347)
          self.createOrder(client, 'MARAYL', 'EURUSD', 20140402, 'BUY', 5, 12347)

          self.createOrder(client, 'MARAYL', 'GBPUSD', 20140302, 'SELL', 3, 15346)
          self.createOrder(client, 'MARAYL', 'GBPUSD', 20140302, 'BUY', 3, 15346)

          self.checkAuth(client)

          self.getAll(client)
          self.getByInstr(client)
          self.getByMarket(client)
          self.getById(client)

      with Server(dbFile, self.now) as server:
        with Client() as client:
          client.setTime(self.now)

          self.getAll(client)
          self.getByInstr(client)
          self.getByMarket(client)
          self.getById(client)

  def checkAuth(self, client):
    client.setAuth(None, 0x2)

    resp = client.send('GET', '/accnt/trades')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    resp = client.send('GET', '/accnt/trades/EURUSD')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    resp = client.send('GET', '/accnt/trades/EURUSD/20140302')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    client.setAuth('MARAYL', ~0x2 & 0x7fffffff)

    resp = client.send('GET', '/accnt/trades')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

    resp = client.send('GET', '/accnt/trades/EURUSD')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

    resp = client.send('GET', '/accnt/trades/EURUSD/20140302')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def getAll(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/trades')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'execCost': 37038,
      u'execLots': 3,
      u'id': 3,
      u'lastLots': 3,
      u'lastTicks': 12346,
      u'liqInd': u'MAKER',
      u'lots': 3,
      u'marketId': 82255,
      u'matchId': 4,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resdLots': 0,
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'TRADE',
      u'ticks': 12346
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'execCost': 37038,
      u'execLots': 3,
      u'id': 4,
      u'lastLots': 3,
      u'lastTicks': 12346,
      u'liqInd': u'TAKER',
      u'lots': 3,
      u'marketId': 82255,
      u'matchId': 3,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resdLots': 0,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12346
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'execCost': 61735,
      u'execLots': 5,
      u'id': 3,
      u'lastLots': 5,
      u'lastTicks': 12347,
      u'liqInd': u'MAKER',
      u'lots': 5,
      u'marketId': 82286,
      u'matchId': 4,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resdLots': 0,
      u'settlDate': 20140402,
      u'side': u'SELL',
      u'state': u'TRADE',
      u'ticks': 12347
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'execCost': 61735,
      u'execLots': 5,
      u'id': 4,
      u'lastLots': 5,
      u'lastTicks': 12347,
      u'liqInd': u'TAKER',
      u'lots': 5,
      u'marketId': 82286,
      u'matchId': 3,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resdLots': 0,
      u'settlDate': 20140402,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12347
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'GBPUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'execCost': 46038,
      u'execLots': 3,
      u'id': 3,
      u'lastLots': 3,
      u'lastTicks': 15346,
      u'liqInd': u'MAKER',
      u'lots': 3,
      u'marketId': 147791,
      u'matchId': 4,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resdLots': 0,
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'TRADE',
      u'ticks': 15346
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'GBPUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'execCost': 46038,
      u'execLots': 3,
      u'id': 4,
      u'lastLots': 3,
      u'lastTicks': 15346,
      u'liqInd': u'TAKER',
      u'lots': 3,
      u'marketId': 147791,
      u'matchId': 3,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resdLots': 0,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 15346
    }], resp.content)

  def getByInstr(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/trades/EURUSD')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'execCost': 37038,
      u'execLots': 3,
      u'id': 3,
      u'lastLots': 3,
      u'lastTicks': 12346,
      u'liqInd': u'MAKER',
      u'lots': 3,
      u'marketId': 82255,
      u'matchId': 4,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resdLots': 0,
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'TRADE',
      u'ticks': 12346
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'execCost': 37038,
      u'execLots': 3,
      u'id': 4,
      u'lastLots': 3,
      u'lastTicks': 12346,
      u'liqInd': u'TAKER',
      u'lots': 3,
      u'marketId': 82255,
      u'matchId': 3,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resdLots': 0,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12346
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'execCost': 61735,
      u'execLots': 5,
      u'id': 3,
      u'lastLots': 5,
      u'lastTicks': 12347,
      u'liqInd': u'MAKER',
      u'lots': 5,
      u'marketId': 82286,
      u'matchId': 4,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resdLots': 0,
      u'settlDate': 20140402,
      u'side': u'SELL',
      u'state': u'TRADE',
      u'ticks': 12347
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'execCost': 61735,
      u'execLots': 5,
      u'id': 4,
      u'lastLots': 5,
      u'lastTicks': 12347,
      u'liqInd': u'TAKER',
      u'lots': 5,
      u'marketId': 82286,
      u'matchId': 3,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resdLots': 0,
      u'settlDate': 20140402,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12347
    }], resp.content)

  def getByMarket(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/trades/GBPUSD/20140302')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'GBPUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'execCost': 46038,
      u'execLots': 3,
      u'id': 3,
      u'lastLots': 3,
      u'lastTicks': 15346,
      u'liqInd': u'MAKER',
      u'lots': 3,
      u'marketId': 147791,
      u'matchId': 4,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resdLots': 0,
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'TRADE',
      u'ticks': 15346
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'GBPUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'execCost': 46038,
      u'execLots': 3,
      u'id': 4,
      u'lastLots': 3,
      u'lastTicks': 15346,
      u'liqInd': u'TAKER',
      u'lots': 3,
      u'marketId': 147791,
      u'matchId': 3,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resdLots': 0,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 15346
    }], resp.content)

  def getById(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/trades/GBPUSD/20140302/3')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'accnt': u'MARAYL',
      u'instr': u'GBPUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'execCost': 46038,
      u'execLots': 3,
      u'id': 3,
      u'lastLots': 3,
      u'lastTicks': 15346,
      u'liqInd': u'MAKER',
      u'lots': 3,
      u'marketId': 147791,
      u'matchId': 4,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resdLots': 0,
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'TRADE',
      u'ticks': 15346
    }, resp.content)

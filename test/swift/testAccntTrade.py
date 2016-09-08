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
          self.createMarket(client, 'GBPUSD', 20140302)

          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'SELL', 5, 12347)
          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'SELL', 3, 12346)
          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'BUY', 3, 12346)
          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'BUY', 5, 12347)

          self.createOrder(client, 'MARAYL', 'GBPUSD', 20140302, 'SELL', 3, 15346)
          self.createOrder(client, 'MARAYL', 'GBPUSD', 20140302, 'BUY', 3, 15346)

          self.checkAuth(client)

          self.getAll(client)
          self.getByMarket(client)
          self.getById(client)

      with Server(dbFile, self.now) as server:
        with Client() as client:
          client.setTime(self.now)

          self.getAll(client)
          self.getByMarket(client)
          self.getById(client)

  def checkAuth(self, client):
    client.setAuth(None, 0x2)
    resp = client.send('GET', '/accnt/trade')

    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    client.setAuth('MARAYL', ~0x2 & 0x7fffffff)
    resp = client.send('GET', '/accnt/trade')

    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def getAll(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/trade')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 37038,
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec': 3,
      u'id': 4,
      u'lastLots': 3,
      u'lastTicks': 12346,
      u'liqInd': u'MAKER',
      u'lots': 3,
      u'matchId': 5,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resd': 0,
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'TRADE',
      u'ticks': 12346
    }, {
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 37038,
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec': 3,
      u'id': 5,
      u'lastLots': 3,
      u'lastTicks': 12346,
      u'liqInd': u'TAKER',
      u'lots': 3,
      u'matchId': 4,
      u'minLots': None,
      u'orderId': 3,
      u'ref': None,
      u'resd': 0,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12346
    }, {
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 61735,
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec': 5,
      u'id': 7,
      u'lastLots': 5,
      u'lastTicks': 12347,
      u'liqInd': u'MAKER',
      u'lots': 5,
      u'matchId': 8,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resd': 0,
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'TRADE',
      u'ticks': 12347
    }, {
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 61735,
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec': 5,
      u'id': 8,
      u'lastLots': 5,
      u'lastTicks': 12347,
      u'liqInd': u'TAKER',
      u'lots': 5,
      u'matchId': 7,
      u'minLots': None,
      u'orderId': 6,
      u'ref': None,
      u'resd': 0,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12347
    }, {
      u'accnt': u'MARAYL',
      u'contr': u'GBPUSD',
      u'cost': 46038,
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec': 3,
      u'id': 3,
      u'lastLots': 3,
      u'lastTicks': 15346,
      u'liqInd': u'MAKER',
      u'lots': 3,
      u'matchId': 4,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resd': 0,
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'TRADE',
      u'ticks': 15346
    }, {
      u'accnt': u'MARAYL',
      u'contr': u'GBPUSD',
      u'cost': 46038,
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec': 3,
      u'id': 4,
      u'lastLots': 3,
      u'lastTicks': 15346,
      u'liqInd': u'TAKER',
      u'lots': 3,
      u'matchId': 3,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resd': 0,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 15346
    }], resp.content)

  def getByMarket(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/trade/EURUSD/20140302')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 37038,
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec': 3,
      u'id': 4,
      u'lastLots': 3,
      u'lastTicks': 12346,
      u'liqInd': u'MAKER',
      u'lots': 3,
      u'matchId': 5,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resd': 0,
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'TRADE',
      u'ticks': 12346
    }, {
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 37038,
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec': 3,
      u'id': 5,
      u'lastLots': 3,
      u'lastTicks': 12346,
      u'liqInd': u'TAKER',
      u'lots': 3,
      u'matchId': 4,
      u'minLots': None,
      u'orderId': 3,
      u'ref': None,
      u'resd': 0,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12346
    }, {
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 61735,
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec': 5,
      u'id': 7,
      u'lastLots': 5,
      u'lastTicks': 12347,
      u'liqInd': u'MAKER',
      u'lots': 5,
      u'matchId': 8,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resd': 0,
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'TRADE',
      u'ticks': 12347
    }, {
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 61735,
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec': 5,
      u'id': 8,
      u'lastLots': 5,
      u'lastTicks': 12347,
      u'liqInd': u'TAKER',
      u'lots': 5,
      u'matchId': 7,
      u'minLots': None,
      u'orderId': 6,
      u'ref': None,
      u'resd': 0,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12347
    }], resp.content)

  def getById(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/trade/GBPUSD/20140302/3')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'accnt': u'MARAYL',
      u'contr': u'GBPUSD',
      u'cost': 46038,
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec': 3,
      u'id': 3,
      u'lastLots': 3,
      u'lastTicks': 15346,
      u'liqInd': u'MAKER',
      u'lots': 3,
      u'matchId': 4,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resd': 0,
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'TRADE',
      u'ticks': 15346
    }, resp.content)

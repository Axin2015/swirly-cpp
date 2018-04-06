# The Restful Matching-Engine.
# Copyright (C) 2013, 2018 Swirly Cloud Limited.
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
          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'BUY', 5, 12344)
          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'BUY', 7, 12344)

          self.takeOrder(client)

          self.makerOrder(client)
          self.makerExec(client)
          self.makerTrade(client)
          self.makerPosn(client)

      with Server(dbFile, self.now) as server:
        with Client() as client:
          client.setTime(self.now)

          self.makerOrder(client)
          self.makerExec(client)
          self.makerTrade(client)
          self.makerPosn(client)

  def takeOrder(self, client):
    client.setTrader('GOSAYL')
    resp = client.send('POST', '/accnt/orders/EURUSD/20140302',
                       side = 'SELL',
                       lots = 15,
                       ticks = 12344)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'market': {
        u'bidCount': [None, None, None],
        u'bidLots': [None, None, None],
        u'bidTicks': [None, None, None],
        u'instr': u'EURUSD',
        u'id': 82255,
        u'lastLots': 7,
        u'lastTicks': 12344,
        u'lastTime': self.now,
        u'offerCount': [None, None, None],
        u'offerLots': [None, None, None],
        u'offerTicks': [None, None, None],
        u'settlDate': 20140302,
        u'state': 0
      },
      u'execs': [{
        u'accnt': u'GOSAYL',
        u'instr': u'EURUSD',
        u'cpty': u'MARAYL',
        u'created': self.now,
        u'execCost': 185163,
        u'execLots': 15,
        u'id': 10,
        u'lastLots': 7,
        u'lastTicks': 12344,
        u'liqInd': u'TAKER',
        u'lots': 15,
        u'marketId': 82255,
        u'matchId': 9,
        u'minLots': None,
        u'orderId': 4,
        u'ref': None,
        u'resdLots': 0,
        u'settlDate': 20140302,
        u'side': u'SELL',
        u'state': u'TRADE',
        u'ticks': 12344
      }, {
        u'accnt': u'GOSAYL',
        u'instr': u'EURUSD',
        u'cpty': u'MARAYL',
        u'created': self.now,
        u'execCost': 98755,
        u'execLots': 8,
        u'id': 8,
        u'lastLots': 5,
        u'lastTicks': 12344,
        u'liqInd': u'TAKER',
        u'lots': 15,
        u'marketId': 82255,
        u'matchId': 7,
        u'minLots': None,
        u'orderId': 4,
        u'ref': None,
        u'resdLots': 7,
        u'settlDate': 20140302,
        u'side': u'SELL',
        u'state': u'TRADE',
        u'ticks': 12344
      }, {
        u'accnt': u'GOSAYL',
        u'instr': u'EURUSD',
        u'cpty': u'MARAYL',
        u'created': self.now,
        u'execCost': 37035,
        u'execLots': 3,
        u'id': 6,
        u'lastLots': 3,
        u'lastTicks': 12345,
        u'liqInd': u'TAKER',
        u'lots': 15,
        u'marketId': 82255,
        u'matchId': 5,
        u'minLots': None,
        u'orderId': 4,
        u'ref': None,
        u'resdLots': 12,
        u'settlDate': 20140302,
        u'side': u'SELL',
        u'state': u'TRADE',
        u'ticks': 12344
      }, {
        u'accnt': u'GOSAYL',
        u'instr': u'EURUSD',
        u'cpty': None,
        u'created': self.now,
        u'execCost': 0,
        u'execLots': 0,
        u'id': 4,
        u'lastLots': None,
        u'lastTicks': None,
        u'liqInd': None,
        u'lots': 15,
        u'marketId': 82255,
        u'matchId': None,
        u'minLots': None,
        u'orderId': 4,
        u'ref': None,
        u'resdLots': 15,
        u'settlDate': 20140302,
        u'side': u'SELL',
        u'state': u'NEW',
        u'ticks': 12344
      }],
      u'orders': [{
        u'accnt': u'GOSAYL',
        u'instr': u'EURUSD',
        u'created': self.now,
        u'execCost': 185163,
        u'execLots': 15,
        u'id': 4,
        u'lastLots': 7,
        u'lastTicks': 12344,
        u'lots': 15,
        u'marketId': 82255,
        u'minLots': None,
        u'modified': self.now,
        u'ref': None,
        u'resdLots': 0,
        u'settlDate': 20140302,
        u'side': u'SELL',
        u'state': u'TRADE',
        u'ticks': 12344
      }],
      u'posn': {
        u'accnt': u'GOSAYL',
        u'buyCost': 0,
        u'buyLots': 0,
        u'instr': u'EURUSD',
        u'marketId': 82255,
        u'sellCost': 185163,
        u'sellLots': 15,
        u'settlDate': 20140302
      }
    }, resp.content)

  def makerOrder(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/orders')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([], resp.content)

  def makerExec(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/execs')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'execCost': 86408,
      u'execLots': 7,
      u'id': 9,
      u'lastLots': 7,
      u'lastTicks': 12344,
      u'liqInd': u'MAKER',
      u'lots': 7,
      u'marketId': 82255,
      u'matchId': 10,
      u'minLots': None,
      u'orderId': 3,
      u'ref': None,
      u'resdLots': 0,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12344
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'execCost': 61720,
      u'execLots': 5,
      u'id': 7,
      u'lastLots': 5,
      u'lastTicks': 12344,
      u'liqInd': u'MAKER',
      u'lots': 5,
      u'marketId': 82255,
      u'matchId': 8,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resdLots': 0,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12344
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'execCost': 37035,
      u'execLots': 3,
      u'id': 5,
      u'lastLots': 3,
      u'lastTicks': 12345,
      u'liqInd': u'MAKER',
      u'lots': 3,
      u'marketId': 82255,
      u'matchId': 6,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resdLots': 0,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12345
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': None,
      u'created': self.now,
      u'execCost': 0,
      u'execLots': 0,
      u'id': 3,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 7,
      u'marketId': 82255,
      u'matchId': None,
      u'minLots': None,
      u'orderId': 3,
      u'ref': None,
      u'resdLots': 7,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'NEW',
      u'ticks': 12344
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': None,
      u'created': self.now,
      u'execCost': 0,
      u'execLots': 0,
      u'id': 2,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 5,
      u'marketId': 82255,
      u'matchId': None,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resdLots': 5,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'NEW',
      u'ticks': 12344
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': None,
      u'created': self.now,
      u'execCost': 0,
      u'execLots': 0,
      u'id': 1,
      u'lastLots': None,
      u'lastTicks': None,
      u'liqInd': None,
      u'lots': 3,
      u'marketId': 82255,
      u'matchId': None,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resdLots': 3,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'NEW',
      u'ticks': 12345
    }], resp.content)

  def makerTrade(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/trades')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'execCost': 37035,
      u'execLots': 3,
      u'id': 5,
      u'lastLots': 3,
      u'lastTicks': 12345,
      u'liqInd': u'MAKER',
      u'lots': 3,
      u'marketId': 82255,
      u'matchId': 6,
      u'minLots': None,
      u'orderId': 1,
      u'ref': None,
      u'resdLots': 0,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12345
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'execCost': 61720,
      u'execLots': 5,
      u'id': 7,
      u'lastLots': 5,
      u'lastTicks': 12344,
      u'liqInd': u'MAKER',
      u'lots': 5,
      u'marketId': 82255,
      u'matchId': 8,
      u'minLots': None,
      u'orderId': 2,
      u'ref': None,
      u'resdLots': 0,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12344
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'execCost': 86408,
      u'execLots': 7,
      u'id': 9,
      u'lastLots': 7,
      u'lastTicks': 12344,
      u'liqInd': u'MAKER',
      u'lots': 7,
      u'marketId': 82255,
      u'matchId': 10,
      u'minLots': None,
      u'orderId': 3,
      u'ref': None,
      u'resdLots': 0,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12344
    }], resp.content)

  def makerPosn(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/posns')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'buyCost': 185163,
      u'buyLots': 15,
      u'instr': u'EURUSD',
      u'marketId': 82255,
      u'sellCost': 0,
      u'sellLots': 0,
      u'settlDate': 20140302
    }], resp.content)

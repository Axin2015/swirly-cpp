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

          self.createOrder(client, 'MARAYL', 'EURUSD', 20140402, 'SELL', 5, 12346)
          self.createOrder(client, 'MARAYL', 'EURUSD', 20140402, 'BUY', 5, 12346)

          self.createOrder(client, 'MARAYL', 'GBPUSD', 20140302, 'SELL', 7, 15346)
          self.createOrder(client, 'MARAYL', 'GBPUSD', 20140302, 'BUY', 7, 15346)

          self.checkAuth(client)

          self.getAll(client)
          self.getByContr(client)
          self.getBySettlDate(client)

      with Server(dbFile, self.now) as server:
        with Client() as client:
          client.setTime(self.now)

          self.getAll(client)
          self.getByContr(client)
          self.getBySettlDate(client)

  def checkAuth(self, client):
    client.setAuth(None, 0x2)

    resp = client.send('GET', '/accnt/posn')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    resp = client.send('GET', '/accnt/posn/EURUSD')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    resp = client.send('GET', '/accnt/posn/EURUSD/20140302')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    client.setAuth('MARAYL', ~0x2 & 0x7fffffff)

    resp = client.send('GET', '/accnt/posn')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

    resp = client.send('GET', '/accnt/posn/EURUSD')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

    resp = client.send('GET', '/accnt/posn/EURUSD/20140302')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def getAll(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/posn')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'buyCost': 37038,
      u'buyLots': 3,
      u'contr': u'EURUSD',
      u'marketId': 82255,
      u'sellCost': 37038,
      u'sellLots': 3,
      u'settlDate': 20140302
    }, {
      u'accnt': u'MARAYL',
      u'buyCost': 61730,
      u'buyLots': 5,
      u'contr': u'EURUSD',
      u'marketId': 82286,
      u'sellCost': 61730,
      u'sellLots': 5,
      u'settlDate': 20140402
    }, {
      u'accnt': u'MARAYL',
      u'buyCost': 107422,
      u'buyLots': 7,
      u'contr': u'GBPUSD',
      u'marketId': 147791,
      u'sellCost': 107422,
      u'sellLots': 7,
      u'settlDate': 20140302
    }], resp.content)

  def getByContr(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/posn/EURUSD')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'buyCost': 37038,
      u'buyLots': 3,
      u'contr': u'EURUSD',
      u'marketId': 82255,
      u'sellCost': 37038,
      u'sellLots': 3,
      u'settlDate': 20140302
    }, {
      u'accnt': u'MARAYL',
      u'buyCost': 61730,
      u'buyLots': 5,
      u'contr': u'EURUSD',
      u'marketId': 82286,
      u'sellCost': 61730,
      u'sellLots': 5,
      u'settlDate': 20140402
    }], resp.content)

  def getBySettlDate(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/posn/EURUSD/20140302')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'accnt': u'MARAYL',
      u'buyCost': 37038,
      u'buyLots': 3,
      u'contr': u'EURUSD',
      u'marketId': 82255,
      u'sellCost': 37038,
      u'sellLots': 3,
      u'settlDate': 20140302
    }, resp.content)

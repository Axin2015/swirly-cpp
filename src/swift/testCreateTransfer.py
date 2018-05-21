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

          self.checkAuth(client)

          self.createDeposit(client)
          self.createWithdraw(client)

  def checkAuth(self, client):
    client.setAuth(None, 0x1)

    resp = client.send('POST', '/accnt/trades/EURUSD/20140302')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    client.setAuth('ADMIN', ~0x1 & 0x7fffffff)

    resp = client.send('POST', '/accnt/trades/EURUSD/20140302')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def createDeposit(self, client):
    client.setAdmin()
    resp = client.send('POST', '/accnt/trades/EURUSD/20140302',
                       accnt = 'MARAYL',
                       ref = 'test1',
                       side = 'Buy',
                       lots = 10)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': None,
      u'created': self.now,
      u'execCost': 0,
      u'execLots': 10,
      u'id': 1,
      u'lastLots': 10,
      u'lastTicks': 0,
      u'lots': 10,
      u'marketId': 82255,
      u'matchId': None,
      u'minLots': 1,
      u'orderId': 0,
      u'ref': u'test1',
      u'resdLots': 0,
      u'liqInd': None,
      u'settlDate': 20140302,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 0
    }], resp.content)

  def createWithdraw(self, client):
    client.setAdmin()
    resp = client.send('POST', '/accnt/trades/EURUSD/20140302',
                       accnt = 'MARAYL',
                       ref = 'test1',
                       side = 'Sell',
                       lots = 10)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': None,
      u'created': self.now,
      u'execCost': 0,
      u'execLots': 10,
      u'id': 2,
      u'lastLots': 10,
      u'lastTicks': 0,
      u'lots': 10,
      u'marketId': 82255,
      u'matchId': None,
      u'minLots': 1,
      u'orderId': 0,
      u'ref': u'test1',
      u'resdLots': 0,
      u'liqInd': None,
      u'settlDate': 20140302,
      u'side': u'Sell',
      u'state': u'Trade',
      u'ticks': 0
    }], resp.content)

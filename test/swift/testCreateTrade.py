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
    with Fixture() as fixture:
      with Connection() as conn:
        conn.setTime(self.now)

        self.createMarket(conn, 'EURUSD.MAR14', 'EURUSD', 20140302, 20140301)

        self.checkAuth(conn)

        self.createMakerBuy(conn)
        self.createTakerSell(conn)

  def checkAuth(self, conn):
    conn.setAuth(None, 0x1)
    resp = conn.send('POST', '/accnt/trade/EURUSD.MAR14')

    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    conn.setAuth('ADMIN', ~0x1 & 0x7fffffff)
    resp = conn.send('POST', '/accnt/trade/EURUSD.MAR14')

    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def createMakerBuy(self, conn):
    conn.setAdmin()
    resp = conn.send('POST', '/accnt/trade/EURUSD.MAR14',
                     accnt = 'MARAYL',
                     ref = 'test1',
                     side = 'BUY',
                     lots = 10,
                     ticks = 12345,
                     liqInd = 'MAKER',
                     cpty = 'GOSAYL')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 123450,
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec': 10,
      u'id': 1,
      u'lastLots': 10,
      u'lastTicks': 12345,
      u'lots': 10,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': 1,
      u'orderId': 0,
      u'ref': u'test1',
      u'resd': 0,
      u'liqInd': u'MAKER',
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12345
    }, {
      u'accnt': u'GOSAYL',
      u'contr': u'EURUSD',
      u'cost': 123450,
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec': 10,
      u'id': 2,
      u'lastLots': 10,
      u'lastTicks': 12345,
      u'lots': 10,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': 1,
      u'orderId': 0,
      u'ref': u'test1',
      u'resd': 0,
      u'liqInd': u'TAKER',
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'TRADE',
      u'ticks': 12345
    }], resp.content)

  def createTakerSell(self, conn):
    conn.setAdmin()
    resp = conn.send('POST', '/accnt/trade/EURUSD.MAR14',
                     accnt = 'MARAYL',
                     ref = 'test2',
                     side = 'SELL',
                     lots = 15,
                     ticks = 12345,
                     liqInd = 'TAKER',
                     cpty = 'GOSAYL')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 185175,
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec': 15,
      u'id': 3,
      u'lastLots': 15,
      u'lastTicks': 12345,
      u'lots': 15,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': 1,
      u'orderId': 0,
      u'ref': u'test2',
      u'resd': 0,
      u'liqInd': u'TAKER',
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'TRADE',
      u'ticks': 12345
    }, {
      u'accnt': u'GOSAYL',
      u'contr': u'EURUSD',
      u'cost': 185175,
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec': 15,
      u'id': 4,
      u'lastLots': 15,
      u'lastTicks': 12345,
      u'lots': 15,
      u'market': u'EURUSD.MAR14',
      u'matchId': None,
      u'minLots': 1,
      u'orderId': 0,
      u'ref': u'test2',
      u'resd': 0,
      u'liqInd': u'MAKER',
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 12345
    }], resp.content)

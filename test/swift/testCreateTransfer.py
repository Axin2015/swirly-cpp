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

        self.createMarket(conn, 'EURUSD', 20140302)

        self.checkAuth(conn)

        self.createDeposit(conn)
        self.createWithdraw(conn)

  def checkAuth(self, conn):
    conn.setAuth(None, 0x1)
    resp = conn.send('POST', '/accnt/trade/EURUSD/20140302')

    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    conn.setAuth('ADMIN', ~0x1 & 0x7fffffff)
    resp = conn.send('POST', '/accnt/trade/EURUSD/20140302')

    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def createDeposit(self, conn):
    conn.setAdmin()
    resp = conn.send('POST', '/accnt/trade/EURUSD/20140302',
                     accnt = 'MARAYL',
                     ref = 'test1',
                     side = 'BUY',
                     lots = 10)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 10,
      u'id': 1,
      u'lastLots': 10,
      u'lastTicks': 0,
      u'lots': 10,
      u'matchId': None,
      u'minLots': 1,
      u'orderId': 0,
      u'ref': u'test1',
      u'resd': 0,
      u'liqInd': None,
      u'settlDate': 20140302,
      u'side': u'BUY',
      u'state': u'TRADE',
      u'ticks': 0
    }], resp.content)

  def createWithdraw(self, conn):
    conn.setAdmin()
    resp = conn.send('POST', '/accnt/trade/EURUSD/20140302',
                     accnt = 'MARAYL',
                     ref = 'test1',
                     side = 'SELL',
                     lots = 10)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'contr': u'EURUSD',
      u'cost': 0,
      u'cpty': None,
      u'created': self.now,
      u'exec': 10,
      u'id': 2,
      u'lastLots': 10,
      u'lastTicks': 0,
      u'lots': 10,
      u'matchId': None,
      u'minLots': 1,
      u'orderId': 0,
      u'ref': u'test1',
      u'resd': 0,
      u'liqInd': None,
      u'settlDate': 20140302,
      u'side': u'SELL',
      u'state': u'TRADE',
      u'ticks': 0
    }], resp.content)

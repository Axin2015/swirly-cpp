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
        self.createMarket(conn, 'EURUSD', 20140402)
        self.createMarket(conn, 'GBPUSD', 20140302)

        self.createOrder(conn, 'MARAYL', 'EURUSD', 20140302, 'SELL', 3, 12346)
        self.createOrder(conn, 'MARAYL', 'EURUSD', 20140302, 'BUY', 3, 12346)

        self.createOrder(conn, 'MARAYL', 'EURUSD', 20140402, 'SELL', 5, 12346)
        self.createOrder(conn, 'MARAYL', 'EURUSD', 20140402, 'BUY', 5, 12346)

        self.createOrder(conn, 'MARAYL', 'GBPUSD', 20140302, 'SELL', 7, 15346)
        self.createOrder(conn, 'MARAYL', 'GBPUSD', 20140302, 'BUY', 7, 15346)

        self.checkAuth(conn)

        self.getAll(conn)
        self.getByContr(conn)
        self.getBySettlDate(conn)

  def checkAuth(self, conn):
    conn.setAuth(None, 0x2)
    resp = conn.send('GET', '/accnt/posn')

    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    conn.setAuth('MARAYL', ~0x2 & 0x7fffffff)
    resp = conn.send('GET', '/accnt/posn')

    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def getAll(self, conn):
    conn.setTrader('MARAYL')
    resp = conn.send('GET', '/accnt/posn')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'buyCost': 37038,
      u'buyLots': 3,
      u'contr': u'EURUSD',
      u'sellCost': 37038,
      u'sellLots': 3,
      u'settlDate': 20140302
    }, {
      u'accnt': u'MARAYL',
      u'buyCost': 107422,
      u'buyLots': 7,
      u'contr': u'GBPUSD',
      u'sellCost': 107422,
      u'sellLots': 7,
      u'settlDate': 20140302
    }, {
      u'accnt': u'MARAYL',
      u'buyCost': 61730,
      u'buyLots': 5,
      u'contr': u'EURUSD',
      u'sellCost': 61730,
      u'sellLots': 5,
      u'settlDate': 20140402
    }], resp.content)

  def getByContr(self, conn):
    conn.setTrader('MARAYL')
    resp = conn.send('GET', '/accnt/posn/EURUSD')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'buyCost': 37038,
      u'buyLots': 3,
      u'contr': u'EURUSD',
      u'sellCost': 37038,
      u'sellLots': 3,
      u'settlDate': 20140302
    }, {
      u'accnt': u'MARAYL',
      u'buyCost': 61730,
      u'buyLots': 5,
      u'contr': u'EURUSD',
      u'sellCost': 61730,
      u'sellLots': 5,
      u'settlDate': 20140402
    }], resp.content)

  def getBySettlDate(self, conn):
    conn.setTrader('MARAYL')
    resp = conn.send('GET', '/accnt/posn/EURUSD/20140302')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'accnt': u'MARAYL',
      u'buyCost': 37038,
      u'buyLots': 3,
      u'contr': u'EURUSD',
      u'sellCost': 37038,
      u'sellLots': 3,
      u'settlDate': 20140302
    }, resp.content)

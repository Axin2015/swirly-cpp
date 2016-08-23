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

        conn.setAuth('ADMIN', 0x1)
        self.createMarket(conn, 'EURUSD.MAR14', 'EURUSD', 20140302, 20140301)
        self.createMarket(conn, 'GBPUSD.MAR14', 'GBPUSD', 20140302, 20140301)
        self.createMarket(conn, 'USDCHF.MAR14', 'USDCHF', 20140302, 20140301)

        self.getAll(conn)
        self.getByMnem(conn)

  def getAll(self, conn):
    resp = conn.send('GET', '/rec/market')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'contr': u'EURUSD',
      u'display': u'EURUSD.MAR14',
      u'expiryDate': 20140301,
      u'mnem': u'EURUSD.MAR14',
      u'settlDate': 20140302,
      u'state': 0
    }, {
      u'contr': u'GBPUSD',
      u'display': u'GBPUSD.MAR14',
      u'expiryDate': 20140301,
      u'mnem': u'GBPUSD.MAR14',
      u'settlDate': 20140302,
      u'state': 0
    }, {
      u'contr': u'USDCHF',
      u'display': u'USDCHF.MAR14',
      u'expiryDate': 20140301,
      u'mnem': u'USDCHF.MAR14',
      u'settlDate': 20140302,
      u'state': 0
    }], resp.content)

  def getByMnem(self, conn):
    resp = conn.send('GET', '/rec/market/EURUSD.MAR14')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'contr': u'EURUSD',
      u'display': u'EURUSD.MAR14',
      u'expiryDate': 20140301,
      u'mnem': u'EURUSD.MAR14',
      u'settlDate': 20140302,
      u'state': 0
    }, resp.content)

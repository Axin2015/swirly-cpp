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

        self.checkAuth(conn)

        self.createMarket(conn)
        self.updateDisplayAndState(conn)
        self.updateDisplayOnly(conn)
        self.updateStateOnly(conn)

  def checkAuth(self, conn):
    conn.setAuth(None, 0x1)
    resp = conn.send('POST', '/rec/market')

    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    resp = conn.send('PUT', '/rec/market/USDJPY.MAR14')

    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    conn.setAuth('ADMIN', ~0x1 & 0x7fffffff)
    resp = conn.send('POST', '/rec/market')

    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

    resp = conn.send('PUT', '/rec/market/USDJPY.MAR14')

    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def createMarket(self, conn):
    conn.setAdmin()
    resp = conn.send('POST', '/rec/market',
                     mnem = 'USDJPY.MAR14',
                     display = 'first',
                     contr = 'USDJPY',
                     settlDate = 20140302,
                     state = 1)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'contr': u'USDJPY',
      u'display': u'first',
      u'mnem': u'USDJPY.MAR14',
      u'settlDate': 20140302,
      u'state': 1
    }, resp.content)

  def updateDisplayAndState(self, conn):
    conn.setAdmin()
    resp = conn.send('PUT', '/rec/market/USDJPY.MAR14',
                     display = 'second',
                     state = 2)
    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'contr': u'USDJPY',
      u'display': u'second',
      u'mnem': u'USDJPY.MAR14',
      u'settlDate': 20140302,
      u'state': 2
    }, resp.content)

  def updateDisplayOnly(self, conn):
    conn.setAdmin()
    resp = conn.send('PUT', '/rec/market/USDJPY.MAR14',
                     display = 'third',
                     state = None)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'contr': u'USDJPY',
      u'display': u'third',
      u'mnem': u'USDJPY.MAR14',
      u'settlDate': 20140302,
      u'state': 2
    }, resp.content)

  def updateStateOnly(self, conn):
    conn.setAdmin()
    resp = conn.send('PUT', '/rec/market/USDJPY.MAR14',
                     display = None,
                     state = 3)
    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'contr': u'USDJPY',
      u'display': u'third',
      u'mnem': u'USDJPY.MAR14',
      u'settlDate': 20140302,
      u'state': 3
    }, resp.content)

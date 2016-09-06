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
        self.updateMarket(conn)

  def checkAuth(self, conn):
    conn.setAuth(None, 0x1)
    resp = conn.send('POST', '/market')

    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    resp = conn.send('PUT', '/market/USDJPY/20140302')

    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    conn.setAuth('ADMIN', ~0x1 & 0x7fffffff)
    resp = conn.send('POST', '/market')

    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

    resp = conn.send('PUT', '/market/USDJPY/20140302')

    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def createMarket(self, conn):
    conn.setAdmin()
    resp = conn.send('POST', '/market',
                     contr = 'USDJPY',
                     settlDate = 20140302,
                     state = 1)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'bidCount': [None, None, None],
      u'bidResd': [None, None, None],
      u'bidTicks': [None, None, None],
      u'contr': u'USDJPY',
      u'lastLots': None,
      u'lastTicks': None,
      u'lastTime': None,
      u'offerCount': [None, None, None],
      u'offerResd': [None, None, None],
      u'offerTicks': [None, None, None],
      u'settlDate': 20140302,
      u'state': 1
    }, resp.content)

  def updateMarket(self, conn):
    conn.setAdmin()
    resp = conn.send('PUT', '/market/USDJPY/20140302',
                     state = 2)
    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'bidCount': [None, None, None],
      u'bidResd': [None, None, None],
      u'bidTicks': [None, None, None],
      u'contr': u'USDJPY',
      u'lastLots': None,
      u'lastTicks': None,
      u'lastTime': None,
      u'offerCount': [None, None, None],
      u'offerResd': [None, None, None],
      u'offerTicks': [None, None, None],
      u'settlDate': 20140302,
      u'state': 2
    }, resp.content)

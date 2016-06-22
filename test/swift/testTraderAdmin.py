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
    self.now = 1459974268204
    with Fixture() as fixture:
      with Connection() as conn:
        conn.setTime(self.now)
        conn.setAuth('emailAddress=mark.aylett@swirlycloud.com')

        self.createTrader(conn)
        self.updateTrader(conn)

  def createTrader(self, conn):
    resp = conn.send('POST', '/api/rec/trader',
                     mnem = 'PIPAYL',
                     display = 'Pippin Aylettx',
                     email = 'pippin.aylett@swirlycloud.com')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'display': u'Pippin Aylettx',
      u'email': u'pippin.aylett@swirlycloud.com',
      u'mnem': u'PIPAYL'
    }, resp.content)

  def updateTrader(self, conn):
    resp = conn.send('PUT', '/api/rec/trader/PIPAYL',
                     display = 'Pippin Aylett')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'display': u'Pippin Aylett',
      u'email': u'pippin.aylett@swirlycloud.com',
      u'mnem': u'PIPAYL'
    }, resp.content)

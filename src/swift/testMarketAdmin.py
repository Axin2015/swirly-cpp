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
    self.max_diff = None
    self.now = 1388534400000
    with DbFile() as db_file:
      with Server(db_file, self.now) as server:
        with Client() as client:
          client.set_time(self.now)

          self.create_market(client)
          self.check_auth(client)
          self.update_market(client)

  def create_market(self, client):
    client.set_admin()
    resp = client.send('POST', '/api/market',
                       instr = 'EURUSD',
                       settl_date = 20140302,
                       state = 1)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.eurusd_id = int(resp.content['id'])
    self.assertDictEqual({
      u'bid_count': [None, None, None],
      u'bid_lots': [None, None, None],
      u'bid_ticks': [None, None, None],
      u'instr': u'EURUSD',
      u'id': self.eurusd_id,
      u'last_lots': None,
      u'last_ticks': None,
      u'last_time': None,
      u'offer_count': [None, None, None],
      u'offer_lots': [None, None, None],
      u'offer_ticks': [None, None, None],
      u'settl_date': 20140302,
      u'state': 1
    }, resp.content)

  def check_auth(self, client):
    client.set_auth(None, 0x1)

    resp = client.send('POST', '/api/market')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    resp = client.send('POST', '/api/market/' + str(self.eurusd_id))
    self.assertEqual(405, resp.status)
    self.assertEqual('Method Not Allowed', resp.reason)

    resp = client.send('PUT', '/api/market/' + str(self.eurusd_id))
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    client.set_auth('ADMIN', ~0x1 & 0x7fffffff)

    resp = client.send('POST', '/api/market')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

    resp = client.send('POST', '/api/market/' + str(self.eurusd_id))
    self.assertEqual(405, resp.status)
    self.assertEqual('Method Not Allowed', resp.reason)

    resp = client.send('PUT', '/api/market/' + str(self.eurusd_id))
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def update_market(self, client):
    client.set_admin()
    resp = client.send('PUT', '/api/market/' + str(self.eurusd_id),
                       state = 2)
    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'bid_count': [None, None, None],
      u'bid_lots': [None, None, None],
      u'bid_ticks': [None, None, None],
      u'instr': u'EURUSD',
      u'id': self.eurusd_id,
      u'last_lots': None,
      u'last_ticks': None,
      u'last_time': None,
      u'offer_count': [None, None, None],
      u'offer_lots': [None, None, None],
      u'offer_ticks': [None, None, None],
      u'settl_date': 20140302,
      u'state': 2
    }, resp.content)

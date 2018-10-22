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

          self.eurusd_id = self.create_market(client, 'EURUSD', 20140302)

          self.check_auth(client)

          self.create_bid(client)
          self.create_offer(client)

  def check_auth(self, client):
    client.set_auth(None, 0x2)

    resp = client.send('POST', '/api/sess/order')
    self.assertEqual(405, resp.status)
    self.assertEqual('Method Not Allowed', resp.reason)

    resp = client.send('POST', '/api/sess/order/' + str(self.eurusd_id))
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    client.set_auth('MARAYL', ~0x2 & 0x7fffffff)

    resp = client.send('POST', '/api/sess/order')
    self.assertEqual(405, resp.status)
    self.assertEqual('Method Not Allowed', resp.reason)

    resp = client.send('POST', '/api/sess/order/' + str(self.eurusd_id))
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def create_bid(self, client):
    client.set_trader('MARAYL')
    resp = client.send('POST', '/api/sess/order/' + str(self.eurusd_id),
                       side = 'Buy',
                       lots = 3,
                       ticks = 12344)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'market': {
        u'bid_count': [1, None, None],
        u'bid_lots': [3, None, None],
        u'bid_ticks': [12344, None, None],
        u'instr': u'EURUSD',
        u'id': self.eurusd_id,
        u'last_lots': None,
        u'last_ticks': None,
        u'last_time': None,
        u'offer_count': [None, None, None],
        u'offer_lots': [None, None, None],
        u'offer_ticks': [None, None, None],
        u'settl_date': 20140302,
        u'state': 0
      },
      u'execs': [{
        u'accnt': u'MARAYL',
        u'cpty': None,
        u'created': self.now,
        u'exec_cost': 0,
        u'exec_lots': 0,
        u'id': 1,
        u'last_lots': None,
        u'last_ticks': None,
        u'liq_ind': None,
        u'lots': 3,
        u'market_id': self.eurusd_id,
        u'match_id': None,
        u'min_lots': None,
        u'order_id': 1,
        u'posn_cost': None,
        u'posn_lots': None,
        u'ref': None,
        u'resd_lots': 3,
        u'side': u'Buy',
        u'state': u'New',
        u'ticks': 12344
      }],
      u'orders': [{
        u'accnt': u'MARAYL',
        u'created': self.now,
        u'exec_cost': 0,
        u'exec_lots': 0,
        u'id': 1,
        u'last_lots': None,
        u'last_ticks': None,
        u'lots': 3,
        u'market_id': self.eurusd_id,
        u'min_lots': None,
        u'modified': self.now,
        u'ref': None,
        u'resd_lots': 3,
        u'side': u'Buy',
        u'state': u'New',
        u'ticks': 12344
      }],
      u'posn': None
    }, resp.content)

  def create_offer(self, client):
    client.set_trader('MARAYL')
    resp = client.send('POST', '/api/sess/order/' + str(self.eurusd_id),
                       side = 'Sell',
                       lots = 3,
                       ticks = 12346)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'market': {
        u'bid_count': [1, None, None],
        u'bid_lots': [3, None, None],
        u'bid_ticks': [12344, None, None],
        u'instr': u'EURUSD',
        u'id': self.eurusd_id,
        u'last_lots': None,
        u'last_ticks': None,
        u'last_time': None,
        u'offer_count': [1, None, None],
        u'offer_lots': [3, None, None],
        u'offer_ticks': [12346, None, None],
        u'settl_date': 20140302,
        u'state': 0
      },
      u'execs': [{
        u'accnt': u'MARAYL',
        u'cpty': None,
        u'created': self.now,
        u'exec_cost': 0,
        u'exec_lots': 0,
        u'id': 2,
        u'last_lots': None,
        u'last_ticks': None,
        u'liq_ind': None,
        u'lots': 3,
        u'market_id': self.eurusd_id,
        u'match_id': None,
        u'min_lots': None,
        u'order_id': 2,
        u'posn_cost': None,
        u'posn_lots': None,
        u'ref': None,
        u'resd_lots': 3,
        u'side': u'Sell',
        u'state': u'New',
        u'ticks': 12346
      }],
      u'orders': [{
        u'accnt': u'MARAYL',
        u'created': self.now,
        u'exec_cost': 0,
        u'exec_lots': 0,
        u'id': 2,
        u'last_lots': None,
        u'last_ticks': None,
        u'lots': 3,
        u'market_id': self.eurusd_id,
        u'min_lots': None,
        u'modified': self.now,
        u'ref': None,
        u'resd_lots': 3,
        u'side': u'Sell',
        u'state': u'New',
        u'ticks': 12346
      }],
      u'posn': None
    }, resp.content)

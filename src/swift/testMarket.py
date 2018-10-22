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
          self.gbpusd_id = self.create_market(client, 'GBPUSD', 20140302)
          self.gbpusd2_id = self.create_market(client, 'GBPUSD', 20140402)
          self.usdchf_id = self.create_market(client, 'USDCHF', 20140302)

          self.create_order(client, 'MARAYL', self.eurusd_id, 'Sell', 7, 12348)
          self.create_order(client, 'MARAYL', self.eurusd_id, 'Sell', 5, 12347)
          self.create_order(client, 'MARAYL', self.eurusd_id, 'Sell', 3, 12346)
          self.create_order(client, 'MARAYL', self.eurusd_id, 'Buy', 3, 12344)
          self.create_order(client, 'MARAYL', self.eurusd_id, 'Buy', 5, 12343)
          self.create_order(client, 'MARAYL', self.eurusd_id, 'Buy', 7, 12343)

          self.create_order(client, 'MARAYL', self.gbpusd_id, 'Sell', 5, 15346)
          self.create_order(client, 'MARAYL', self.gbpusd_id, 'Sell', 3, 15346)
          self.create_order(client, 'MARAYL', self.gbpusd_id, 'Buy', 3, 15344)
          self.create_order(client, 'MARAYL', self.gbpusd_id, 'Buy', 5, 15343)

          self.create_order(client, 'MARAYL', self.gbpusd2_id, 'Sell', 7, 15347)
          self.create_order(client, 'MARAYL', self.gbpusd2_id, 'Buy', 7, 15342)

          self.create_order(client, 'MARAYL', self.usdchf_id, 'Sell', 3, 9346)
          self.create_order(client, 'MARAYL', self.usdchf_id, 'Buy', 3, 9344)

          self.get_all(client)
          self.get_by_market(client)

      with Server(db_file, self.now) as server:
        with Client() as client:
          client.set_time(self.now)

          self.get_all(client)
          self.get_by_market(client)

  def get_all(self, client):
    client.set_anon()
    resp = client.send('GET', '/api/market')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'bid_count': [1, 2, None],
      u'bid_lots': [3, 12, None],
      u'bid_ticks': [12344, 12343, None],
      u'instr': u'EURUSD',
      u'id': self.eurusd_id,
      u'last_lots': None,
      u'last_ticks': None,
      u'last_time': None,
      u'offer_count': [1, 1, 1],
      u'offer_lots': [3, 5, 7],
      u'offer_ticks': [12346, 12347, 12348],
      u'settl_date': 20140302,
      u'state': 0
    }, {
      u'bid_count': [1, 1, None],
      u'bid_lots': [3, 5, None],
      u'bid_ticks': [15344, 15343, None],
      u'instr': u'GBPUSD',
      u'id': self.gbpusd_id,
      u'last_lots': None,
      u'last_ticks': None,
      u'last_time': None,
      u'offer_count': [2, None, None],
      u'offer_lots': [8, None, None],
      u'offer_ticks': [15346, None, None],
      u'settl_date': 20140302,
      u'state': 0
    }, {
      u'bid_count': [1, None, None],
      u'bid_lots': [7, None, None],
      u'bid_ticks': [15342, None, None],
      u'instr': u'GBPUSD',
      u'id': self.gbpusd2_id,
      u'last_lots': None,
      u'last_ticks': None,
      u'last_time': None,
      u'offer_count': [1, None, None],
      u'offer_lots': [7, None, None],
      u'offer_ticks': [15347, None, None],
      u'settl_date': 20140402,
      u'state': 0
    }, {
      u'bid_count': [1, None, None],
      u'bid_lots': [3, None, None],
      u'bid_ticks': [9344, None, None],
      u'instr': u'USDCHF',
      u'id': self.usdchf_id,
      u'last_lots': None,
      u'last_ticks': None,
      u'last_time': None,
      u'offer_count': [1, None, None],
      u'offer_lots': [3, None, None],
      u'offer_ticks': [9346, None, None],
      u'settl_date': 20140302,
      u'state': 0
    }], resp.content)

  def get_by_market(self, client):
    client.set_anon()
    resp = client.send('GET', '/api/market/' + str(self.gbpusd_id))

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'bid_count': [1, 1, None],
      u'bid_lots': [3, 5, None],
      u'bid_ticks': [15344, 15343, None],
      u'instr': u'GBPUSD',
      u'id': self.gbpusd_id,
      u'last_lots': None,
      u'last_ticks': None,
      u'last_time': None,
      u'offer_count': [2, None, None],
      u'offer_lots': [8, None, None],
      u'offer_ticks': [15346, None, None],
      u'settl_date': 20140302,
      u'state': 0
    }, resp.content)

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
          self.eurusd2_id = self.create_market(client, 'EURUSD', 20140402)
          self.gbpusd_id = self.create_market(client, 'GBPUSD', 20140302)

          self.create_order(client, 'MARAYL', self.eurusd_id, 'Sell', 3, 12346)
          self.create_order(client, 'MARAYL', self.eurusd_id, 'Buy', 3, 12346)

          self.create_order(client, 'MARAYL', self.eurusd2_id, 'Sell', 5, 12346)
          self.create_order(client, 'MARAYL', self.eurusd2_id, 'Buy', 5, 12346)

          self.create_order(client, 'MARAYL', self.gbpusd_id, 'Sell', 7, 15346)
          self.create_order(client, 'MARAYL', self.gbpusd_id, 'Buy', 7, 15346)

          self.check_auth(client)

          self.get_all(client)
          self.get_by_market(client)

      with Server(db_file, self.now) as server:
        with Client() as client:
          client.set_time(self.now)

          self.get_all(client)
          self.get_by_market(client)

  def check_auth(self, client):
    client.set_auth(None, 0x2)

    resp = client.send('GET', '/api/sess/posn')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    resp = client.send('GET', '/api/sess/posn/' + str(self.eurusd_id))
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    client.set_auth('MARAYL', ~0x2 & 0x7fffffff)

    resp = client.send('GET', '/api/sess/posn')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

    resp = client.send('GET', '/api/sess/posn/' + str(self.eurusd_id))
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def get_all(self, client):
    client.set_trader('MARAYL')
    resp = client.send('GET', '/api/sess/posn')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'buy_cost': 37038,
      u'buy_lots': 3,
      u'market_id': self.eurusd_id,
      u'open_cost': 0,
      u'sell_cost': 37038,
      u'sell_lots': 3
    }, {
      u'accnt': u'MARAYL',
      u'buy_cost': 61730,
      u'buy_lots': 5,
      u'market_id': self.eurusd2_id,
      u'open_cost': 0,
      u'sell_cost': 61730,
      u'sell_lots': 5
    }, {
      u'accnt': u'MARAYL',
      u'buy_cost': 107422,
      u'buy_lots': 7,
      u'market_id': self.gbpusd_id,
      u'open_cost': 0,
      u'sell_cost': 107422,
      u'sell_lots': 7
    }], resp.content)

  def get_by_instr(self, client):
    client.set_trader('MARAYL')
    resp = client.send('GET', '/api/sess/posn/EURUSD')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'buy_cost': 37038,
      u'buy_lots': 3,
      u'market_id': self.eurusd_id,
      u'open_cost': 0,
      u'sell_cost': 37038,
      u'sell_lots': 3
    }, {
      u'accnt': u'MARAYL',
      u'buy_cost': 61730,
      u'buy_lots': 5,
      u'market_id': self.eurusd2_id,
      u'open_cost': 0,
      u'sell_cost': 61730,
      u'sell_lots': 5
    }], resp.content)

  def get_by_market(self, client):
    client.set_trader('MARAYL')
    resp = client.send('GET', '/api/sess/posn/' + str(self.eurusd_id))

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'accnt': u'MARAYL',
      u'buy_cost': 37038,
      u'buy_lots': 3,
      u'market_id': self.eurusd_id,
      u'open_cost': 0,
      u'sell_cost': 37038,
      u'sell_lots': 3
    }, resp.content)

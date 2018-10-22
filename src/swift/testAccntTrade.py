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

          self.create_order(client, 'MARAYL', self.eurusd2_id, 'Sell', 5, 12347)
          self.create_order(client, 'MARAYL', self.eurusd2_id, 'Buy', 5, 12347)

          self.create_order(client, 'MARAYL', self.gbpusd_id, 'Sell', 3, 15346)
          self.create_order(client, 'MARAYL', self.gbpusd_id, 'Buy', 3, 15346)

          self.check_auth(client)

          self.get_all(client)
          self.get_by_market(client)
          self.get_by_id(client)

  def check_auth(self, client):
    client.set_auth(None, 0x2)

    resp = client.send('GET', '/api/sess/trade')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    resp = client.send('GET', '/api/sess/trade/' + str(self.eurusd_id))
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    client.set_auth('MARAYL', ~0x2 & 0x7fffffff)

    resp = client.send('GET', '/api/sess/trade')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

    resp = client.send('GET', '/api/sess/trade/' + str(self.eurusd_id))
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def get_all(self, client):
    client.set_trader('MARAYL')
    resp = client.send('GET', '/api/sess/trade')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 37038,
      u'exec_lots': 3,
      u'id': 3,
      u'last_lots': 3,
      u'last_ticks': 12346,
      u'liq_ind': u'Maker',
      u'lots': 3,
      u'market_id': self.eurusd_id,
      u'match_id': 4,
      u'min_lots': None,
      u'order_id': 1,
      u'posn_cost': 0,
      u'posn_lots': 0,
      u'ref': None,
      u'resd_lots': 0,
      u'side': u'Sell',
      u'state': u'Trade',
      u'ticks': 12346
    }, {
      u'accnt': u'MARAYL',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 37038,
      u'exec_lots': 3,
      u'id': 4,
      u'last_lots': 3,
      u'last_ticks': 12346,
      u'liq_ind': u'Taker',
      u'lots': 3,
      u'market_id': self.eurusd_id,
      u'match_id': 3,
      u'min_lots': None,
      u'order_id': 2,
      u'posn_cost': -37038,
      u'posn_lots': -3,
      u'ref': None,
      u'resd_lots': 0,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 12346
    }, {
      u'accnt': u'MARAYL',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 61735,
      u'exec_lots': 5,
      u'id': 3,
      u'last_lots': 5,
      u'last_ticks': 12347,
      u'liq_ind': u'Maker',
      u'lots': 5,
      u'market_id': self.eurusd2_id,
      u'match_id': 4,
      u'min_lots': None,
      u'order_id': 1,
      u'posn_cost': 0,
      u'posn_lots': 0,
      u'ref': None,
      u'resd_lots': 0,
      u'side': u'Sell',
      u'state': u'Trade',
      u'ticks': 12347
    }, {
      u'accnt': u'MARAYL',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 61735,
      u'exec_lots': 5,
      u'id': 4,
      u'last_lots': 5,
      u'last_ticks': 12347,
      u'liq_ind': u'Taker',
      u'lots': 5,
      u'market_id': self.eurusd2_id,
      u'match_id': 3,
      u'min_lots': None,
      u'order_id': 2,
      u'posn_cost': -61735,
      u'posn_lots': -5,
      u'ref': None,
      u'resd_lots': 0,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 12347
    }, {
      u'accnt': u'MARAYL',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 46038,
      u'exec_lots': 3,
      u'id': 3,
      u'last_lots': 3,
      u'last_ticks': 15346,
      u'liq_ind': u'Maker',
      u'lots': 3,
      u'market_id': self.gbpusd_id,
      u'match_id': 4,
      u'min_lots': None,
      u'order_id': 1,
      u'posn_cost': 0,
      u'posn_lots': 0,
      u'ref': None,
      u'resd_lots': 0,
      u'side': u'Sell',
      u'state': u'Trade',
      u'ticks': 15346
    }, {
      u'accnt': u'MARAYL',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 46038,
      u'exec_lots': 3,
      u'id': 4,
      u'last_lots': 3,
      u'last_ticks': 15346,
      u'liq_ind': u'Taker',
      u'lots': 3,
      u'market_id': self.gbpusd_id,
      u'match_id': 3,
      u'min_lots': None,
      u'order_id': 2,
      u'posn_cost': -46038,
      u'posn_lots': -3,
      u'ref': None,
      u'resd_lots': 0,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 15346
    }], resp.content)

  def get_by_market(self, client):
    client.set_trader('MARAYL')
    resp = client.send('GET', '/api/sess/trade/' + str(self.gbpusd_id))

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 46038,
      u'exec_lots': 3,
      u'id': 3,
      u'last_lots': 3,
      u'last_ticks': 15346,
      u'liq_ind': u'Maker',
      u'lots': 3,
      u'market_id': self.gbpusd_id,
      u'match_id': 4,
      u'min_lots': None,
      u'order_id': 1,
      u'posn_cost': 0,
      u'posn_lots': 0,
      u'ref': None,
      u'resd_lots': 0,
      u'side': u'Sell',
      u'state': u'Trade',
      u'ticks': 15346
    }, {
      u'accnt': u'MARAYL',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 46038,
      u'exec_lots': 3,
      u'id': 4,
      u'last_lots': 3,
      u'last_ticks': 15346,
      u'liq_ind': u'Taker',
      u'lots': 3,
      u'market_id': self.gbpusd_id,
      u'match_id': 3,
      u'min_lots': None,
      u'order_id': 2,
      u'posn_cost': -46038,
      u'posn_lots': -3,
      u'ref': None,
      u'resd_lots': 0,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 15346
    }], resp.content)

  def get_by_id(self, client):
    client.set_trader('MARAYL')
    resp = client.send('GET', '/api/sess/trade/{}/3'.format(self.gbpusd_id))

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'accnt': u'MARAYL',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 46038,
      u'exec_lots': 3,
      u'id': 3,
      u'last_lots': 3,
      u'last_ticks': 15346,
      u'liq_ind': u'Maker',
      u'lots': 3,
      u'market_id': self.gbpusd_id,
      u'match_id': 4,
      u'min_lots': None,
      u'order_id': 1,
      u'posn_cost': 0,
      u'posn_lots': 0,
      u'ref': None,
      u'resd_lots': 0,
      u'side': u'Sell',
      u'state': u'Trade',
      u'ticks': 15346
    }, resp.content)

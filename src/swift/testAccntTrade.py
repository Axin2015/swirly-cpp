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

          self.create_market(client, 'EURUSD', 20140302)
          self.create_market(client, 'EURUSD', 20140402)
          self.create_market(client, 'GBPUSD', 20140302)

          self.create_order(client, 'MARAYL', 'EURUSD', 20140302, 'Sell', 3, 12346)
          self.create_order(client, 'MARAYL', 'EURUSD', 20140302, 'Buy', 3, 12346)

          self.create_order(client, 'MARAYL', 'EURUSD', 20140402, 'Sell', 5, 12347)
          self.create_order(client, 'MARAYL', 'EURUSD', 20140402, 'Buy', 5, 12347)

          self.create_order(client, 'MARAYL', 'GBPUSD', 20140302, 'Sell', 3, 15346)
          self.create_order(client, 'MARAYL', 'GBPUSD', 20140302, 'Buy', 3, 15346)

          self.check_auth(client)

          self.get_all(client)
          self.get_by_instr(client)
          self.get_by_market(client)
          self.get_by_id(client)

  def check_auth(self, client):
    client.set_auth(None, 0x2)

    resp = client.send('GET', '/api/sess/trade')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    resp = client.send('GET', '/api/sess/trade/EURUSD')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    resp = client.send('GET', '/api/sess/trade/EURUSD/20140302')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    client.set_auth('MARAYL', ~0x2 & 0x7fffffff)

    resp = client.send('GET', '/api/sess/trade')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

    resp = client.send('GET', '/api/sess/trade/EURUSD')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

    resp = client.send('GET', '/api/sess/trade/EURUSD/20140302')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def get_all(self, client):
    client.set_trader('MARAYL')
    resp = client.send('GET', '/api/sess/trade')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 37038,
      u'exec_lots': 3,
      u'id': 3,
      u'last_lots': 3,
      u'last_ticks': 12346,
      u'liq_ind': u'Maker',
      u'lots': 3,
      u'market_id': 82255,
      u'match_id': 4,
      u'min_lots': None,
      u'order_id': 1,
      u'posn_cost': 0,
      u'posn_lots': 0,
      u'ref': None,
      u'resd_lots': 0,
      u'settl_date': 20140302,
      u'side': u'Sell',
      u'state': u'Trade',
      u'ticks': 12346
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 37038,
      u'exec_lots': 3,
      u'id': 4,
      u'last_lots': 3,
      u'last_ticks': 12346,
      u'liq_ind': u'Taker',
      u'lots': 3,
      u'market_id': 82255,
      u'match_id': 3,
      u'min_lots': None,
      u'order_id': 2,
      u'posn_cost': -37038,
      u'posn_lots': -3,
      u'ref': None,
      u'resd_lots': 0,
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 12346
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 61735,
      u'exec_lots': 5,
      u'id': 3,
      u'last_lots': 5,
      u'last_ticks': 12347,
      u'liq_ind': u'Maker',
      u'lots': 5,
      u'market_id': 82286,
      u'match_id': 4,
      u'min_lots': None,
      u'order_id': 1,
      u'posn_cost': 0,
      u'posn_lots': 0,
      u'ref': None,
      u'resd_lots': 0,
      u'settl_date': 20140402,
      u'side': u'Sell',
      u'state': u'Trade',
      u'ticks': 12347
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 61735,
      u'exec_lots': 5,
      u'id': 4,
      u'last_lots': 5,
      u'last_ticks': 12347,
      u'liq_ind': u'Taker',
      u'lots': 5,
      u'market_id': 82286,
      u'match_id': 3,
      u'min_lots': None,
      u'order_id': 2,
      u'posn_cost': -61735,
      u'posn_lots': -5,
      u'ref': None,
      u'resd_lots': 0,
      u'settl_date': 20140402,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 12347
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'GBPUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 46038,
      u'exec_lots': 3,
      u'id': 3,
      u'last_lots': 3,
      u'last_ticks': 15346,
      u'liq_ind': u'Maker',
      u'lots': 3,
      u'market_id': 147791,
      u'match_id': 4,
      u'min_lots': None,
      u'order_id': 1,
      u'posn_cost': 0,
      u'posn_lots': 0,
      u'ref': None,
      u'resd_lots': 0,
      u'settl_date': 20140302,
      u'side': u'Sell',
      u'state': u'Trade',
      u'ticks': 15346
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'GBPUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 46038,
      u'exec_lots': 3,
      u'id': 4,
      u'last_lots': 3,
      u'last_ticks': 15346,
      u'liq_ind': u'Taker',
      u'lots': 3,
      u'market_id': 147791,
      u'match_id': 3,
      u'min_lots': None,
      u'order_id': 2,
      u'posn_cost': -46038,
      u'posn_lots': -3,
      u'ref': None,
      u'resd_lots': 0,
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 15346
    }], resp.content)

  def get_by_instr(self, client):
    client.set_trader('MARAYL')
    resp = client.send('GET', '/api/sess/trade/EURUSD')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 37038,
      u'exec_lots': 3,
      u'id': 3,
      u'last_lots': 3,
      u'last_ticks': 12346,
      u'liq_ind': u'Maker',
      u'lots': 3,
      u'market_id': 82255,
      u'match_id': 4,
      u'min_lots': None,
      u'order_id': 1,
      u'posn_cost': 0,
      u'posn_lots': 0,
      u'ref': None,
      u'resd_lots': 0,
      u'settl_date': 20140302,
      u'side': u'Sell',
      u'state': u'Trade',
      u'ticks': 12346
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 37038,
      u'exec_lots': 3,
      u'id': 4,
      u'last_lots': 3,
      u'last_ticks': 12346,
      u'liq_ind': u'Taker',
      u'lots': 3,
      u'market_id': 82255,
      u'match_id': 3,
      u'min_lots': None,
      u'order_id': 2,
      u'posn_cost': -37038,
      u'posn_lots': -3,
      u'ref': None,
      u'resd_lots': 0,
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 12346
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 61735,
      u'exec_lots': 5,
      u'id': 3,
      u'last_lots': 5,
      u'last_ticks': 12347,
      u'liq_ind': u'Maker',
      u'lots': 5,
      u'market_id': 82286,
      u'match_id': 4,
      u'min_lots': None,
      u'order_id': 1,
      u'posn_cost': 0,
      u'posn_lots': 0,
      u'ref': None,
      u'resd_lots': 0,
      u'settl_date': 20140402,
      u'side': u'Sell',
      u'state': u'Trade',
      u'ticks': 12347
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 61735,
      u'exec_lots': 5,
      u'id': 4,
      u'last_lots': 5,
      u'last_ticks': 12347,
      u'liq_ind': u'Taker',
      u'lots': 5,
      u'market_id': 82286,
      u'match_id': 3,
      u'min_lots': None,
      u'order_id': 2,
      u'posn_cost': -61735,
      u'posn_lots': -5,
      u'ref': None,
      u'resd_lots': 0,
      u'settl_date': 20140402,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 12347
    }], resp.content)

  def get_by_market(self, client):
    client.set_trader('MARAYL')
    resp = client.send('GET', '/api/sess/trade/GBPUSD/20140302')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'GBPUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 46038,
      u'exec_lots': 3,
      u'id': 3,
      u'last_lots': 3,
      u'last_ticks': 15346,
      u'liq_ind': u'Maker',
      u'lots': 3,
      u'market_id': 147791,
      u'match_id': 4,
      u'min_lots': None,
      u'order_id': 1,
      u'posn_cost': 0,
      u'posn_lots': 0,
      u'ref': None,
      u'resd_lots': 0,
      u'settl_date': 20140302,
      u'side': u'Sell',
      u'state': u'Trade',
      u'ticks': 15346
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'GBPUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 46038,
      u'exec_lots': 3,
      u'id': 4,
      u'last_lots': 3,
      u'last_ticks': 15346,
      u'liq_ind': u'Taker',
      u'lots': 3,
      u'market_id': 147791,
      u'match_id': 3,
      u'min_lots': None,
      u'order_id': 2,
      u'posn_cost': -46038,
      u'posn_lots': -3,
      u'ref': None,
      u'resd_lots': 0,
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 15346
    }], resp.content)

  def get_by_id(self, client):
    client.set_trader('MARAYL')
    resp = client.send('GET', '/api/sess/trade/GBPUSD/20140302/3')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'accnt': u'MARAYL',
      u'instr': u'GBPUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 46038,
      u'exec_lots': 3,
      u'id': 3,
      u'last_lots': 3,
      u'last_ticks': 15346,
      u'liq_ind': u'Maker',
      u'lots': 3,
      u'market_id': 147791,
      u'match_id': 4,
      u'min_lots': None,
      u'order_id': 1,
      u'posn_cost': 0,
      u'posn_lots': 0,
      u'ref': None,
      u'resd_lots': 0,
      u'settl_date': 20140302,
      u'side': u'Sell',
      u'state': u'Trade',
      u'ticks': 15346
    }, resp.content)

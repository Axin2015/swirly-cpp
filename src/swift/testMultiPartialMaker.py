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

          self.create_order(client, 'MARAYL', 'EURUSD', 20140302, 'Buy', 3, 12345)
          self.create_order(client, 'MARAYL', 'EURUSD', 20140302, 'Buy', 5, 12344)
          self.create_order(client, 'MARAYL', 'EURUSD', 20140302, 'Buy', 7, 12344)

          self.take_order(client)

          self.maker_order(client)
          self.maker_exec(client)
          self.maker_trade(client)
          self.maker_posn(client)

  def take_order(self, client):
    client.set_trader('GOSAYL')
    resp = client.send('POST', '/api/sess/order/EURUSD/20140302',
                       side = 'Sell',
                       lots = 11,
                       ticks = 12344)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'market': {
        u'bid_count': [1, None, None],
        u'bid_lots': [4, None, None],
        u'bid_ticks': [12344, None, None],
        u'instr': u'EURUSD',
        u'id': 82255,
        u'last_lots': 3,
        u'last_ticks': 12344,
        u'last_time': self.now,
        u'offer_count': [None, None, None],
        u'offer_lots': [None, None, None],
        u'offer_ticks': [None, None, None],
        u'settl_date': 20140302,
        u'state': 0
      },
      u'execs': [{
        u'accnt': u'GOSAYL',
        u'instr': u'EURUSD',
        u'cpty': u'MARAYL',
        u'created': self.now,
        u'exec_cost': 135787,
        u'exec_lots': 11,
        u'id': 10,
        u'last_lots': 3,
        u'last_ticks': 12344,
        u'liq_ind': u'Taker',
        u'lots': 11,
        u'market_id': 82255,
        u'match_id': 9,
        u'min_lots': None,
        u'order_id': 4,
        u'posn_cost': -98755,
        u'posn_lots': -8,
        u'ref': None,
        u'resd_lots': 0,
        u'settl_date': 20140302,
        u'side': u'Sell',
        u'state': u'Trade',
        u'ticks': 12344
      }, {
        u'accnt': u'GOSAYL',
        u'instr': u'EURUSD',
        u'cpty': u'MARAYL',
        u'created': self.now,
        u'exec_cost': 98755,
        u'exec_lots': 8,
        u'id': 8,
        u'last_lots': 5,
        u'last_ticks': 12344,
        u'liq_ind': u'Taker',
        u'lots': 11,
        u'market_id': 82255,
        u'match_id': 7,
        u'min_lots': None,
        u'order_id': 4,
        u'posn_cost': -37035,
        u'posn_lots': -3,
        u'ref': None,
        u'resd_lots': 3,
        u'settl_date': 20140302,
        u'side': u'Sell',
        u'state': u'Trade',
        u'ticks': 12344
      }, {
        u'accnt': u'GOSAYL',
        u'instr': u'EURUSD',
        u'cpty': u'MARAYL',
        u'created': self.now,
        u'exec_cost': 37035,
        u'exec_lots': 3,
        u'id': 6,
        u'last_lots': 3,
        u'last_ticks': 12345,
        u'liq_ind': u'Taker',
        u'lots': 11,
        u'market_id': 82255,
        u'match_id': 5,
        u'min_lots': None,
        u'order_id': 4,
        u'posn_cost': 0,
        u'posn_lots': 0,
        u'ref': None,
        u'resd_lots': 8,
        u'settl_date': 20140302,
        u'side': u'Sell',
        u'state': u'Trade',
        u'ticks': 12344
      }, {
        u'accnt': u'GOSAYL',
        u'instr': u'EURUSD',
        u'cpty': None,
        u'created': self.now,
        u'exec_cost': 0,
        u'exec_lots': 0,
        u'id': 4,
        u'last_lots': None,
        u'last_ticks': None,
        u'liq_ind': None,
        u'lots': 11,
        u'market_id': 82255,
        u'match_id': None,
        u'min_lots': None,
        u'order_id': 4,
        u'posn_cost': None,
        u'posn_lots': None,
        u'ref': None,
        u'resd_lots': 11,
        u'settl_date': 20140302,
        u'side': u'Sell',
        u'state': u'New',
        u'ticks': 12344
      }],
      u'orders': [{
        u'accnt': u'GOSAYL',
        u'instr': u'EURUSD',
        u'created': self.now,
        u'exec_cost': 135787,
        u'exec_lots': 11,
        u'id': 4,
        u'last_lots': 3,
        u'last_ticks': 12344,
        u'lots': 11,
        u'market_id': 82255,
        u'min_lots': None,
        u'modified': self.now,
        u'ref': None,
        u'resd_lots': 0,
        u'settl_date': 20140302,
        u'side': u'Sell',
        u'state': u'Trade',
        u'ticks': 12344
      }],
      u'posn': {
        u'accnt': u'GOSAYL',
        u'buy_cost': 0,
        u'buy_lots': 0,
        u'instr': u'EURUSD',
        u'market_id': 82255,
        u'sell_cost': 135787,
        u'sell_lots': 11,
        u'settl_date': 20140302
      }
    }, resp.content)

  def maker_order(self, client):
    client.set_trader('MARAYL')
    resp = client.send('GET', '/api/sess/order')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'created': self.now,
      u'exec_cost': 37032,
      u'exec_lots': 3,
      u'id': 3,
      u'last_lots': 3,
      u'last_ticks': 12344,
      u'lots': 7,
      u'market_id': 82255,
      u'min_lots': None,
      u'modified': self.now,
      u'ref': None,
      u'resd_lots': 4,
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 12344
    }], resp.content)

  def maker_exec(self, client):
    client.set_trader('MARAYL')
    resp = client.send('GET', '/api/sess/exec')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec_cost': 37032,
      u'exec_lots': 3,
      u'id': 9,
      u'instr': u'EURUSD',
      u'last_lots': 3,
      u'last_ticks': 12344,
      u'liq_ind': u'Maker',
      u'lots': 7,
      u'market_id': 82255,
      u'match_id': 10,
      u'min_lots': None,
      u'order_id': 3,
      u'posn_cost': 98755,
      u'posn_lots': 8,
      u'ref': None,
      u'resd_lots': 4,
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 12344
    }, {
      u'accnt': u'MARAYL',
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec_cost': 61720,
      u'exec_lots': 5,
      u'id': 7,
      u'instr': u'EURUSD',
      u'last_lots': 5,
      u'last_ticks': 12344,
      u'liq_ind': u'Maker',
      u'lots': 5,
      u'market_id': 82255,
      u'match_id': 8,
      u'min_lots': None,
      u'order_id': 2,
      u'posn_cost': 37035,
      u'posn_lots': 3,
      u'ref': None,
      u'resd_lots': 0,
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 12344
    }, {
      u'accnt': u'MARAYL',
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec_cost': 37035,
      u'exec_lots': 3,
      u'id': 5,
      u'instr': u'EURUSD',
      u'last_lots': 3,
      u'last_ticks': 12345,
      u'liq_ind': u'Maker',
      u'lots': 3,
      u'market_id': 82255,
      u'match_id': 6,
      u'min_lots': None,
      u'order_id': 1,
      u'posn_cost': 0,
      u'posn_lots': 0,
      u'ref': None,
      u'resd_lots': 0,
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 12345
    }, {
      u'accnt': u'MARAYL',
      u'cpty': None,
      u'created': self.now,
      u'exec_cost': 0,
      u'exec_lots': 0,
      u'id': 3,
      u'instr': u'EURUSD',
      u'last_lots': None,
      u'last_ticks': None,
      u'liq_ind': None,
      u'lots': 7,
      u'market_id': 82255,
      u'match_id': None,
      u'min_lots': None,
      u'order_id': 3,
      u'posn_cost': None,
      u'posn_lots': None,
      u'ref': None,
      u'resd_lots': 7,
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'New',
      u'ticks': 12344
    }, {
      u'accnt': u'MARAYL',
      u'cpty': None,
      u'created': self.now,
      u'exec_cost': 0,
      u'exec_lots': 0,
      u'id': 2,
      u'instr': u'EURUSD',
      u'last_lots': None,
      u'last_ticks': None,
      u'liq_ind': None,
      u'lots': 5,
      u'market_id': 82255,
      u'match_id': None,
      u'min_lots': None,
      u'order_id': 2,
      u'posn_cost': None,
      u'posn_lots': None,
      u'ref': None,
      u'resd_lots': 5,
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'New',
      u'ticks': 12344
    }, {
      u'accnt': u'MARAYL',
      u'cpty': None,
      u'created': self.now,
      u'exec_cost': 0,
      u'exec_lots': 0,
      u'id': 1,
      u'instr': u'EURUSD',
      u'last_lots': None,
      u'last_ticks': None,
      u'liq_ind': None,
      u'lots': 3,
      u'market_id': 82255,
      u'match_id': None,
      u'min_lots': None,
      u'order_id': 1,
      u'posn_cost': None,
      u'posn_lots': None,
      u'ref': None,
      u'resd_lots': 3,
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'New',
      u'ticks': 12345
    }], resp.content)

  def maker_trade(self, client):
    client.set_trader('MARAYL')
    resp = client.send('GET', '/api/sess/trade')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec_cost': 37035,
      u'exec_lots': 3,
      u'id': 5,
      u'last_lots': 3,
      u'last_ticks': 12345,
      u'liq_ind': u'Maker',
      u'lots': 3,
      u'market_id': 82255,
      u'match_id': 6,
      u'min_lots': None,
      u'order_id': 1,
      u'posn_cost': 0,
      u'posn_lots': 0,
      u'ref': None,
      u'resd_lots': 0,
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 12345
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec_cost': 61720,
      u'exec_lots': 5,
      u'id': 7,
      u'last_lots': 5,
      u'last_ticks': 12344,
      u'liq_ind': u'Maker',
      u'lots': 5,
      u'market_id': 82255,
      u'match_id': 8,
      u'min_lots': None,
      u'order_id': 2,
      u'posn_cost': 37035,
      u'posn_lots': 3,
      u'ref': None,
      u'resd_lots': 0,
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 12344
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec_cost': 37032,
      u'exec_lots': 3,
      u'id': 9,
      u'last_lots': 3,
      u'last_ticks': 12344,
      u'liq_ind': u'Maker',
      u'lots': 7,
      u'market_id': 82255,
      u'match_id': 10,
      u'min_lots': None,
      u'order_id': 3,
      u'posn_cost': 98755,
      u'posn_lots': 8,
      u'ref': None,
      u'resd_lots': 4,
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 12344
    }], resp.content)

  def maker_posn(self, client):
    client.set_trader('MARAYL')
    resp = client.send('GET', '/api/sess/posn')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'buy_cost': 135787,
      u'buy_lots': 11,
      u'instr': u'EURUSD',
      u'market_id': 82255,
      u'sell_cost': 0,
      u'sell_lots': 0,
      u'settl_date': 20140302
    }], resp.content)

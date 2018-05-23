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
    self.maxDiff = None
    self.now = 1388534400000
    with DbFile() as dbFile:
      with Server(dbFile, self.now) as server:
        with Client() as client:
          client.setTime(self.now)

          self.createMarket(client, 'EURUSD', 20140302)

          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'Buy', 3, 12345)
          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'Buy', 5, 12344)
          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'Buy', 7, 12344)

          self.takeOrder(client)

          self.makerOrder(client)
          self.makerExec(client)
          self.makerTrade(client)
          self.makerPosn(client)

  def takeOrder(self, client):
    client.setTrader('GOSAYL')
    resp = client.send('POST', '/accnt/orders/EURUSD/20140302',
                       side = 'Sell',
                       lots = 15,
                       ticks = 12344)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'market': {
        u'bid_count': [None, None, None],
        u'bid_lots': [None, None, None],
        u'bid_ticks': [None, None, None],
        u'instr': u'EURUSD',
        u'id': 82255,
        u'last_lots': 7,
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
        u'exec_cost': 185163,
        u'exec_lots': 15,
        u'id': 10,
        u'last_lots': 7,
        u'last_ticks': 12344,
        u'liq_ind': u'Taker',
        u'lots': 15,
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
        u'lots': 15,
        u'market_id': 82255,
        u'match_id': 7,
        u'min_lots': None,
        u'order_id': 4,
        u'posn_cost': -37035,
        u'posn_lots': -3,
        u'ref': None,
        u'resd_lots': 7,
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
        u'lots': 15,
        u'market_id': 82255,
        u'match_id': 5,
        u'min_lots': None,
        u'order_id': 4,
        u'posn_cost': 0,
        u'posn_lots': 0,
        u'ref': None,
        u'resd_lots': 12,
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
        u'lots': 15,
        u'market_id': 82255,
        u'match_id': None,
        u'min_lots': None,
        u'order_id': 4,
        u'posn_cost': None,
        u'posn_lots': None,
        u'ref': None,
        u'resd_lots': 15,
        u'settl_date': 20140302,
        u'side': u'Sell',
        u'state': u'New',
        u'ticks': 12344
      }],
      u'orders': [{
        u'accnt': u'GOSAYL',
        u'instr': u'EURUSD',
        u'created': self.now,
        u'exec_cost': 185163,
        u'exec_lots': 15,
        u'id': 4,
        u'last_lots': 7,
        u'last_ticks': 12344,
        u'lots': 15,
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
        u'sell_cost': 185163,
        u'sell_lots': 15,
        u'settl_date': 20140302
      }
    }, resp.content)

  def makerOrder(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/orders')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([], resp.content)

  def makerExec(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/execs')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec_cost': 86408,
      u'exec_lots': 7,
      u'id': 9,
      u'last_lots': 7,
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
      u'cpty': None,
      u'created': self.now,
      u'exec_cost': 0,
      u'exec_lots': 0,
      u'id': 3,
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
      u'instr': u'EURUSD',
      u'cpty': None,
      u'created': self.now,
      u'exec_cost': 0,
      u'exec_lots': 0,
      u'id': 2,
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
      u'instr': u'EURUSD',
      u'cpty': None,
      u'created': self.now,
      u'exec_cost': 0,
      u'exec_lots': 0,
      u'id': 1,
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

  def makerTrade(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/trades')

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
      u'exec_cost': 86408,
      u'exec_lots': 7,
      u'id': 9,
      u'last_lots': 7,
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
      u'resd_lots': 0,
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 12344
    }], resp.content)

  def makerPosn(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/posns')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'buy_cost': 185163,
      u'buy_lots': 15,
      u'instr': u'EURUSD',
      u'market_id': 82255,
      u'sell_cost': 0,
      u'sell_lots': 0,
      u'settl_date': 20140302
    }], resp.content)

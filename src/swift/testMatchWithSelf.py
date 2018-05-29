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

          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'Buy', 5, 12345)

          self.takeOrder(client)

  def takeOrder(self, client):
    client.setTrader('MARAYL')
    resp = client.send('POST', '/accnt/orders/EURUSD/20140302',
                       side = 'Sell',
                       lots = 5,
                       ticks = 12345)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'market': {
        u'bid_count': [None, None, None],
        u'bid_lots': [None, None, None],
        u'bid_ticks': [None, None, None],
        u'instr': u'EURUSD',
        u'id': 82255,
        u'last_lots': 5,
        u'last_ticks': 12345,
        u'last_time': self.now,
        u'offer_count': [None, None, None],
        u'offer_lots': [None, None, None],
        u'offer_ticks': [None, None, None],
        u'settl_date': 20140302,
        u'state': 0
      },
      u'execs': [{
        u'accnt': u'MARAYL',
        u'instr': u'EURUSD',
        u'cpty': u'MARAYL',
        u'created': self.now,
        u'exec_cost': 61725,
        u'exec_lots': 5,
        u'id': 4,
        u'last_lots': 5,
        u'last_ticks': 12345,
        u'liq_ind': u'Taker',
        u'lots': 5,
        u'market_id': 82255,
        u'match_id': 3,
        u'min_lots': None,
        u'order_id': 2,
        u'posn_cost': 61725,
        u'posn_lots': 5,
        u'ref': None,
        u'resd_lots': 0,
        u'settl_date': 20140302,
        u'side': u'Sell',
        u'state': u'Trade',
        u'ticks': 12345
      }, {
        u'accnt': u'MARAYL',
        u'instr': u'EURUSD',
        u'cpty': u'MARAYL',
        u'created': self.now,
        u'exec_cost': 61725,
        u'exec_lots': 5,
        u'id': 3,
        u'last_lots': 5,
        u'last_ticks': 12345,
        u'liq_ind': u'Maker',
        u'lots': 5,
        u'market_id': 82255,
        u'match_id': 4,
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
        u'side': u'Sell',
        u'state': u'New',
        u'ticks': 12345
      }],
      u'orders': [{
        u'accnt': u'MARAYL',
        u'instr': u'EURUSD',
        u'created': self.now,
        u'exec_cost': 61725,
        u'exec_lots': 5,
        u'id': 2,
        u'last_lots': 5,
        u'last_ticks': 12345,
        u'lots': 5,
        u'market_id': 82255,
        u'min_lots': None,
        u'modified': self.now,
        u'ref': None,
        u'resd_lots': 0,
        u'settl_date': 20140302,
        u'side': u'Sell',
        u'state': u'Trade',
        u'ticks': 12345
      }, {
        u'accnt': u'MARAYL',
        u'instr': u'EURUSD',
        u'created': self.now,
        u'exec_cost': 61725,
        u'exec_lots': 5,
        u'id': 1,
        u'last_lots': 5,
        u'last_ticks': 12345,
        u'lots': 5,
        u'market_id': 82255,
        u'min_lots': None,
        u'modified': self.now,
        u'ref': None,
        u'resd_lots': 0,
        u'settl_date': 20140302,
        u'side': u'Buy',
        u'state': u'Trade',
        u'ticks': 12345
      }],
      u'posn': {
        u'accnt': u'MARAYL',
        u'buy_cost': 61725,
        u'buy_lots': 5,
        u'instr': u'EURUSD',
        u'market_id': 82255,
        u'sell_cost': 61725,
        u'sell_lots': 5,
        u'settl_date': 20140302
      }
    }, resp.content)

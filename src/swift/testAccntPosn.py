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

          self.create_order(client, 'MARAYL', 'EURUSD', 20140402, 'Sell', 5, 12346)
          self.create_order(client, 'MARAYL', 'EURUSD', 20140402, 'Buy', 5, 12346)

          self.create_order(client, 'MARAYL', 'GBPUSD', 20140302, 'Sell', 7, 15346)
          self.create_order(client, 'MARAYL', 'GBPUSD', 20140302, 'Buy', 7, 15346)

          self.check_auth(client)

          self.get_all(client)
          self.get_by_instr(client)
          self.get_by_settl_date(client)

      with Server(db_file, self.now) as server:
        with Client() as client:
          client.set_time(self.now)

          self.get_all(client)
          self.get_by_instr(client)
          self.get_by_settl_date(client)

  def check_auth(self, client):
    client.set_auth(None, 0x2)

    resp = client.send('GET', '/api/accnt/posn')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    resp = client.send('GET', '/api/accnt/posn/EURUSD')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    resp = client.send('GET', '/api/accnt/posn/EURUSD/20140302')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    client.set_auth('MARAYL', ~0x2 & 0x7fffffff)

    resp = client.send('GET', '/api/accnt/posn')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

    resp = client.send('GET', '/api/accnt/posn/EURUSD')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

    resp = client.send('GET', '/api/accnt/posn/EURUSD/20140302')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def get_all(self, client):
    client.set_trader('MARAYL')
    resp = client.send('GET', '/api/accnt/posn')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'buy_cost': 37038,
      u'buy_lots': 3,
      u'instr': u'EURUSD',
      u'market_id': 82255,
      u'sell_cost': 37038,
      u'sell_lots': 3,
      u'settl_date': 20140302
    }, {
      u'accnt': u'MARAYL',
      u'buy_cost': 61730,
      u'buy_lots': 5,
      u'instr': u'EURUSD',
      u'market_id': 82286,
      u'sell_cost': 61730,
      u'sell_lots': 5,
      u'settl_date': 20140402
    }, {
      u'accnt': u'MARAYL',
      u'buy_cost': 107422,
      u'buy_lots': 7,
      u'instr': u'GBPUSD',
      u'market_id': 147791,
      u'sell_cost': 107422,
      u'sell_lots': 7,
      u'settl_date': 20140302
    }], resp.content)

  def get_by_instr(self, client):
    client.set_trader('MARAYL')
    resp = client.send('GET', '/api/accnt/posn/EURUSD')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'buy_cost': 37038,
      u'buy_lots': 3,
      u'instr': u'EURUSD',
      u'market_id': 82255,
      u'sell_cost': 37038,
      u'sell_lots': 3,
      u'settl_date': 20140302
    }, {
      u'accnt': u'MARAYL',
      u'buy_cost': 61730,
      u'buy_lots': 5,
      u'instr': u'EURUSD',
      u'market_id': 82286,
      u'sell_cost': 61730,
      u'sell_lots': 5,
      u'settl_date': 20140402
    }], resp.content)

  def get_by_settl_date(self, client):
    client.set_trader('MARAYL')
    resp = client.send('GET', '/api/accnt/posn/EURUSD/20140302')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'accnt': u'MARAYL',
      u'buy_cost': 37038,
      u'buy_lots': 3,
      u'instr': u'EURUSD',
      u'market_id': 82255,
      u'sell_cost': 37038,
      u'sell_lots': 3,
      u'settl_date': 20140302
    }, resp.content)

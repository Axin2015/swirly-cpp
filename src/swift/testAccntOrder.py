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
          self.createMarket(client, 'EURUSD', 20140402)
          self.createMarket(client, 'GBPUSD', 20140302)

          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'Sell', 3, 12346)
          self.createOrder(client, 'MARAYL', 'EURUSD', 20140302, 'Buy', 3, 12344)

          self.createOrder(client, 'MARAYL', 'EURUSD', 20140402, 'Sell', 5, 12347)
          self.createOrder(client, 'MARAYL', 'EURUSD', 20140402, 'Buy', 5, 12343)

          self.createOrder(client, 'MARAYL', 'GBPUSD', 20140302, 'Sell', 3, 15346)
          self.createOrder(client, 'MARAYL', 'GBPUSD', 20140302, 'Buy', 3, 15344)

          self.checkAuth(client)

          self.getAll(client)
          self.getByInstr(client)
          self.getByMarket(client)
          self.getById(client)

      with Server(dbFile, self.now) as server:
        with Client() as client:
          client.setTime(self.now)

          self.getAll(client)
          self.getByInstr(client)
          self.getByMarket(client)
          self.getById(client)

  def checkAuth(self, client):
    client.setAuth(None, 0x2)

    resp = client.send('GET', '/accnt/orders')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    resp = client.send('GET', '/accnt/orders/EURUSD')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    resp = client.send('GET', '/accnt/orders/EURUSD/20140302')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    client.setAuth('MARAYL', ~0x2 & 0x7fffffff)

    resp = client.send('GET', '/accnt/orders')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

    resp = client.send('GET', '/accnt/orders/EURUSD')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

    resp = client.send('GET', '/accnt/orders/EURUSD/20140302')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def getAll(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/orders')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'created': self.now,
      u'exec_cost': 0,
      u'exec_lots': 0,
      u'id': 1,
      u'last_lots': None,
      u'last_ticks': None,
      u'lots': 3,
      u'market_id': 82255,
      u'min_lots': None,
      u'modified': self.now,
      u'ref': None,
      u'resd_lots': 3,
      u'settl_date': 20140302,
      u'side': u'Sell',
      u'state': u'New',
      u'ticks': 12346
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'created': self.now,
      u'exec_cost': 0,
      u'exec_lots': 0,
      u'id': 2,
      u'last_lots': None,
      u'last_ticks': None,
      u'lots': 3,
      u'market_id': 82255,
      u'min_lots': None,
      u'modified': self.now,
      u'ref': None,
      u'resd_lots': 3,
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'New',
      u'ticks': 12344
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'created': self.now,
      u'exec_cost': 0,
      u'exec_lots': 0,
      u'id': 1,
      u'last_lots': None,
      u'last_ticks': None,
      u'lots': 5,
      u'market_id': 82286,
      u'min_lots': None,
      u'modified': self.now,
      u'ref': None,
      u'resd_lots': 5,
      u'settl_date': 20140402,
      u'side': u'Sell',
      u'state': u'New',
      u'ticks': 12347
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'created': self.now,
      u'exec_cost': 0,
      u'exec_lots': 0,
      u'id': 2,
      u'last_lots': None,
      u'last_ticks': None,
      u'lots': 5,
      u'market_id': 82286,
      u'min_lots': None,
      u'modified': self.now,
      u'ref': None,
      u'resd_lots': 5,
      u'settl_date': 20140402,
      u'side': u'Buy',
      u'state': u'New',
      u'ticks': 12343
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'GBPUSD',
      u'created': self.now,
      u'exec_cost': 0,
      u'exec_lots': 0,
      u'id': 1,
      u'last_lots': None,
      u'last_ticks': None,
      u'lots': 3,
      u'market_id': 147791,
      u'min_lots': None,
      u'modified': self.now,
      u'ref': None,
      u'resd_lots': 3,
      u'settl_date': 20140302,
      u'side': u'Sell',
      u'state': u'New',
      u'ticks': 15346
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'GBPUSD',
      u'created': self.now,
      u'exec_cost': 0,
      u'exec_lots': 0,
      u'id': 2,
      u'last_lots': None,
      u'last_ticks': None,
      u'lots': 3,
      u'market_id': 147791,
      u'min_lots': None,
      u'modified': self.now,
      u'ref': None,
      u'resd_lots': 3,
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'New',
      u'ticks': 15344
    }], resp.content)

  def getByInstr(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/orders/EURUSD')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'created': self.now,
      u'exec_cost': 0,
      u'exec_lots': 0,
      u'id': 1,
      u'last_lots': None,
      u'last_ticks': None,
      u'lots': 3,
      u'market_id': 82255,
      u'min_lots': None,
      u'modified': self.now,
      u'ref': None,
      u'resd_lots': 3,
      u'settl_date': 20140302,
      u'side': u'Sell',
      u'state': u'New',
      u'ticks': 12346
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'created': self.now,
      u'exec_cost': 0,
      u'exec_lots': 0,
      u'id': 2,
      u'last_lots': None,
      u'last_ticks': None,
      u'lots': 3,
      u'market_id': 82255,
      u'min_lots': None,
      u'modified': self.now,
      u'ref': None,
      u'resd_lots': 3,
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'New',
      u'ticks': 12344
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'created': self.now,
      u'exec_cost': 0,
      u'exec_lots': 0,
      u'id': 1,
      u'last_lots': None,
      u'last_ticks': None,
      u'lots': 5,
      u'market_id': 82286,
      u'min_lots': None,
      u'modified': self.now,
      u'ref': None,
      u'resd_lots': 5,
      u'settl_date': 20140402,
      u'side': u'Sell',
      u'state': u'New',
      u'ticks': 12347
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'created': self.now,
      u'exec_cost': 0,
      u'exec_lots': 0,
      u'id': 2,
      u'last_lots': None,
      u'last_ticks': None,
      u'lots': 5,
      u'market_id': 82286,
      u'min_lots': None,
      u'modified': self.now,
      u'ref': None,
      u'resd_lots': 5,
      u'settl_date': 20140402,
      u'side': u'Buy',
      u'state': u'New',
      u'ticks': 12343
    }], resp.content)

  def getByMarket(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/orders/EURUSD/20140302')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'created': self.now,
      u'exec_cost': 0,
      u'exec_lots': 0,
      u'id': 1,
      u'last_lots': None,
      u'last_ticks': None,
      u'lots': 3,
      u'market_id': 82255,
      u'min_lots': None,
      u'modified': self.now,
      u'ref': None,
      u'resd_lots': 3,
      u'settl_date': 20140302,
      u'side': u'Sell',
      u'state': u'New',
      u'ticks': 12346
    }, {
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'created': self.now,
      u'exec_cost': 0,
      u'exec_lots': 0,
      u'id': 2,
      u'last_lots': None,
      u'last_ticks': None,
      u'lots': 3,
      u'market_id': 82255,
      u'min_lots': None,
      u'modified': self.now,
      u'ref': None,
      u'resd_lots': 3,
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'New',
      u'ticks': 12344
    }], resp.content)

  def getById(self, client):
    client.setTrader('MARAYL')
    resp = client.send('GET', '/accnt/orders/GBPUSD/20140302/1')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'accnt': u'MARAYL',
      u'instr': u'GBPUSD',
      u'created': self.now,
      u'exec_cost': 0,
      u'exec_lots': 0,
      u'id': 1,
      u'last_lots': None,
      u'last_ticks': None,
      u'lots': 3,
      u'market_id': 147791,
      u'min_lots': None,
      u'modified': self.now,
      u'ref': None,
      u'resd_lots': 3,
      u'settl_date': 20140302,
      u'side': u'Sell',
      u'state': u'New',
      u'ticks': 15346
    }, resp.content)

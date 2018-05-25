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

          self.checkAuth(client)

          self.createMakerBuy(client)
          self.createTakerSell(client)

          self.createMakerBuyByInstr(client)
          self.createTakerSellByInstr(client)

          self.createMakerBuyByMarket(client)
          self.createTakerSellByMarket(client)

  def checkAuth(self, client):
    client.setAuth(None, 0x1)

    resp = client.send('POST', '/accnt/trades')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    resp = client.send('POST', '/accnt/trades/EURUSD')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    resp = client.send('POST', '/accnt/trades/EURUSD/20140302')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    client.setAuth('ADMIN', ~0x1 & 0x7fffffff)

    resp = client.send('POST', '/accnt/trades')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

    resp = client.send('POST', '/accnt/trades/EURUSD')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

    resp = client.send('POST', '/accnt/trades/EURUSD/20140302')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def createMakerBuy(self, client):
    client.setAdmin()
    resp = client.send('POST', '/accnt/trades',
                       accnt = 'MARAYL',
                       instr = 'EURUSD',
                       settl_date = 20140302,
                       ref = 'test1',
                       side = 'Buy',
                       lots = 10,
                       ticks = 12345,
                       liq_ind = 'Maker',
                       cpty = 'GOSAYL')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec_cost': 123450,
      u'exec_lots': 10,
      u'id': 1,
      u'last_lots': 10,
      u'last_ticks': 12345,
      u'lots': 10,
      u'market_id': 82255,
      u'match_id': None,
      u'min_lots': 1,
      u'order_id': 0,
      u'ref': u'test1',
      u'resd_lots': 0,
      u'liq_ind': u'Maker',
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 12345
    }, {
      u'accnt': u'GOSAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 123450,
      u'exec_lots': 10,
      u'id': 2,
      u'last_lots': 10,
      u'last_ticks': 12345,
      u'lots': 10,
      u'market_id': 82255,
      u'match_id': None,
      u'min_lots': 1,
      u'order_id': 0,
      u'ref': u'test1',
      u'resd_lots': 0,
      u'liq_ind': u'Taker',
      u'settl_date': 20140302,
      u'side': u'Sell',
      u'state': u'Trade',
      u'ticks': 12345
    }], resp.content)

  def createTakerSell(self, client):
    client.setAdmin()
    resp = client.send('POST', '/accnt/trades',
                       accnt = 'MARAYL',
                       instr = 'EURUSD',
                       settl_date = 20140302,
                       ref = 'test2',
                       side = 'Sell',
                       lots = 15,
                       ticks = 12345,
                       liq_ind = 'Taker',
                       cpty = 'GOSAYL')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec_cost': 185175,
      u'exec_lots': 15,
      u'id': 3,
      u'last_lots': 15,
      u'last_ticks': 12345,
      u'lots': 15,
      u'market_id': 82255,
      u'match_id': None,
      u'min_lots': 1,
      u'order_id': 0,
      u'ref': u'test2',
      u'resd_lots': 0,
      u'liq_ind': u'Taker',
      u'settl_date': 20140302,
      u'side': u'Sell',
      u'state': u'Trade',
      u'ticks': 12345
    }, {
      u'accnt': u'GOSAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 185175,
      u'exec_lots': 15,
      u'id': 4,
      u'last_lots': 15,
      u'last_ticks': 12345,
      u'lots': 15,
      u'market_id': 82255,
      u'match_id': None,
      u'min_lots': 1,
      u'order_id': 0,
      u'ref': u'test2',
      u'resd_lots': 0,
      u'liq_ind': u'Maker',
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 12345
    }], resp.content)

  def createMakerBuyByInstr(self, client):
    client.setAdmin()
    resp = client.send('POST', '/accnt/trades/EURUSD',
                       accnt = 'MARAYL',
                       settl_date = 20140302,
                       ref = 'test1',
                       side = 'Buy',
                       lots = 10,
                       ticks = 12345,
                       liq_ind = 'Maker',
                       cpty = 'GOSAYL')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec_cost': 123450,
      u'exec_lots': 10,
      u'id': 5,
      u'last_lots': 10,
      u'last_ticks': 12345,
      u'lots': 10,
      u'market_id': 82255,
      u'match_id': None,
      u'min_lots': 1,
      u'order_id': 0,
      u'ref': u'test1',
      u'resd_lots': 0,
      u'liq_ind': u'Maker',
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 12345
    }, {
      u'accnt': u'GOSAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 123450,
      u'exec_lots': 10,
      u'id': 6,
      u'last_lots': 10,
      u'last_ticks': 12345,
      u'lots': 10,
      u'market_id': 82255,
      u'match_id': None,
      u'min_lots': 1,
      u'order_id': 0,
      u'ref': u'test1',
      u'resd_lots': 0,
      u'liq_ind': u'Taker',
      u'settl_date': 20140302,
      u'side': u'Sell',
      u'state': u'Trade',
      u'ticks': 12345
    }], resp.content)

  def createTakerSellByInstr(self, client):
    client.setAdmin()
    resp = client.send('POST', '/accnt/trades/EURUSD',
                       accnt = 'MARAYL',
                       settl_date = 20140302,
                       ref = 'test2',
                       side = 'Sell',
                       lots = 15,
                       ticks = 12345,
                       liq_ind = 'Taker',
                       cpty = 'GOSAYL')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec_cost': 185175,
      u'exec_lots': 15,
      u'id': 7,
      u'last_lots': 15,
      u'last_ticks': 12345,
      u'lots': 15,
      u'market_id': 82255,
      u'match_id': None,
      u'min_lots': 1,
      u'order_id': 0,
      u'ref': u'test2',
      u'resd_lots': 0,
      u'liq_ind': u'Taker',
      u'settl_date': 20140302,
      u'side': u'Sell',
      u'state': u'Trade',
      u'ticks': 12345
    }, {
      u'accnt': u'GOSAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 185175,
      u'exec_lots': 15,
      u'id': 8,
      u'last_lots': 15,
      u'last_ticks': 12345,
      u'lots': 15,
      u'market_id': 82255,
      u'match_id': None,
      u'min_lots': 1,
      u'order_id': 0,
      u'ref': u'test2',
      u'resd_lots': 0,
      u'liq_ind': u'Maker',
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 12345
    }], resp.content)

  def createMakerBuyByMarket(self, client):
    client.setAdmin()
    resp = client.send('POST', '/accnt/trades/EURUSD/20140302',
                       accnt = 'MARAYL',
                       ref = 'test1',
                       side = 'Buy',
                       lots = 10,
                       ticks = 12345,
                       liq_ind = 'Maker',
                       cpty = 'GOSAYL')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec_cost': 123450,
      u'exec_lots': 10,
      u'id': 9,
      u'last_lots': 10,
      u'last_ticks': 12345,
      u'lots': 10,
      u'market_id': 82255,
      u'match_id': None,
      u'min_lots': 1,
      u'order_id': 0,
      u'ref': u'test1',
      u'resd_lots': 0,
      u'liq_ind': u'Maker',
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 12345
    }, {
      u'accnt': u'GOSAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 123450,
      u'exec_lots': 10,
      u'id': 10,
      u'last_lots': 10,
      u'last_ticks': 12345,
      u'lots': 10,
      u'market_id': 82255,
      u'match_id': None,
      u'min_lots': 1,
      u'order_id': 0,
      u'ref': u'test1',
      u'resd_lots': 0,
      u'liq_ind': u'Taker',
      u'settl_date': 20140302,
      u'side': u'Sell',
      u'state': u'Trade',
      u'ticks': 12345
    }], resp.content)

  def createTakerSellByMarket(self, client):
    client.setAdmin()
    resp = client.send('POST', '/accnt/trades/EURUSD/20140302',
                       accnt = 'MARAYL',
                       ref = 'test2',
                       side = 'Sell',
                       lots = 15,
                       ticks = 12345,
                       liq_ind = 'Taker',
                       cpty = 'GOSAYL')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'accnt': u'MARAYL',
      u'instr': u'EURUSD',
      u'cpty': u'GOSAYL',
      u'created': self.now,
      u'exec_cost': 185175,
      u'exec_lots': 15,
      u'id': 11,
      u'last_lots': 15,
      u'last_ticks': 12345,
      u'lots': 15,
      u'market_id': 82255,
      u'match_id': None,
      u'min_lots': 1,
      u'order_id': 0,
      u'ref': u'test2',
      u'resd_lots': 0,
      u'liq_ind': u'Taker',
      u'settl_date': 20140302,
      u'side': u'Sell',
      u'state': u'Trade',
      u'ticks': 12345
    }, {
      u'accnt': u'GOSAYL',
      u'instr': u'EURUSD',
      u'cpty': u'MARAYL',
      u'created': self.now,
      u'exec_cost': 185175,
      u'exec_lots': 15,
      u'id': 12,
      u'last_lots': 15,
      u'last_ticks': 12345,
      u'lots': 15,
      u'market_id': 82255,
      u'match_id': None,
      u'min_lots': 1,
      u'order_id': 0,
      u'ref': u'test2',
      u'resd_lots': 0,
      u'liq_ind': u'Maker',
      u'settl_date': 20140302,
      u'side': u'Buy',
      u'state': u'Trade',
      u'ticks': 12345
    }], resp.content)

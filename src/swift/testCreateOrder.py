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

          self.createBid(client)
          self.createBidByInstr(client)
          self.createBidByMarket(client)

          self.createOffer(client)
          self.createOfferByInstr(client)
          self.createOfferByMarket(client)

  def checkAuth(self, client):
    client.setAuth(None, 0x2)

    resp = client.send('POST', '/accnt/orders')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    resp = client.send('POST', '/accnt/orders/EURUSD')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    resp = client.send('POST', '/accnt/orders/EURUSD/20140302')
    self.assertEqual(401, resp.status)
    self.assertEqual('Unauthorized', resp.reason)

    client.setAuth('MARAYL', ~0x2 & 0x7fffffff)

    resp = client.send('POST', '/accnt/orders')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

    resp = client.send('POST', '/accnt/orders/EURUSD')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

    resp = client.send('POST', '/accnt/orders/EURUSD/20140302')
    self.assertEqual(403, resp.status)
    self.assertEqual('Forbidden', resp.reason)

  def createBid(self, client):
    client.setTrader('MARAYL')
    resp = client.send('POST', '/accnt/orders',
                       instr = 'EURUSD',
                       settl_date = 20140302,
                       side = 'Buy',
                       lots = 3,
                       ticks = 12344)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'market': {
        u'bid_count': [1, None, None],
        u'bid_lots': [3, None, None],
        u'bid_ticks': [12344, None, None],
        u'instr': u'EURUSD',
        u'id': 82255,
        u'last_lots': None,
        u'last_ticks': None,
        u'last_time': None,
        u'offer_count': [None, None, None],
        u'offer_lots': [None, None, None],
        u'offer_ticks': [None, None, None],
        u'settl_date': 20140302,
        u'state': 0
      },
      u'execs': [{
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
        u'ticks': 12344
      }],
      u'orders': [{
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
        u'side': u'Buy',
        u'state': u'New',
        u'ticks': 12344
      }],
      u'posn': None
    }, resp.content)

  def createBidByInstr(self, client):
    client.setTrader('GOSAYL')
    resp = client.send('POST', '/accnt/orders',
                       instr = 'EURUSD',
                       settl_date = 20140302,
                       side = 'Buy',
                       lots = 5,
                       ticks = 12343)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'market': {
        u'bid_count': [1, 1, None],
        u'bid_lots': [3, 5, None],
        u'bid_ticks': [12344, 12343, None],
        u'instr': u'EURUSD',
        u'id': 82255,
        u'last_lots': None,
        u'last_ticks': None,
        u'last_time': None,
        u'offer_count': [None, None, None],
        u'offer_lots': [None, None, None],
        u'offer_ticks': [None, None, None],
        u'settl_date': 20140302,
        u'state': 0
      },
      u'execs': [{
        u'accnt': u'GOSAYL',
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
        u'ticks': 12343
      }],
      u'orders': [{
        u'accnt': u'GOSAYL',
        u'instr': u'EURUSD',
        u'created': self.now,
        u'exec_cost': 0,
        u'exec_lots': 0,
        u'id': 2,
        u'last_lots': None,
        u'last_ticks': None,
        u'lots': 5,
        u'market_id': 82255,
        u'min_lots': None,
        u'modified': self.now,
        u'ref': None,
        u'resd_lots': 5,
        u'settl_date': 20140302,
        u'side': u'Buy',
        u'state': u'New',
        u'ticks': 12343
      }],
      u'posn': None
    }, resp.content)

  def createBidByMarket(self, client):
    client.setTrader('EDIAYL')
    resp = client.send('POST', '/accnt/orders/EURUSD/20140302',
                       side = 'Buy',
                       lots = 7,
                       ticks = 12342)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'market': {
        u'bid_count': [1, 1, 1],
        u'bid_lots': [3, 5, 7],
        u'bid_ticks': [12344, 12343, 12342],
        u'instr': u'EURUSD',
        u'id': 82255,
        u'last_lots': None,
        u'last_ticks': None,
        u'last_time': None,
        u'offer_count': [None, None, None],
        u'offer_lots': [None, None, None],
        u'offer_ticks': [None, None, None],
        u'settl_date': 20140302,
        u'state': 0
      },
      u'execs': [{
        u'accnt': u'EDIAYL',
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
        u'ticks': 12342
      }],
      u'orders': [{
        u'accnt': u'EDIAYL',
        u'instr': u'EURUSD',
        u'created': self.now,
        u'exec_cost': 0,
        u'exec_lots': 0,
        u'id': 3,
        u'last_lots': None,
        u'last_ticks': None,
        u'lots': 7,
        u'market_id': 82255,
        u'min_lots': None,
        u'modified': self.now,
        u'ref': None,
        u'resd_lots': 7,
        u'settl_date': 20140302,
        u'side': u'Buy',
        u'state': u'New',
        u'ticks': 12342
      }],
      u'posn': None
    }, resp.content)

  def createOffer(self, client):
    client.setTrader('MARAYL')
    resp = client.send('POST', '/accnt/orders',
                       instr = 'EURUSD',
                       settl_date = 20140302,
                       side = 'Sell',
                       lots = 3,
                       ticks = 12346)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'market': {
        u'bid_count': [1, 1, 1],
        u'bid_lots': [3, 5, 7],
        u'bid_ticks': [12344, 12343, 12342],
        u'instr': u'EURUSD',
        u'id': 82255,
        u'last_lots': None,
        u'last_ticks': None,
        u'last_time': None,
        u'offer_count': [1, None, None],
        u'offer_lots': [3, None, None],
        u'offer_ticks': [12346, None, None],
        u'settl_date': 20140302,
        u'state': 0
      },
      u'execs': [{
        u'accnt': u'MARAYL',
        u'instr': u'EURUSD',
        u'cpty': None,
        u'created': self.now,
        u'exec_cost': 0,
        u'exec_lots': 0,
        u'id': 4,
        u'last_lots': None,
        u'last_ticks': None,
        u'liq_ind': None,
        u'lots': 3,
        u'market_id': 82255,
        u'match_id': None,
        u'min_lots': None,
        u'order_id': 4,
        u'posn_cost': None,
        u'posn_lots': None,
        u'ref': None,
        u'resd_lots': 3,
        u'settl_date': 20140302,
        u'side': u'Sell',
        u'state': u'New',
        u'ticks': 12346
      }],
      u'orders': [{
        u'accnt': u'MARAYL',
        u'instr': u'EURUSD',
        u'created': self.now,
        u'exec_cost': 0,
        u'exec_lots': 0,
        u'id': 4,
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
      }],
      u'posn': None
    }, resp.content)

  def createOfferByInstr(self, client):
    client.setTrader('GOSAYL')
    resp = client.send('POST', '/accnt/orders',
                       instr = 'EURUSD',
                       settl_date = 20140302,
                       side = 'Sell',
                       lots = 5,
                       ticks = 12347)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'market': {
        u'bid_count': [1, 1, 1],
        u'bid_lots': [3, 5, 7],
        u'bid_ticks': [12344, 12343, 12342],
        u'instr': u'EURUSD',
        u'id': 82255,
        u'last_lots': None,
        u'last_ticks': None,
        u'last_time': None,
        u'offer_count': [1, 1, None],
        u'offer_lots': [3, 5, None],
        u'offer_ticks': [12346, 12347, None],
        u'settl_date': 20140302,
        u'state': 0
      },
      u'execs': [{
        u'accnt': u'GOSAYL',
        u'instr': u'EURUSD',
        u'cpty': None,
        u'created': self.now,
        u'exec_cost': 0,
        u'exec_lots': 0,
        u'id': 5,
        u'last_lots': None,
        u'last_ticks': None,
        u'liq_ind': None,
        u'lots': 5,
        u'market_id': 82255,
        u'match_id': None,
        u'min_lots': None,
        u'order_id': 5,
        u'posn_cost': None,
        u'posn_lots': None,
        u'ref': None,
        u'resd_lots': 5,
        u'settl_date': 20140302,
        u'side': u'Sell',
        u'state': u'New',
        u'ticks': 12347
      }],
      u'orders': [{
        u'accnt': u'GOSAYL',
        u'instr': u'EURUSD',
        u'created': self.now,
        u'exec_cost': 0,
        u'exec_lots': 0,
        u'id': 5,
        u'last_lots': None,
        u'last_ticks': None,
        u'lots': 5,
        u'market_id': 82255,
        u'min_lots': None,
        u'modified': self.now,
        u'ref': None,
        u'resd_lots': 5,
        u'settl_date': 20140302,
        u'side': u'Sell',
        u'state': u'New',
        u'ticks': 12347
      }],
      u'posn': None
    }, resp.content)

  def createOfferByMarket(self, client):
    client.setTrader('EDIAYL')
    resp = client.send('POST', '/accnt/orders/EURUSD/20140302',
                       side = 'Sell',
                       lots = 7,
                       ticks = 12348)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'market': {
        u'bid_count': [1, 1, 1],
        u'bid_lots': [3, 5, 7],
        u'bid_ticks': [12344, 12343, 12342],
        u'instr': u'EURUSD',
        u'id': 82255,
        u'last_lots': None,
        u'last_ticks': None,
        u'last_time': None,
        u'offer_count': [1, 1,1],
        u'offer_lots': [3, 5, 7],
        u'offer_ticks': [12346, 12347, 12348],
        u'settl_date': 20140302,
        u'state': 0
      },
      u'execs': [{
        u'accnt': u'EDIAYL',
        u'instr': u'EURUSD',
        u'cpty': None,
        u'created': self.now,
        u'exec_cost': 0,
        u'exec_lots': 0,
        u'id': 6,
        u'last_lots': None,
        u'last_ticks': None,
        u'liq_ind': None,
        u'lots': 7,
        u'market_id': 82255,
        u'match_id': None,
        u'min_lots': None,
        u'order_id': 6,
        u'posn_cost': None,
        u'posn_lots': None,
        u'ref': None,
        u'resd_lots': 7,
        u'settl_date': 20140302,
        u'side': u'Sell',
        u'state': u'New',
        u'ticks': 12348
      }],
      u'orders': [{
        u'accnt': u'EDIAYL',
        u'instr': u'EURUSD',
        u'created': self.now,
        u'exec_cost': 0,
        u'exec_lots': 0,
        u'id': 6,
        u'last_lots': None,
        u'last_ticks': None,
        u'lots': 7,
        u'market_id': 82255,
        u'min_lots': None,
        u'modified': self.now,
        u'ref': None,
        u'resd_lots': 7,
        u'settl_date': 20140302,
        u'side': u'Sell',
        u'state': u'New',
        u'ticks': 12348
      }],
      u'posn': None
    }, resp.content)

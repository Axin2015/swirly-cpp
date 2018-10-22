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

          self.get_all(client)
          self.get_by_symbol(client)

  def get_all(self, client):
    client.set_anon()
    resp = client.send('GET', '/api/refdata/instr')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'base_asset': u'CAP',
      u'term_ccy': u'USD',
      u'display': u'Central Appalachia Coal',
      u'id': 5,
      u'lot_denom': 1,
      u'lot_numer': 1000,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'CAP',
      u'pip_dp': 2,
      u'tick_denom': 20,
      u'tick_numer': 1
    }, {
      u'base_asset': u'CSCO',
      u'term_ccy': u'USD',
      u'display': u'Cisco Systems Inc',
      u'id': 14,
      u'lot_denom': 1,
      u'lot_numer': 1,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'CSCO',
      u'pip_dp': 3,
      u'tick_denom': 1000,
      u'tick_numer': 1
    }, {
      u'base_asset': u'DIS',
      u'term_ccy': u'USD',
      u'display': u'Walt Disney',
      u'id': 15,
      u'lot_denom': 1,
      u'lot_numer': 1,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'DIS',
      u'pip_dp': 3,
      u'tick_denom': 1000,
      u'tick_numer': 1
    }, {
      u'base_asset': u'EUR',
      u'term_ccy': u'USD',
      u'display': u'EURUSD',
      u'id': 1,
      u'lot_denom': 1,
      u'lot_numer': 1000000,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'EURUSD',
      u'pip_dp': 4,
      u'tick_denom': 10000,
      u'tick_numer': 1
    }, {
      u'base_asset': u'GBP',
      u'term_ccy': u'USD',
      u'display': u'GBPUSD',
      u'id': 2,
      u'lot_denom': 1,
      u'lot_numer': 1000000,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'GBPUSD',
      u'pip_dp': 4,
      u'tick_denom': 10000,
      u'tick_numer': 1
    }, {
      u'base_asset': u'IBM',
      u'term_ccy': u'USD',
      u'display': u'Ibm Corp',
      u'id': 16,
      u'lot_denom': 1,
      u'lot_numer': 1,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'IBM',
      u'pip_dp': 3,
      u'tick_denom': 1000,
      u'tick_numer': 1
    }, {
      u'base_asset': u'ILB',
      u'term_ccy': u'USD',
      u'display': u'Illinois Basin Coal',
      u'id': 7,
      u'lot_denom': 1,
      u'lot_numer': 1000,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'ILB',
      u'pip_dp': 2,
      u'tick_denom': 20,
      u'tick_numer': 1
    }, {
      u'base_asset': u'INTC',
      u'term_ccy': u'USD',
      u'display': u'Intel Corp',
      u'id': 17,
      u'lot_denom': 1,
      u'lot_numer': 1,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'INTC',
      u'pip_dp': 3,
      u'tick_denom': 1000,
      u'tick_numer': 1
    }, {
      u'base_asset': u'MSFT',
      u'term_ccy': u'USD',
      u'display': u'Microsoft Corp',
      u'id': 18,
      u'lot_denom': 1,
      u'lot_numer': 1,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'MSFT',
      u'pip_dp': 3,
      u'tick_denom': 1000,
      u'tick_numer': 1
    }, {
      u'base_asset': u'NAP',
      u'term_ccy': u'USD',
      u'display': u'Northern Appalachia Coal',
      u'id': 6,
      u'lot_denom': 1,
      u'lot_numer': 1000,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'NAP',
      u'pip_dp': 2,
      u'tick_denom': 20,
      u'tick_numer': 1
    }, {
      u'base_asset': u'PRB',
      u'term_ccy': u'USD',
      u'display': u'Powder River Basin Coal',
      u'id': 8,
      u'lot_denom': 1,
      u'lot_numer': 1000,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'PRB',
      u'pip_dp': 2,
      u'tick_denom': 20,
      u'tick_numer': 1
    }, {
      u'base_asset': u'UIB',
      u'term_ccy': u'USD',
      u'display': u'Uinta Basin Coal',
      u'id': 9,
      u'lot_denom': 1,
      u'lot_numer': 1000,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'UIB',
      u'pip_dp': 2,
      u'tick_denom': 20,
      u'tick_numer': 1
    }, {
      u'base_asset': u'USD',
      u'term_ccy': u'CHF',
      u'display': u'USDCHF',
      u'id': 3,
      u'lot_denom': 1,
      u'lot_numer': 1000000,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'USDCHF',
      u'pip_dp': 4,
      u'tick_denom': 10000,
      u'tick_numer': 1
    }, {
      u'base_asset': u'USD',
      u'term_ccy': u'JPY',
      u'display': u'USDJPY',
      u'id': 4,
      u'lot_denom': 1,
      u'lot_numer': 1000000,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'USDJPY',
      u'pip_dp': 2,
      u'tick_denom': 100,
      u'tick_numer': 1
    }, {
      u'base_asset': u'VIA',
      u'term_ccy': u'USD',
      u'display': u'Viacom Inc',
      u'id': 19,
      u'lot_denom': 1,
      u'lot_numer': 1,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'VIA',
      u'pip_dp': 3,
      u'tick_denom': 1000,
      u'tick_numer': 1
    }, {
      u'base_asset': u'VOD',
      u'term_ccy': u'USD',
      u'display': u'Vodafone Group Plc',
      u'id': 20,
      u'lot_denom': 1,
      u'lot_numer': 1,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'VOD',
      u'pip_dp': 3,
      u'tick_denom': 1000,
      u'tick_numer': 1
    }, {
      u'base_asset': u'VZ',
      u'term_ccy': u'USD',
      u'display': u'Verizon Com',
      u'id': 21,
      u'lot_denom': 1,
      u'lot_numer': 1,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'VZ',
      u'pip_dp': 3,
      u'tick_denom': 1000,
      u'tick_numer': 1
    }, {
      u'base_asset': u'WGAA',
      u'term_ccy': u'ETB',
      u'display': u'Gelena Abaya A',
      u'id': 13,
      u'lot_denom': 1,
      u'lot_numer': 1,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'WGAA',
      u'pip_dp': 0,
      u'tick_denom': 1,
      u'tick_numer': 1
    }, {
      u'base_asset': u'WKCA',
      u'term_ccy': u'ETB',
      u'display': u'Kochere A',
      u'id': 12,
      u'lot_denom': 1,
      u'lot_numer': 1,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'WKCA',
      u'pip_dp': 0,
      u'tick_denom': 1,
      u'tick_numer': 1
    }, {
      u'base_asset': u'WWNA',
      u'term_ccy': u'ETB',
      u'display': u'Wenago A',
      u'id': 11,
      u'lot_denom': 1,
      u'lot_numer': 1,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'WWNA',
      u'pip_dp': 0,
      u'tick_denom': 1,
      u'tick_numer': 1
    }, {
      u'base_asset': u'WYCA',
      u'term_ccy': u'ETB',
      u'display': u'Yirgachefe A',
      u'id': 10,
      u'lot_denom': 1,
      u'lot_numer': 1,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'WYCA',
      u'pip_dp': 0,
      u'tick_denom': 1,
      u'tick_numer': 1
    }], resp.content)

  def get_by_symbol(self, client):
    client.set_anon()
    resp = client.send('GET', '/api/refdata/instr/EURUSD')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'base_asset': u'EUR',
      u'term_ccy': u'USD',
      u'display': u'EURUSD',
      u'id': 1,
      u'lot_denom': 1,
      u'lot_numer': 1000000,
      u'max_lots': 10,
      u'min_lots': 1,
      u'symbol': u'EURUSD',
      u'pip_dp': 4,
      u'tick_denom': 10000,
      u'tick_numer': 1
    }, resp.content)

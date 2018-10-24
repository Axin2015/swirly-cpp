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
    resp = client.send('GET', '/api/refdata/asset')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([
      {u'display': u'Central Appalachia Coal', u'id': 7, u'symbol': u'CAP', u'type': u'Cmdty'},
      {u'display': u'Switzerland, Francs', u'id': 1, u'symbol': u'CHF', u'type': u'Ccy'},
      {u'display': u'Cisco Systems Inc', u'id': 17, u'symbol': u'CSCO', u'type': u'Corp'},
      {u'display': u'Walt Disney', u'id': 18, u'symbol': u'DIS', u'type': u'Corp'},
      {u'display': u'Ethiopia, Birr', u'id': 12, u'symbol': u'ETB', u'type': u'Ccy'},
      {u'display': u'Euro Member Countries, Euro', u'id': 2, u'symbol': u'EUR', u'type': u'Ccy'},
      {u'display': u'United Kingdom, Pounds', u'id': 3, u'symbol': u'GBP',  u'type': u'Ccy'},
      {u'display': u'Ibm Corp', u'id': 19, u'symbol': u'IBM', u'type': u'Corp'},
      {u'display': u'Illinois Basin Coal', u'id': 9, u'symbol': u'ILB', u'type': u'Cmdty'},
      {u'display': u'Intel Corp', u'id': 20, u'symbol': u'INTC', u'type': u'Corp'},
      {u'display': u'Japan, Yen', u'id': 4, u'symbol': u'JPY', u'type': u'Ccy'},
      {u'display': u'Microsoft Corp', u'id': 21, u'symbol': u'MSFT', u'type': u'Corp'},
      {u'display': u'Northern Appalachia Coal', u'id': 8, u'symbol': u'NAP', u'type': u'Cmdty'},
      {u'display': u'Powder River Basin Coal', u'id': 10, u'symbol': u'PRB', u'type': u'Cmdty'},
      {u'display': u'Uinta Basin Coal', u'id': 11, u'symbol': u'UIB', u'type': u'Cmdty'},
      {u'display': u'United States of America, Dollars', u'id': 5, u'symbol': u'USD',
       u'type': u'Ccy'},
      {u'display': u'Viacom Inc', u'id': 22, u'symbol': u'VIA', u'type': u'Corp'},
      {u'display': u'Vodafone Group Plc', u'id': 23, u'symbol': u'VOD', u'type': u'Corp'},
      {u'display': u'Verizon Com', u'id': 24, u'symbol': u'VZ', u'type': u'Corp'},
      {u'display': u'Gelena Abaya A', u'id': 16, u'symbol': u'WGAA', u'type': u'Cmdty'},
      {u'display': u'Kochere A', u'id': 15, u'symbol': u'WKCA', u'type': u'Cmdty'},
      {u'display': u'Wenago A', u'id': 14, u'symbol': u'WWNA', u'type': u'Cmdty'},
      {u'display': u'Yirgachefe A', u'id': 13, u'symbol': u'WYCA', u'type': u'Cmdty'},
      {u'display': u'South Africa, Rand', u'id': 6, u'symbol': u'ZAR', u'type': u'Ccy'}
    ], resp.content)

  def get_by_symbol(self, client):
    client.set_anon()
    resp = client.send('GET', '/api/refdata/asset/EUR')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'display': u'Euro Member Countries, Euro',
      u'id': 2,
      u'symbol': u'EUR',
      u'type': u'Ccy'
    }, resp.content)

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

          self.getAll(client)
          self.getBySymbol(client)

  def getAll(self, client):
    client.setAnon()
    resp = client.send('GET', '/refdata/assets')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([
      {u'display': u'Central Appalachia Coal', u'symbol': u'CAP',  u'type': u'Cmdty'},
      {u'display': u'Switzerland, Francs', u'symbol': u'CHF', u'type': u'Ccy'},
      {u'display': u'Cisco Systems Inc', u'symbol': u'CSCO', u'type': u'Corp'},
      {u'display': u'Walt Disney', u'symbol': u'DIS', u'type': u'Corp'},
      {u'display': u'Ethiopia, Birr', u'symbol': u'ETB', u'type': u'Ccy'},
      {u'display': u'Euro Member Countries, Euro', u'symbol': u'EUR', u'type': u'Ccy'},
      {u'display': u'United Kingdom, Pounds', u'symbol': u'GBP',  u'type': u'Ccy'},
      {u'display': u'Ibm Corp', u'symbol': u'IBM', u'type': u'Corp'},
      {u'display': u'Illinois Basin Coal', u'symbol': u'ILB', u'type': u'Cmdty'},
      {u'display': u'Intel Corp', u'symbol': u'INTC', u'type': u'Corp'},
      {u'display': u'Japan, Yen', u'symbol': u'JPY', u'type': u'Ccy'},
      {u'display': u'Microsoft Corp', u'symbol': u'MSFT', u'type': u'Corp'},
      {u'display': u'Northern Appalachia Coal', u'symbol': u'NAP', u'type': u'Cmdty'},
      {u'display': u'Powder River Basin Coal', u'symbol': u'PRB', u'type': u'Cmdty'},
      {u'display': u'Uinta Basin Coal', u'symbol': u'UIB', u'type': u'Cmdty'},
      {u'display': u'United States of America, Dollars', u'symbol': u'USD', u'type': u'Ccy'},
      {u'display': u'Viacom Inc', u'symbol': u'VIA', u'type': u'Corp'},
      {u'display': u'Vodafone Group Plc', u'symbol': u'VOD', u'type': u'Corp'},
      {u'display': u'Verizon Com', u'symbol': u'VZ', u'type': u'Corp'},
      {u'display': u'Gelena Abaya A', u'symbol': u'WGAA', u'type': u'Cmdty'},
      {u'display': u'Kochere A', u'symbol': u'WKCA', u'type': u'Cmdty'},
      {u'display': u'Wenago A', u'symbol': u'WWNA', u'type': u'Cmdty'},
      {u'display': u'Yirgachefe A', u'symbol': u'WYCA', u'type': u'Cmdty'},
      {u'display': u'South Africa, Rand', u'symbol': u'ZAR', u'type': u'Ccy'}
    ], resp.content)

  def getBySymbol(self, client):
    client.setAnon()
    resp = client.send('GET', '/refdata/assets/EUR')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'display': u'Euro Member Countries, Euro',
      u'symbol': u'EUR',
      u'type': u'Ccy'
    }, resp.content)

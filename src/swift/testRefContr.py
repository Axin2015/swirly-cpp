# The Restful Matching-Engine.
# Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
    resp = client.send('GET', '/refdata/instr')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'baseAsset': u'CAP',
      u'termCcy': u'USD',
      u'display': u'Central Appalachia Coal',
      u'lotDenom': 1,
      u'lotNumer': 1000,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'CAP',
      u'pipDp': 2,
      u'tickDenom': 20,
      u'tickNumer': 1
    }, {
      u'baseAsset': u'CSCO',
      u'termCcy': u'USD',
      u'display': u'Cisco Systems Inc',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'CSCO',
      u'pipDp': 3,
      u'tickDenom': 1000,
      u'tickNumer': 1
    }, {
      u'baseAsset': u'DIS',
      u'termCcy': u'USD',
      u'display': u'Walt Disney',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'DIS',
      u'pipDp': 3,
      u'tickDenom': 1000,
      u'tickNumer': 1
    }, {
      u'baseAsset': u'EUR',
      u'termCcy': u'USD',
      u'display': u'EURUSD',
      u'lotDenom': 1,
      u'lotNumer': 1000000,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'EURUSD',
      u'pipDp': 4,
      u'tickDenom': 10000,
      u'tickNumer': 1
    }, {
      u'baseAsset': u'GBP',
      u'termCcy': u'USD',
      u'display': u'GBPUSD',
      u'lotDenom': 1,
      u'lotNumer': 1000000,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'GBPUSD',
      u'pipDp': 4,
      u'tickDenom': 10000,
      u'tickNumer': 1
    }, {
      u'baseAsset': u'IBM',
      u'termCcy': u'USD',
      u'display': u'Ibm Corp',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'IBM',
      u'pipDp': 3,
      u'tickDenom': 1000,
      u'tickNumer': 1
    }, {
      u'baseAsset': u'ILB',
      u'termCcy': u'USD',
      u'display': u'Illinois Basin Coal',
      u'lotDenom': 1,
      u'lotNumer': 1000,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'ILB',
      u'pipDp': 2,
      u'tickDenom': 20,
      u'tickNumer': 1
    }, {
      u'baseAsset': u'INTC',
      u'termCcy': u'USD',
      u'display': u'Intel Corp',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'INTC',
      u'pipDp': 3,
      u'tickDenom': 1000,
      u'tickNumer': 1
    }, {
      u'baseAsset': u'MSFT',
      u'termCcy': u'USD',
      u'display': u'Microsoft Corp',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'MSFT',
      u'pipDp': 3,
      u'tickDenom': 1000,
      u'tickNumer': 1
    }, {
      u'baseAsset': u'NAP',
      u'termCcy': u'USD',
      u'display': u'Northern Appalachia Coal',
      u'lotDenom': 1,
      u'lotNumer': 1000,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'NAP',
      u'pipDp': 2,
      u'tickDenom': 20,
      u'tickNumer': 1
    }, {
      u'baseAsset': u'PRB',
      u'termCcy': u'USD',
      u'display': u'Powder River Basin Coal',
      u'lotDenom': 1,
      u'lotNumer': 1000,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'PRB',
      u'pipDp': 2,
      u'tickDenom': 20,
      u'tickNumer': 1
    }, {
      u'baseAsset': u'UIB',
      u'termCcy': u'USD',
      u'display': u'Uinta Basin Coal',
      u'lotDenom': 1,
      u'lotNumer': 1000,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'UIB',
      u'pipDp': 2,
      u'tickDenom': 20,
      u'tickNumer': 1
    }, {
      u'baseAsset': u'USD',
      u'termCcy': u'CHF',
      u'display': u'USDCHF',
      u'lotDenom': 1,
      u'lotNumer': 1000000,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'USDCHF',
      u'pipDp': 4,
      u'tickDenom': 10000,
      u'tickNumer': 1
    }, {
      u'baseAsset': u'USD',
      u'termCcy': u'JPY',
      u'display': u'USDJPY',
      u'lotDenom': 1,
      u'lotNumer': 1000000,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'USDJPY',
      u'pipDp': 2,
      u'tickDenom': 100,
      u'tickNumer': 1
    }, {
      u'baseAsset': u'VIA',
      u'termCcy': u'USD',
      u'display': u'Viacom Inc',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'VIA',
      u'pipDp': 3,
      u'tickDenom': 1000,
      u'tickNumer': 1
    }, {
      u'baseAsset': u'VOD',
      u'termCcy': u'USD',
      u'display': u'Vodafone Group Plc',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'VOD',
      u'pipDp': 3,
      u'tickDenom': 1000,
      u'tickNumer': 1
    }, {
      u'baseAsset': u'VZ',
      u'termCcy': u'USD',
      u'display': u'Verizon Com',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'VZ',
      u'pipDp': 3,
      u'tickDenom': 1000,
      u'tickNumer': 1
    }, {
      u'baseAsset': u'WGAA',
      u'termCcy': u'ETB',
      u'display': u'Gelena Abaya A',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'WGAA',
      u'pipDp': 0,
      u'tickDenom': 1,
      u'tickNumer': 1
    }, {
      u'baseAsset': u'WKCA',
      u'termCcy': u'ETB',
      u'display': u'Kochere A',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'WKCA',
      u'pipDp': 0,
      u'tickDenom': 1,
      u'tickNumer': 1
    }, {
      u'baseAsset': u'WWNA',
      u'termCcy': u'ETB',
      u'display': u'Wenago A',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'WWNA',
      u'pipDp': 0,
      u'tickDenom': 1,
      u'tickNumer': 1
    }, {
      u'baseAsset': u'WYCA',
      u'termCcy': u'ETB',
      u'display': u'Yirgachefe A',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'WYCA',
      u'pipDp': 0,
      u'tickDenom': 1,
      u'tickNumer': 1
    }], resp.content)

  def getBySymbol(self, client):
    client.setAnon()
    resp = client.send('GET', '/refdata/instr/EURUSD')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'baseAsset': u'EUR',
      u'termCcy': u'USD',
      u'display': u'EURUSD',
      u'lotDenom': 1,
      u'lotNumer': 1000000,
      u'maxLots': 10,
      u'minLots': 1,
      u'symbol': u'EURUSD',
      u'pipDp': 4,
      u'tickDenom': 10000,
      u'tickNumer': 1
    }, resp.content)

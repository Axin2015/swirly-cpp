# The Restful Matching-Engine.
# Copyright (C) 2013, 2016 Swirly Cloud Limited.
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
    with Fixture() as fixture:
      with Connection() as conn:
        conn.setTime(self.now)

        self.getAll(conn)
        self.getByMnem(conn)

  def getAll(self, conn):
    conn.setAnon()
    resp = conn.send('GET', '/ref/contr')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'asset': u'CAP',
      u'ccy': u'USD',
      u'display': u'Central Appalachia Coal',
      u'lotDenom': 1,
      u'lotNumer': 1000,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'CAP',
      u'pipDp': 2,
      u'tickDenom': 20,
      u'tickNumer': 1
    }, {
      u'asset': u'CSCO',
      u'ccy': u'USD',
      u'display': u'Cisco Systems Inc',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'CSCO',
      u'pipDp': 3,
      u'tickDenom': 1000,
      u'tickNumer': 1
    }, {
      u'asset': u'DIS',
      u'ccy': u'USD',
      u'display': u'Walt Disney',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'DIS',
      u'pipDp': 3,
      u'tickDenom': 1000,
      u'tickNumer': 1
    }, {
      u'asset': u'EUR',
      u'ccy': u'USD',
      u'display': u'EURUSD',
      u'lotDenom': 1,
      u'lotNumer': 1000000,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'EURUSD',
      u'pipDp': 4,
      u'tickDenom': 10000,
      u'tickNumer': 1
    }, {
      u'asset': u'GBP',
      u'ccy': u'USD',
      u'display': u'GBPUSD',
      u'lotDenom': 1,
      u'lotNumer': 1000000,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'GBPUSD',
      u'pipDp': 4,
      u'tickDenom': 10000,
      u'tickNumer': 1
    }, {
      u'asset': u'IBM',
      u'ccy': u'USD',
      u'display': u'Ibm Corp',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'IBM',
      u'pipDp': 3,
      u'tickDenom': 1000,
      u'tickNumer': 1
    }, {
      u'asset': u'ILB',
      u'ccy': u'USD',
      u'display': u'Illinois Basin Coal',
      u'lotDenom': 1,
      u'lotNumer': 1000,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'ILB',
      u'pipDp': 2,
      u'tickDenom': 20,
      u'tickNumer': 1
    }, {
      u'asset': u'INTC',
      u'ccy': u'USD',
      u'display': u'Intel Corp',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'INTC',
      u'pipDp': 3,
      u'tickDenom': 1000,
      u'tickNumer': 1
    }, {
      u'asset': u'MSFT',
      u'ccy': u'USD',
      u'display': u'Microsoft Corp',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'MSFT',
      u'pipDp': 3,
      u'tickDenom': 1000,
      u'tickNumer': 1
    }, {
      u'asset': u'NAP',
      u'ccy': u'USD',
      u'display': u'Northern Appalachia Coal',
      u'lotDenom': 1,
      u'lotNumer': 1000,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'NAP',
      u'pipDp': 2,
      u'tickDenom': 20,
      u'tickNumer': 1
    }, {
      u'asset': u'PRB',
      u'ccy': u'USD',
      u'display': u'Powder River Basin Coal',
      u'lotDenom': 1,
      u'lotNumer': 1000,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'PRB',
      u'pipDp': 2,
      u'tickDenom': 20,
      u'tickNumer': 1
    }, {
      u'asset': u'UIB',
      u'ccy': u'USD',
      u'display': u'Uinta Basin Coal',
      u'lotDenom': 1,
      u'lotNumer': 1000,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'UIB',
      u'pipDp': 2,
      u'tickDenom': 20,
      u'tickNumer': 1
    }, {
      u'asset': u'USD',
      u'ccy': u'CHF',
      u'display': u'USDCHF',
      u'lotDenom': 1,
      u'lotNumer': 1000000,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'USDCHF',
      u'pipDp': 4,
      u'tickDenom': 10000,
      u'tickNumer': 1
    }, {
      u'asset': u'USD',
      u'ccy': u'JPY',
      u'display': u'USDJPY',
      u'lotDenom': 1,
      u'lotNumer': 1000000,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'USDJPY',
      u'pipDp': 2,
      u'tickDenom': 100,
      u'tickNumer': 1
    }, {
      u'asset': u'VIA',
      u'ccy': u'USD',
      u'display': u'Viacom Inc',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'VIA',
      u'pipDp': 3,
      u'tickDenom': 1000,
      u'tickNumer': 1
    }, {
      u'asset': u'VOD',
      u'ccy': u'USD',
      u'display': u'Vodafone Group Plc',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'VOD',
      u'pipDp': 3,
      u'tickDenom': 1000,
      u'tickNumer': 1
    }, {
      u'asset': u'VZ',
      u'ccy': u'USD',
      u'display': u'Verizon Com',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'VZ',
      u'pipDp': 3,
      u'tickDenom': 1000,
      u'tickNumer': 1
    }, {
      u'asset': u'WGAA',
      u'ccy': u'ETB',
      u'display': u'Gelena Abaya A',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'WGAA',
      u'pipDp': 0,
      u'tickDenom': 1,
      u'tickNumer': 1
    }, {
      u'asset': u'WKCA',
      u'ccy': u'ETB',
      u'display': u'Kochere A',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'WKCA',
      u'pipDp': 0,
      u'tickDenom': 1,
      u'tickNumer': 1
    }, {
      u'asset': u'WWNA',
      u'ccy': u'ETB',
      u'display': u'Wenago A',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'WWNA',
      u'pipDp': 0,
      u'tickDenom': 1,
      u'tickNumer': 1
    }, {
      u'asset': u'WYCA',
      u'ccy': u'ETB',
      u'display': u'Yirgachefe A',
      u'lotDenom': 1,
      u'lotNumer': 1,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'WYCA',
      u'pipDp': 0,
      u'tickDenom': 1,
      u'tickNumer': 1
    }], resp.content)

  def getByMnem(self, conn):
    conn.setAnon()
    resp = conn.send('GET', '/ref/contr/EURUSD')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'asset': u'EUR',
      u'ccy': u'USD',
      u'display': u'EURUSD',
      u'lotDenom': 1,
      u'lotNumer': 1000000,
      u'maxLots': 10,
      u'minLots': 1,
      u'mnem': u'EURUSD',
      u'pipDp': 4,
      u'tickDenom': 10000,
      u'tickNumer': 1
    }, resp.content)

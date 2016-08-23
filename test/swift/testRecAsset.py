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
    resp = conn.send('GET', '/rec/asset')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([
      {u'display': u'Central Appalachia Coal', u'mnem': u'CAP',  u'type': u'COMMODITY'},
      {u'display': u'Switzerland, Francs', u'mnem': u'CHF', u'type': u'CURRENCY'},
      {u'display': u'Cisco Systems Inc', u'mnem': u'CSCO', u'type': u'CORPORATE'},
      {u'display': u'Walt Disney', u'mnem': u'DIS', u'type': u'CORPORATE'},
      {u'display': u'Ethiopia, Birr', u'mnem': u'ETB', u'type': u'CURRENCY'},
      {u'display': u'Euro Member Countries, Euro', u'mnem': u'EUR', u'type': u'CURRENCY'},
      {u'display': u'United Kingdom, Pounds', u'mnem': u'GBP',  u'type': u'CURRENCY'},
      {u'display': u'Ibm Corp', u'mnem': u'IBM', u'type': u'CORPORATE'},
      {u'display': u'Illinois Basin Coal', u'mnem': u'ILB', u'type': u'COMMODITY'},
      {u'display': u'Intel Corp', u'mnem': u'INTC', u'type': u'CORPORATE'},
      {u'display': u'Japan, Yen', u'mnem': u'JPY', u'type': u'CURRENCY'},
      {u'display': u'Microsoft Corp', u'mnem': u'MSFT', u'type': u'CORPORATE'},
      {u'display': u'Northern Appalachia Coal', u'mnem': u'NAP', u'type': u'COMMODITY'},
      {u'display': u'Powder River Basin Coal', u'mnem': u'PRB', u'type': u'COMMODITY'},
      {u'display': u'Uinta Basin Coal', u'mnem': u'UIB', u'type': u'COMMODITY'},
      {u'display': u'United States of America, Dollars', u'mnem': u'USD', u'type': u'CURRENCY'},
      {u'display': u'Viacom Inc', u'mnem': u'VIA', u'type': u'CORPORATE'},
      {u'display': u'Vodafone Group Plc', u'mnem': u'VOD', u'type': u'CORPORATE'},
      {u'display': u'Verizon Com', u'mnem': u'VZ', u'type': u'CORPORATE'},
      {u'display': u'Gelena Abaya A', u'mnem': u'WGAA', u'type': u'COMMODITY'},
      {u'display': u'Kochere A', u'mnem': u'WKCA', u'type': u'COMMODITY'},
      {u'display': u'Wenago A', u'mnem': u'WWNA', u'type': u'COMMODITY'},
      {u'display': u'Yirgachefe A', u'mnem': u'WYCA', u'type': u'COMMODITY'},
      {u'display': u'South Africa, Rand', u'mnem': u'ZAR', u'type': u'CURRENCY'}
    ], resp.content)

  def getByMnem(self, conn):
    resp = conn.send('GET', '/rec/asset/EUR')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertDictEqual({
      u'display': u'Euro Member Countries, Euro',
      u'mnem': u'EUR',
      u'type': u'CURRENCY'
    }, resp.content)

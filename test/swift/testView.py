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

import unittest

class TestCase(unittest.TestCase):

  def test(self):
    self.maxDiff = None
    self.now = 1459974268204
    with Fixture() as fixture:
      with Connection() as conn:
        self.createMarket(conn)
        self.makerBid1(conn)
        self.makerBid2(conn)
        self.makerBid3(conn)
        self.makerOffer1(conn)
        self.makerOffer2(conn)
        self.makerOffer3(conn)
        self.getView(conn)

  def createMarket(self, conn):
    conn.setTime(self.now)
    conn.setUser('MARAYL')
    resp = conn.send('POST', '/api/rec/market',
                     mnem = 'EURUSD.MAR14',
                     display = 'first',
                     contr = 'EURUSD',
                     settlDate = 20170102,
                     expiryDate = 20170101,
                     state = 1)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)

  def makerBid1(self, conn):
    conn.setTime(self.now)
    conn.setUser('MARAYL')
    resp = conn.send('POST', '/api/sess/order/EURUSD.MAR14',
                     side = 'BUY',
                     lots = 3,
                     ticks = 12344)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)

  def makerBid2(self, conn):
    conn.setTime(self.now)
    conn.setUser('MARAYL')
    resp = conn.send('POST', '/api/sess/order/EURUSD.MAR14',
                     side = 'BUY',
                     lots = 5,
                     ticks = 12343)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)

  def makerBid3(self, conn):
    conn.setTime(self.now)
    conn.setUser('MARAYL')
    resp = conn.send('POST', '/api/sess/order/EURUSD.MAR14',
                     side = 'BUY',
                     lots = 7,
                     ticks = 12343)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)

  def makerOffer1(self, conn):
    conn.setTime(self.now)
    conn.setUser('MARAYL')
    resp = conn.send('POST', '/api/sess/order/EURUSD.MAR14',
                     side = 'SELL',
                     lots = 3,
                     ticks = 12346)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)

  def makerOffer2(self, conn):
    conn.setTime(self.now)
    conn.setUser('MARAYL')
    resp = conn.send('POST', '/api/sess/order/EURUSD.MAR14',
                     side = 'SELL',
                     lots = 5,
                     ticks = 12347)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)

  def makerOffer3(self, conn):
    conn.setTime(self.now)
    conn.setUser('MARAYL')
    resp = conn.send('POST', '/api/sess/order/EURUSD.MAR14',
                     side = 'SELL',
                     lots = 7,
                     ticks = 12348)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)

  def getView(self, conn):
    conn.setTime(self.now)
    conn.setUser(None)
    resp = conn.send('GET', '/api/view')

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)
    self.assertListEqual([{
      u'bidCount': [1, 2, None],
      u'bidResd': [3, 12, None],
      u'bidTicks': [12344, 12343, None],
      u'contr': u'EURUSD',
      u'lastLots': None,
      u'lastTicks': None,
      u'lastTime': None,
      u'market': u'EURUSD.MAR14',
      u'offerCount': [1, 1, 1],
      u'offerResd': [3, 5, 7],
      u'offerTicks': [12346, 12347, 12348],
      u'settlDate': 20170102
    }], resp.content)

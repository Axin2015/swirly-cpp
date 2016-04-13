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

  @classmethod
  def setUpClass(cls):
    cls.fixture = Fixture()

  @classmethod
  def tearDownClass(cls):
    cls.fixture.close()
    cls = None

  def setUp(self):
    self.maxDiff = None

  def testCreate(self):
    with Connection() as conn:
      resp = conn.send('POST', '/api/rec/market',
                       mnem = 'USDJPY.MAR14',
                       display = 'first',
                       contr = 'USDJPY',
                       settlDate = 20170102,
                       expiryDate = 20170101,
                       state = 1)
      self.assertEqual(200, resp.status)
      self.assertEqual('OK', resp.reason)
      self.assertDictEqual({
        u'contr': u'USDJPY',
        u'display': u'first',
        u'expiryDate': 20170101,
        u'mnem': u'USDJPY.MAR14',
        u'settlDate': 20170102,
        u'state': 1
      }, resp.content)

  def testUpdate(self):
    with Connection() as conn:

      # Update display and state.
      resp = conn.send('PUT', '/api/rec/market/USDJPY.MAR14',
                       display = 'second',
                       state = 2)
      self.assertEqual(200, resp.status)
      self.assertEqual('OK', resp.reason)
      self.assertDictEqual({
        u'contr': u'USDJPY',
        u'display': u'second',
        u'expiryDate': 20170101,
        u'mnem': u'USDJPY.MAR14',
        u'settlDate': 20170102,
        u'state': 2
      }, resp.content)

      # Update display only.
      resp = conn.send('PUT', '/api/rec/market/USDJPY.MAR14',
                       display = 'third',
                       state = None)
      self.assertEqual(200, resp.status)
      self.assertEqual('OK', resp.reason)
      self.assertDictEqual({
        u'contr': u'USDJPY',
        u'display': u'third',
        u'expiryDate': 20170101,
        u'mnem': u'USDJPY.MAR14',
        u'settlDate': 20170102,
        u'state': 2
      }, resp.content)

      # Update state only.
      resp = conn.send('PUT', '/api/rec/market/USDJPY.MAR14',
                       display = None,
                       state = 3)
      self.assertEqual(200, resp.status)
      self.assertEqual('OK', resp.reason)
      self.assertDictEqual({
        u'contr': u'USDJPY',
        u'display': u'third',
        u'expiryDate': 20170101,
        u'mnem': u'USDJPY.MAR14',
        u'settlDate': 20170102,
        u'state': 3
      }, resp.content)

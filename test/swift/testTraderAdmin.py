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
      resp = conn.send('POST', '/api/rec/trader',
                       mnem = 'MARAYL2',
                       display = 'Mark Aylettx',
                       email = 'mark.aylett@swirlycloud.com')
      self.assertEqual(200, resp.status)
      self.assertEqual('OK', resp.reason)
      self.assertDictEqual({
        u'display': u'Mark Aylettx',
        u'email': u'mark.aylett@swirlycloud.com',
        u'mnem': u'MARAYL2'
      }, resp.content)

  def testUpdate(self):
    with Connection() as conn:

      resp = conn.send('PUT', '/api/rec/trader/MARAYL2',
                       display = 'Mark Aylett')
      self.assertEqual(200, resp.status)
      self.assertEqual('OK', resp.reason)
      self.assertDictEqual({
        u'display': u'Mark Aylett',
        u'email': u'mark.aylett@swirlycloud.com',
        u'mnem': u'MARAYL2'
      }, resp.content)

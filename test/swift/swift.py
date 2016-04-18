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

from contextlib import closing

import httplib

import json
import os
import socket
import subprocess
import tempfile
import time
import unittest

class Process(object):

  def __init__(self, prog, logLevel, logFile, port):
    proc = subprocess.Popen([
      prog,
      '-e' + str(logLevel),
      '-l' + logFile,
      '-n',
      '-p' + str(port)
    ])
    self.proc = proc

  def __enter__(self):
    return self

  def __exit__(self, extype, exval, bt):
    self.close()

  def close(self):
    proc = self.proc
    proc.terminate()
    proc.wait()

def getPort():
  with closing(socket.socket(socket.AF_INET, socket.SOCK_STREAM)) as sock:
    sock.bind(('localhost', 0))
    addr, port = sock.getsockname()
    return port

def getProg():
  return os.getenv('SWIRLYD_EXECUTABLE', 'swirlyd')

def readLine(file):
  while True:
    where = file.tell()
    line = file.readline()
    if not line:
      time.sleep(0.1)
      file.seek(where)
    else:
      return line

class Fixture(object):

  port = getPort()
  prog = getProg()

  def __init__(self):
    temp = tempfile.NamedTemporaryFile(delete = True)
    proc = None
    try:
      proc = Process(Fixture.prog, 3, temp.name, Fixture.port)
      try:
        line = readLine(temp)
      except:
        proc.close()
        raise
    except:
      temp.close()
      raise
    self.temp = temp
    self.proc = proc

  def __enter__(self):
    return self

  def __exit__(self, extype, exval, bt):
    self.close()

  def close(self):
    try:
      self.proc.close()
    finally:
      self.temp.close()

class Response(object):

  def __init__(self, resp):
    self.status = resp.status
    self.reason = resp.reason
    self.content = json.load(resp)

  def __str__(self):
    return ('Response(status={},reason="{}",content="{}")'
            .format(self.status, self.reason, self.content))

class Connection(object):

  def __init__(self):
    self.conn = httplib.HTTPConnection('localhost', Fixture.port)
    self.time = None
    self.user = None

  def __enter__(self):
    return self

  def __exit__(self, extype, exval, bt):
    self.close()

  def close(self):
    self.conn.close()

  def setTime(self, time):
    self.time = time

  def setUser(self, user):
    self.user = user

  def send(self, method, uri, **kwargs):
    content = ''
    if kwargs is not None:
      content = json.dumps(kwargs);
    conn = self.conn
    conn.putrequest(method, uri)
    conn.putheader('Accept', 'application/json')
    if self.time is not None:
      conn.putheader('Swirly-Time', self.time)
    if self.user is not None:
      conn.putheader('Swirly-User', self.user)
    conn.putheader('Content-Length', len(content))
    conn.putheader('Content-Type', 'application/json')
    conn.endheaders()
    conn.send(content)
    return Response(conn.getresponse())

class RestTestCase(unittest.TestCase):

  def createMarket(self, conn, mnem, contr, settlDate, expiryDate):
    resp = conn.send('POST', '/api/rec/market',
                     mnem = mnem,
                     display = mnem,
                     contr = contr,
                     settlDate = settlDate,
                     expiryDate = expiryDate,
                     state = 0)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)

  def createOrder(self, conn, market, side, lots, ticks):
    resp = conn.send('POST', '/api/sess/order/' + market,
                     side = side,
                     lots = lots,
                     ticks = ticks)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)

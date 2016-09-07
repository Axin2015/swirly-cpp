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

import errno
import httplib
import json
import os
import socket
import sqlite3
import subprocess
import tempfile
import time
import unittest

def waitForService(server, port, timeout):
  end = time.time() + timeout
  while True:

    nextTimeout = end - time.time()
    if nextTimeout <= 0:
      return False

    try:
      # Sleep for 10ms before attempting to connect.
      time.sleep(0.01)
      with closing(socket.socket(socket.AF_INET, socket.SOCK_STREAM)) as sock:
        sock.settimeout(nextTimeout)
        sock.connect((server, port))

    except socket.timeout, err:
      return False

    except socket.error, err:
      if type(err.args) != tuple or err[0] != errno.ECONNREFUSED:
        raise
    else:
      return True

def getPort():
  with closing(socket.socket(socket.AF_INET, socket.SOCK_STREAM)) as sock:
    sock.bind(('localhost', 0))
    addr, port = sock.getsockname()
    return port

def getProg():
  return os.getenv('SWIRLY_PROGRAM', 'swirlyd')

def getSchema():
  return os.getenv('SWIRLY_SCHEMA', 'schema.sql')

def getRefData():
  return os.getenv('SWIRLY_REFDATA', 'test.sql')

def executeScript(conn, path):
  cur = conn.cursor()
  with open(path, 'r') as fd:
    script = fd.read()
    cur.executescript(script)

class ConfFile(object):
  def __init__(self):
    self.temp = tempfile.NamedTemporaryFile(delete = True)

  def __enter__(self):
    return self

  def __exit__(self, extype, exval, bt):
    self.close()

  def close(self):
    self.temp.close()
    self.temp = None

  def set(self, key, val):
    self.temp.write('{}={}\n'.format(key, val))
    self.temp.flush()

  @property
  def name(self):
    return self.temp.name

class DbFile(object):

  schema = getSchema()
  refData = getRefData()

  def __init__(self):
    temp = tempfile.NamedTemporaryFile(delete = True)
    try:
      with sqlite3.connect(temp.name) as conn:
        executeScript(conn, DbFile.schema)
        executeScript(conn, DbFile.refData)
    except:
      temp.close()
      raise
    self.temp = temp

  def __enter__(self):
    return self

  def __exit__(self, extype, exval, bt):
    self.close()

  def close(self):
    self.temp.close()
    self.temp = None

  @property
  def name(self):
    return self.temp.name

class LogFile(object):
  def __init__(self):
    self.temp = tempfile.NamedTemporaryFile(delete = True)

  def __enter__(self):
    return self

  def __exit__(self, extype, exval, bt):
    self.close()

  def close(self):
    for line in self.temp:
      print line.strip('\n')
    self.temp.close()
    self.temp = None

  @property
  def name(self):
    return self.temp.name

class Process(object):

  def __init__(self, prog, confFile, startTime):
    proc = subprocess.Popen([
      prog,
      '-f' + confFile,
      '-n',
      '-s' + str(startTime)
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

class Server(object):

  port = getPort()
  prog = getProg()

  def __init__(self, dbFile, startTime):
    confFile = None
    logFile = None
    proc = None
    try:
      confFile = ConfFile()
      logFile = LogFile()
      confFile.set('log_file', logFile.name)
      confFile.set('log_level', 5)
      confFile.set('http_port', Server.port)
      confFile.set('sqlite_journ', dbFile.name)
      confFile.set('sqlite_model', dbFile.name)
      confFile.set('sqlite_enable_trace', 'yes')
      confFile.set('sqlite_enable_fkey', 'yes')
      proc = Process(Server.prog, confFile.name, startTime)
      if not waitForService('localhost', Server.port, 5):
        raise RuntimeError, 'Failed to start service'
    except:
      if proc is not None:
        proc.close()
      if logFile is not None:
        logFile.close()
      if confFile is not None:
        confFile.close()
      raise

    self.confFile = confFile
    self.logFile = logFile
    self.proc = proc

  def __enter__(self):
    return self

  def __exit__(self, extype, exval, bt):
    self.close()

  def close(self):
    self.proc.close()
    self.logFile.close()
    self.confFile.close()

class Response(object):

  def __init__(self, resp):
    self.status = resp.status
    self.reason = resp.reason
    self.content = json.load(resp)

  def __str__(self):
    return ('Response(status={},reason="{}",content="{}")'
            .format(self.status, self.reason, self.content))

class Client(object):

  def __init__(self):
    self.conn = httplib.HTTPConnection('localhost', Server.port)
    self.time = None
    self.accnt = None
    self.perm = None

  def __enter__(self):
    return self

  def __exit__(self, extype, exval, bt):
    self.close()

  def close(self):
    self.conn.close()

  def setTime(self, time):
    self.time = time

  def setAuth(self, accnt, perm = None):
    self.accnt = accnt
    self.perm = perm

  def setAnon(self):
    self.accnt = None
    self.perm = None

  def setAdmin(self):
    self.accnt = 'ADMIN'
    self.perm = 0x1

  def setTrader(self, accnt):
    self.accnt = accnt
    self.perm = 0x2

  def send(self, method, uri, **kwargs):
    content = ''
    if kwargs is not None:
      content = json.dumps(kwargs)
    conn = self.conn
    conn.putrequest(method, uri)
    conn.putheader('Accept', 'application/json')
    if self.time is not None:
      conn.putheader('Swirly-Time', self.time)
    if self.accnt is not None:
      conn.putheader('Swirly-Accnt', self.accnt)
    if self.perm is not None:
      conn.putheader('Swirly-Perm', self.perm)
    conn.putheader('Content-Length', len(content))
    conn.putheader('Content-Type', 'application/json')
    conn.endheaders()
    conn.send(content)
    return Response(conn.getresponse())

class RestTestCase(unittest.TestCase):

  def createMarket(self, client, contr, settlDate):
    client.setAdmin()
    resp = client.send('POST', '/market',
                       contr = contr,
                       settlDate = settlDate,
                       state = 0)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)

  def createOrder(self, client, accnt, contr, settlDate, side, lots, ticks):
    client.setTrader(accnt)
    resp = client.send('POST', '/accnt/order/' + contr + '/' + str(settlDate),
                       side = side,
                       lots = lots,
                       ticks = ticks)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)

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

def wait_for_service(server, port, timeout):
  end = time.time() + timeout
  while True:

    next_timeout = end - time.time()
    if next_timeout <= 0:
      return False

    try:
      # Sleep for 10ms before attempting to connect.
      time.sleep(0.01)
      with closing(socket.socket(socket.AF_INET, socket.SOCK_STREAM)) as sock:
        sock.settimeout(next_timeout)
        sock.connect((server, port))

    except socket.timeout, err:
      return False

    except socket.error, err:
      if type(err.args) != tuple or err[0] != errno.ECONNREFUSED:
        raise
    else:
      return True

def get_port():
  with closing(socket.socket(socket.AF_INET, socket.SOCK_STREAM)) as sock:
    sock.bind(('localhost', 0))
    addr, port = sock.getsockname()
    return port

def get_prog():
  return os.getenv('SWIRLY_PROGRAM', 'swirlyd')

def get_schema():
  return os.getenv('SWIRLY_SCHEMA', 'schema.sqlite')

def get_ref_data():
  return os.getenv('SWIRLY_REFDATA', 'test.sql')

def execute_script(conn, path):
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

  schema = get_schema()
  ref_data = get_ref_data()

  def __init__(self):
    temp = tempfile.NamedTemporaryFile(delete = True)
    try:
      with sqlite3.connect(temp.name) as conn:
        execute_script(conn, DbFile.schema)
        execute_script(conn, DbFile.ref_data)
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

  def __init__(self, prog, conf_file, start_time):
    proc = subprocess.Popen([
      prog,
      '-f' + conf_file,
      '-s' + str(start_time)
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

  port = get_port()
  prog = get_prog()

  def __init__(self, db_file, start_time):
    conf_file = None
    log_file = None
    proc = None
    try:
      conf_file = ConfFile()
      log_file = LogFile()
      conf_file.set('log_file', log_file.name)
      conf_file.set('log_level', 5)
      conf_file.set('http_port', Server.port)
      conf_file.set('sqlite_journ', db_file.name)
      conf_file.set('sqlite_model', db_file.name)
      conf_file.set('sqlite_enable_trace', 'yes')
      conf_file.set('sqlite_enable_fkey', 'yes')
      proc = Process(Server.prog, conf_file.name, start_time)
      if not wait_for_service('localhost', Server.port, 5):
        raise RuntimeError, 'Failed to start service'
    except:
      if proc is not None:
        proc.close()
      if log_file is not None:
        log_file.close()
      if conf_file is not None:
        conf_file.close()
      raise

    self.conf_file = conf_file
    self.log_file = log_file
    self.proc = proc

  def __enter__(self):
    return self

  def __exit__(self, extype, exval, bt):
    self.close()

  def close(self):
    self.proc.close()
    self.log_file.close()
    self.conf_file.close()

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

  def set_time(self, time):
    self.time = time

  def set_auth(self, accnt, perm = None):
    self.accnt = accnt
    self.perm = perm

  def set_anon(self):
    self.accnt = None
    self.perm = None

  def set_admin(self):
    self.accnt = 'ADMIN'
    self.perm = 0x1

  def set_trader(self, accnt):
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

  def create_market(self, client, instr, settl_date):
    client.set_admin()
    resp = client.send('POST', '/markets',
                       instr = instr,
                       settl_date = settl_date,
                       state = 0)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)

  def create_order(self, client, accnt, instr, settl_date, side, lots, ticks):
    client.set_trader(accnt)
    resp = client.send('POST', '/accnt/orders/' + instr + '/' + str(settl_date),
                       side = side,
                       lots = lots,
                       ticks = ticks)

    self.assertEqual(200, resp.status)
    self.assertEqual('OK', resp.reason)

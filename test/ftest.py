#!/usr/bin/env python

# This script is intended as the basis for automated functional testing.

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
  buildDir = os.getenv('SWIRLY_BUILD')
  return buildDir + '/bin/swirlyd' if buildDir else 'swirlyd'

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
    temp = tempfile.NamedTemporaryFile()
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

class Connection(object):

  def __init__(self):
    self.conn = httplib.HTTPConnection('localhost', Fixture.port)

  def __enter__(self):
    return self

  def __exit__(self, extype, exval, bt):
    self.close()

  def close(self):
    self.conn.close()

  def send(self, method, uri):
    content = json.dumps({
      'side': 'BUY',
      'lots': 1,
      'ticks': 12340
    })
    conn = self.conn
    conn.putrequest(method, uri)
    conn.putheader('Accept', 'application/json')
    conn.putheader('Content-Length', len(content))
    conn.putheader('Content-Type', 'application/json')
    conn.endheaders()
    conn.send(content)
    return Response(conn.getresponse())

class TestSwirly(unittest.TestCase):

  @classmethod
  def setUpClass(cls):
    cls.fixture = Fixture()

  @classmethod
  def tearDownClass(cls):
    cls.fixture.close()
    cls = None

  def test_rec(self):
    with Connection() as conn:
      self.assertEqual(conn.send('POST', '/api/rec/market').status, 200)
      self.assertEqual(conn.send('PUT', '/api/rec/market/EURUSD').status, 200)
      self.assertEqual(conn.send('POST', '/api/rec/trader').status, 200)
      self.assertEqual(conn.send('PUT', '/api/rec/trader/MARAYL').status, 200)

  def test_sess(self):
    with Connection() as conn:
      self.assertEqual(conn.send('POST', '/api/sess/order/EURUSD').status, 200)
      self.assertEqual(conn.send('PUT', '/api/sess/order/EURUSD/1,2,3').status, 200)
      self.assertEqual(conn.send('DELETE', '/api/sess/order/EURUSD/1,2,3').status, 200)
      self.assertEqual(conn.send('POST', '/api/sess/trade/EURUSD').status, 200)
      self.assertEqual(conn.send('DELETE', '/api/sess/trade/EURUSD/1,2,3').status, 200)

if __name__ == '__main__':
  unittest.main()

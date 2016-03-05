#!/usr/bin/env python
import httplib
import json

# This script is intended as the basis for automated functional testing.

body = json.dumps({
    'side': 'BUY',
    'lots': 1,
    'ticks': 12340
})

def test(conn, method, uri):
    conn.putrequest(method, uri)
    conn.putheader('Accept', 'application/json')
    conn.putheader('Connection', 'keep-alive')
    conn.putheader('Content-Length', len(body))
    conn.putheader('Content-Type', 'application/json')
    conn.endheaders()
    conn.send(body)

    resp = conn.getresponse()
    data = resp.read();
    print('status: ' + str(resp.status))
    print(data)

conn = httplib.HTTPConnection('localhost', 8080)
test(conn, 'POST', '/api/rec/market')
test(conn, 'PUT', '/api/rec/market/EURUSD')
test(conn, 'POST', '/api/rec/trader')
test(conn, 'PUT', '/api/rec/trader/MARAYL')
test(conn, 'POST', '/api/sess/order/EURUSD')
test(conn, 'PUT', '/api/sess/order/EURUSD/1,2,3')
test(conn, 'DELETE', '/api/sess/order/EURUSD/1,2,3')
test(conn, 'POST', '/api/sess/trade/EURUSD')
test(conn, 'DELETE', '/api/sess/trade/EURUSD/1,2,3')
conn.close()

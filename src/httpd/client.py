#!/usr/bin/env python
import httplib
import json

body = json.dumps({
    'accnt': 'DBRA',
    'market': 'EURUSD',
    'action': 'BUY',
    'ticks': 12340,
    'lots': 1
})

conn = httplib.HTTPConnection('localhost', 8080)

for x in range(5):
    conn.putrequest('POST', '/rec/a')
    conn.putheader('Content-Type', 'application/json')
    conn.putheader('Content-Length', str(len(body)))
    conn.putheader('Accept', 'application/json')
    conn.endheaders()
    conn.send(body)

    resp = conn.getresponse()
    data = resp.read();
    print('status: ' + str(resp.status))
    print(data)

conn.close()

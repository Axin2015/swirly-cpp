#!/usr/bin/env python

from dbrpy import *
from threading import Thread

import json
import web

TMOUT = 2000

class WorkerHandler(Handler):
    def __init__(self, clnt):
        super(WorkerHandler, self).__init__()
        self.clnt = clnt
    def on_async(self, fn):
        return fn(self.clnt)

def worker(ctx):
    pool = Pool(8 * 1024 * 1024)
    clnt = Clnt(ctx, 'TEST', 'tcp://localhost:3270', 'tcp://localhost:3271', millis(), pool)
    handler = WorkerHandler(clnt)
    while clnt.is_open():
        print('polling')
        clnt.poll(TMOUT, handler)

class CloseRequest(object):
    def __call__(self, clnt):
        return clnt.close(TMOUT)

class TraderRequest(object):
    def __init__(self, mnems):
        self.mnems = mnems
    def __call__(self, clnt):
        traders = []
        if self.mnems:
            for mnem in self.mnems:
                trec = clnt.find_rec_mnem(ENTITY_TRADER, mnem)
                if trec:
                    traders.append({'id': trec.id})
        else:
            for trec in clnt.list_rec(ENTITY_TRADER):
                traders.append({'id': trec.id})
        return traders

urls = ('/api/trader', 'TraderHandler')

class TraderHandler:
    def GET(self):
        async = web.ctx.async
        async.send(TraderRequest(web.input(mnem = []).mnem))
        return json.dumps(async.recv())

def run():
    ctx = ZmqCtx()
    thread = Thread(target = worker, args = (ctx,))
    thread.start()
    async = Async(ctx, 'TEST')
    app = web.application(urls, globals())
    def loadhook():
        web.ctx.async = async
    app.add_processor(web.loadhook(loadhook))
    app.run()
    print('shutdown')
    async.send(CloseRequest())
    async.recv()
    thread.join()

if __name__ == '__main__':
    try:
        run()
    except Error as e:
        print 'error: ' + str(e)

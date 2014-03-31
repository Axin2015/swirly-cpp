#!/usr/bin/env python

from contextlib import contextmanager
from dbrpy import *
from threading import Thread

import os, shlex, sys

SESS = 'TEST'
TMOUT = 5000

def parse(lex, obj):
    toks = []
    while True:
        tok = lex.get_token()
        if tok == lex.eof:
            break
        if tok == ';':
            if len(toks) > 0:
                fn = getattr(obj, toks[0])
                fn(*toks[1:])
                toks = []
        else:
            toks.append(tok)

class RequestHandler(Handler):
    def __init__(self, clnt):
        super(RequestHandler, self).__init__()
        self.clnt = clnt

    def on_close(self):
        print('on_close')

    def on_ready(self):
        print('on_ready')

    def on_logon(self, tid):
        print('on_logon: {0}'.format(tid))

    def on_logoff(self, tid):
        print('on_logoff: {0}'.format(tid))

    def on_timeout(self, req_id):
        print('on_timeout: {0}'.format(req_id))

    def on_status(self, req_id, num, msg):
        print('on_status: {0}: {2} ({1})'.format(req_id, num, msg))

    def on_exec(self, req_id, exc):
        print('on_exec: {0}: {1}'.format(req_id, exc))

    def on_posn(self, posn):
        print('on_posn: {0}'.format(posn))

    def on_view(self, view):
        print('on_view: {0}'.format(view))

    def on_flush(self):
        print('on_flush')

    def on_async(self, fn):
        ret = None
        try:
            ret = fn(self.clnt)
        except Error as e:
            print 'error: ' + str(e)
        return ret

def worker(ctx):
    pool = Pool(8 * 1024 * 1024)
    clnt = Clnt(ctx, SESS, 'tcp://localhost:3270', 'tcp://localhost:3271',
                millis(), pool)
    handler = RequestHandler(clnt)
    while not clnt.is_ready():
        clnt.dispatch(TMOUT, handler)
    while clnt.is_open():
        clnt.dispatch(TMOUT, handler)

class CloseRequest(object):
    def __call__(self, clnt):
        return clnt.close(TMOUT)

class LogonRequest(object):
    def __init__(self, mnem):
        self.mnem = mnem
    def __call__(self, clnt):
        trec = clnt.find_rec_mnem(ENTITY_TRADER, self.mnem)
        return clnt.logon(trec, TMOUT) if trec else -1

class LogoffRequest(object):
    def __init__(self, mnem):
        self.mnem = mnem
    def __call__(self, clnt):
        trec = clnt.find_rec_mnem(ENTITY_TRADER, self.mnem)
        return clnt.logoff(trec, TMOUT) if trec else -1

class TraderRequest(object):
    @staticmethod
    def toDict(trec):
        return {
            'id': trec.id,
            'mnem': trec.mnem,
            'display': trec.display,
            'email': trec.email
        }
    def __init__(self, mnems):
        self.mnems = set(mnems)
    def __call__(self, clnt):
        traders = []
        if self.mnems:
            for mnem in self.mnems:
                trec = clnt.find_rec_mnem(ENTITY_TRADER, mnem)
                if trec:
                    traders.append(TraderRequest.toDict(trec))
        else:
            traders = [TraderRequest.toDict(trec)
                       for trec in clnt.list_rec(ENTITY_TRADER)]
        return traders

class AccntRequest(object):
    @staticmethod
    def toDict(arec):
        return {
            'id': arec.id,
            'mnem': arec.mnem,
            'display': arec.display,
            'email': arec.email
        }
    def __init__(self, mnems):
        self.mnems = set(mnems)
    def __call__(self, clnt):
        accnts = []
        if self.mnems:
            for mnem in self.mnems:
                arec = clnt.find_rec_mnem(ENTITY_ACCNT, mnem)
                if arec:
                    accnts.append(AccntRequest.toDict(arec))
        else:
            accnts = [AccntRequest.toDict(arec)
                      for arec in clnt.list_rec(ENTITY_ACCNT)]
        return accnts

class ContrRequest(object):
    @staticmethod
    def toDict(crec):
        return {
            'id': crec.id,
            'mnem': crec.mnem,
            'display': crec.display,
            'asset_type': crec.asset_type,
            'asset': crec.asset,
            'ccy': crec.ccy,
            'tick_numer': crec.tick_numer,
            'tick_denom': crec.tick_denom,
            'lot_numer': crec.lot_numer,
            'lot_denom': crec.lot_denom,
            'price_dp': crec.price_dp,
            'pip_dp': crec.pip_dp,
            'qty_dp': crec.qty_dp,
            'min_lots': crec.min_lots,
            'max_lots': crec.max_lots
        }
    def __init__(self, mnems):
        self.mnems = set(mnems)
    def __call__(self, clnt):
        contrs = []
        if self.mnems:
            for mnem in self.mnems:
                crec = clnt.find_rec_mnem(ENTITY_CONTR, mnem)
                if crec:
                    contrs.append(ContrRequest.toDict(crec))
        else:
            contrs = [ContrRequest.toDict(crec)
                      for crec in clnt.list_rec(ENTITY_CONTR)]
        return contrs

class OrderRequest(object):
    @staticmethod
    def toDict(order):
        return {
            'id': order.id
        }
    def __init__(self, mnem):
        self.mnem = mnem
    def __call__(self, clnt):
        orders = []
        trec = clnt.find_rec_mnem(ENTITY_TRADER, self.mnem)
        if trec:
            trader = clnt.trader(trec)
            orders = [OrderRequest.toDict(order)
                      for order in trader.list_order()]
        return orders

class ExecRequest(object):
    def __init__(self, mnem):
        self.mnem = mnem
    def __call__(self, clnt):
        return []

class MembRequest(object):
    def __init__(self, mnem):
        self.mnem = mnem
    def __call__(self, clnt):
        return []

class PosnRequest(object):
    def __init__(self, mnem):
        self.mnem = mnem
    def __call__(self, clnt):
        return []

class ViewRequest(object):
    @staticmethod
    def levelDict(level):
        return {
            'ticks': level.ticks,
            'lots': level.lots,
            'count': level.count
        }
    @staticmethod
    def toDict(view):
        return {
            'cid': view.cid,
            'settl_date': view.settl_date,
            'list_bid': [ViewRequest.levelDict(level) for level in view.list_bid],
            'list_ask': [ViewRequest.levelDict(level) for level in view.list_ask]
        }
    def __init__(self, mnems, settl_dates):
        self.mnems = set(mnems)
        self.settl_dates = {int(x) for x in settl_dates}
    def __call__(self, clnt):
        cids = set()
        for mnem in self.mnems:
            crec = clnt.find_rec_mnem(ENTITY_CONTR, mnem)
            if crec:
                cids.add(crec.id)
        views = []
        if cids:
            if self.settl_dates:
                # Cids and settl_dates.
                for cid in cids:
                    for settl_date in self.settl_dates:
                        view = clnt.find_view(crec.id, settl_date)
                        if view:
                            views.append(ViewRequest.toDict(view))
            else:
                # Cids only.
                views = [ViewRequest.toDict(view) for view in clnt.list_view()
                         if view.cid in cids]
        else:
            if self.settl_dates:
                # Settl_dates only.
                views = [ViewRequest.toDict(view) for view in clnt.list_view()
                         if view.settl_date in self.settl_dates]
            else:
                # Neither cids nor settl_dates.
                views = [ViewRequest.toDict(view) for view in clnt.list_view()]
        return views

class AsyncClnt(object):
    def __init__(self):
        self.ctx = ZmqCtx()
        self.thread = Thread(target = worker, args = (self.ctx,))
        self.thread.start()
        self.async = Async(self.ctx, SESS)

    def close(self):
        self.async.send(CloseRequest())
        self.async.recv()
        print('joining')
        self.thread.join()

    def accnts(self):
        print('accnts')

    def ack_trades(self, *args):
        print('ack_trades: ' + ','.join(args))

    def buy(self, lots, price):
        print('buy: {0},{1}'.format(lots, price))

    def cancel(self, *args):
        print('cancel: ' + ','.join(args))

    def contrs(self):
        print('contrs')

    def echo(self, *args):
        print('echo: ' + ','.join(args))

    def env(self):
        print('env')

    def logon(self, tid):
        print('logon: {0}'.format(tid))

    def logoff(self):
        print('logoff')

    def orders(self):
        print('orders')

    def posns(self):
        print('posns')

    def quit(self):
        print('quit')

    def revise(self, id, lots):
        print('revise: {0},{1}'.format(id, lots))

    def sell(self, lots, price):
        print('sell: {0},{1}'.format(lots, price))

    def set(self, name, value):
        print('set: {0},{1}'.format(name, value))

    def traders(self):
        print('traders')

    def trades(self):
        print('trades')

    def unset(self, name):
        print('unset: {0}'.format(name))

    def view(self):
        print('view')

@contextmanager
def async_clnt():
    ac = AsyncClnt()
    try:
        yield ac
    finally:
        print('closing')
        ac.close()

if __name__ == '__main__':
    try:
        lex = shlex.shlex(sys.stdin)
        home = os.environ['HOME']
        rcfile = os.path.join(home, '.dbr_clirc')
        if os.path.isfile(rcfile):
            lex.push_source(open(rcfile, 'r'), rcfile)
        with async_clnt() as ac:
            parse(lex, ac)
    except KeyboardInterrupt as e:
        print 'interrupted'
    except Error as e:
        print 'error: ' + str(e)

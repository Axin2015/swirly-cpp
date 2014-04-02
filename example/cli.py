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
            log_info('end-of-file')
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
        log_info('on_close')

    def on_ready(self):
        log_info('on_ready')

    def on_logon(self, tid):
        log_info('on_logon: {0}'.format(tid))

    def on_logoff(self, tid):
        log_info('on_logoff: {0}'.format(tid))

    def on_reset(self):
        log_info('on_reset')

    def on_timeout(self, req_id):
        log_info('on_timeout: {0}'.format(req_id))

    def on_status(self, req_id, num, msg):
        log_info('on_status: {0}: {2} ({1})'.format(req_id, num, msg))

    def on_exec(self, req_id, exc):
        log_info('on_exec: {0}: {1}'.format(req_id, exc))

    def on_posn(self, posn):
        log_info('on_posn: {0}'.format(posn))

    def on_view(self, view):
        log_info('on_view: {0}'.format(view))

    def on_flush(self):
        log_info('on_flush')

    def on_async(self, fn):
        ret = None
        try:
            ret = fn(self.clnt)
        except Error as e:
            log_error('error: ' + str(e))
        return ret

def worker(ctx):
    pool = Pool(8 * 1024 * 1024)
    clnt = Clnt(ctx, SESS, 'tcp://localhost:3270', 'tcp://localhost:3271',
                millis(), TMOUT, pool)
    handler = RequestHandler(clnt)
    while True:
        try:
            if not clnt.dispatch(TMOUT, handler):
                break
        except Error as e:
            log_error('error: ' + str(e))

class CloseRequest(object):
    def __call__(self, clnt):
        return clnt.close()

class LogonRequest(object):
    def __init__(self, mnem):
        self.mnem = mnem
    def __call__(self, clnt):
        trec = clnt.find_rec_mnem(ENTITY_TRADER, self.mnem)
        return clnt.logon(trec) if trec else -1

class LogoffRequest(object):
    def __init__(self, mnem):
        self.mnem = mnem
    def __call__(self, clnt):
        trec = clnt.find_rec_mnem(ENTITY_TRADER, self.mnem)
        return clnt.logoff(trec) if trec else -1

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
        log_info('joining')
        self.thread.join()

    def accnts(self):
        log_info('accnts')

    def ack_trades(self, *args):
        log_info('ack_trades: ' + ','.join(args))

    def buy(self, lots, price):
        log_info('buy: {0},{1}'.format(lots, price))

    def cancel(self, *args):
        log_info('cancel: ' + ','.join(args))

    def contrs(self):
        log_info('contrs')

    def echo(self, *args):
        log_info('echo: ' + ','.join(args))

    def env(self):
        log_info('env')

    def logon(self, tid):
        log_info('logon: {0}'.format(tid))

    def logoff(self):
        log_info('logoff')

    def orders(self):
        log_info('orders')

    def posns(self):
        log_info('posns')

    def quit(self):
        log_info('quit')

    def revise(self, id, lots):
        log_info('revise: {0},{1}'.format(id, lots))

    def sell(self, lots, price):
        log_info('sell: {0},{1}'.format(lots, price))

    def set(self, name, value):
        log_info('set: {0},{1}'.format(name, value))

    def traders(self):
        log_info('traders')

    def trades(self):
        log_info('trades')

    def unset(self, name):
        log_info('unset: {0}'.format(name))

    def view(self):
        log_info('view')

@contextmanager
def async_clnt():
    ac = AsyncClnt()
    try:
        yield ac
    finally:
        log_info('closing')
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
        log_info('exiting')
    except KeyboardInterrupt as e:
        log_warn('interrupted')
    except Error as e:
        log_error('error: ' + str(e))

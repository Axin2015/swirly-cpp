#!/usr/bin/env python

from contextlib import contextmanager
from dbrpy import *
from threading import Thread
from uuid import *

import os, shlex, sys

TMOUT = 5000

def parse(lex, obj):
    toks = []
    while not obj.is_closed:
        tok = lex.get_token()
        if tok == lex.eof:
            log_info('end-of-file')
            break
        if tok == ';':
            if len(toks) > 0:
                try:
                    fn = getattr(obj, toks[0])
                    fn(*toks[1:])
                except Exception as e:
                    log_error('error: ' + str(e))
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

    def on_logon(self, uid):
        log_info('on_logon: {0}'.format(uid))

    def on_logoff(self, uid):
        log_info('on_logoff: {0}'.format(uid))

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
        pass

    def on_async(self, fn):
        ret = None
        try:
            ret = fn(self.clnt)
        except Exception as e:
            ret = e
        return ret

def worker(ctx, uuid):
    log_setlevel(3)
    pool = Pool(8 * 1024 * 1024)
    clnt = Clnt(ctx, uuid, 'tcp://localhost:3270', 'tcp://localhost:3271',
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
        arec = clnt.find_rec_mnem(ENTITY_ACCNT, self.mnem)
        if not arec:
            err_set(EINVAL, "no such account '{0}'".format(self.mnem))
            raise Error()
        return clnt.logon(arec)

class LogoffRequest(object):
    def __init__(self, mnem):
        self.mnem = mnem
    def __call__(self, clnt):
        arec = clnt.find_rec_mnem(ENTITY_ACCNT, self.mnem)
        if not arec:
            err_set(EINVAL, "no such account '{0}'".format(self.mnem))
            raise Error()
        return clnt.logoff(arec)

class AccntRequest(object):
    @staticmethod
    def to_dict(arec):
        return {
            'id': arec.id,
            'mnem': arec.mnem,
            'display': arec.display,
            'email': arec.email
        }
    def __init__(self, *mnems):
        self.mnems = set(mnems)
    def __call__(self, clnt):
        accnts = []
        if self.mnems:
            for mnem in self.mnems:
                arec = clnt.find_rec_mnem(ENTITY_ACCNT, mnem)
                if not arec:
                    err_set(EINVAL, "no such account '{0}'".format(mnem))
                    raise Error()
                accnts.append(AccntRequest.to_dict(arec))
        else:
            accnts = [AccntRequest.to_dict(arec)
                      for arec in clnt.list_rec(ENTITY_ACCNT)]
        return accnts

class ContrRequest(object):
    @staticmethod
    def to_dict(crec):
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
    def __init__(self, *mnems):
        self.mnems = set(mnems)
    def __call__(self, clnt):
        contrs = []
        if self.mnems:
            for mnem in self.mnems:
                crec = clnt.find_rec_mnem(ENTITY_CONTR, mnem)
                if not crec:
                    err_set(EINVAL, "no such contract '{0}'".format(mnem))
                    raise Error()
                contrs.append(ContrRequest.to_dict(crec))
        else:
            contrs = [ContrRequest.to_dict(crec)
                      for crec in clnt.list_rec(ENTITY_CONTR)]
        return contrs

class GroupRequest(object):
    @staticmethod
    def to_dict(memb):
        return {
            'uid': memb.uid,
            'gid': memb.gid
        }
    def __init__(self, mnem):
        self.mnem = mnem
    def __call__(self, clnt):
        membs = []
        arec = clnt.find_rec_mnem(ENTITY_ACCNT, self.mnem)
        if not arec:
            err_set(EINVAL, "no such account '{0}'".format(self.mnem))
            raise Error()
        accnt = clnt.accnt(arec)
        membs = [GroupRequest.to_dict(memb)
                 for memb in accnt.list_group()]
        return membs

class OrderRequest(object):
    @staticmethod
    def to_dict(order):
        return {
            'id': order.id
        }
    def __init__(self, mnem):
        self.mnem = mnem
    def __call__(self, clnt):
        orders = []
        arec = clnt.find_rec_mnem(ENTITY_ACCNT, self.mnem)
        if not arec:
            err_set(EINVAL, "no such account '{0}'".format(self.mnem))
            raise Error()
        accnt = clnt.accnt(arec)
        orders = [OrderRequest.to_dict(order)
                  for order in accnt.list_order()]
        return orders

class TradeRequest(object):
    @staticmethod
    def to_dict(exc):
        return {
            'id': exc.id
        }
    def __init__(self, mnem):
        self.mnem = mnem
    def __call__(self, clnt):
        trades = []
        arec = clnt.find_rec_mnem(ENTITY_ACCNT, self.mnem)
        if not arec:
            err_set(EINVAL, "no such account '{0}'".format(self.mnem))
            raise Error()
        accnt = clnt.accnt(arec)
        trades = [ExecRequest.to_dict(exc)
                  for exc in accnt.list_trade()]
        return trades

class PosnRequest(object):
    def __init__(self, mnem):
        self.mnem = mnem
    def __call__(self, clnt):
        return []

class ViewRequest(object):
    @staticmethod
    def level_dict(level):
        return {
            'ticks': level.ticks,
            'lots': level.lots,
            'count': level.count
        }
    @staticmethod
    def to_dict(view):
        return {
            'cid': view.cid,
            'settl_date': view.settl_date,
            'list_bid': [ViewRequest.level_dict(level) for level in view.list_bid],
            'list_ask': [ViewRequest.level_dict(level) for level in view.list_ask]
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
                            views.append(ViewRequest.to_dict(view))
            else:
                # Cids only.
                views = [ViewRequest.to_dict(view) for view in clnt.list_view()
                         if view.cid in cids]
        else:
            if self.settl_dates:
                # Settl_dates only.
                views = [ViewRequest.to_dict(view) for view in clnt.list_view()
                         if view.settl_date in self.settl_dates]
            else:
                # Neither cids nor settl_dates.
                views = [ViewRequest.to_dict(view) for view in clnt.list_view()]
        return views

class AsyncClnt(object):
    def __init__(self):
        uuid = uuid1()
        log_info('uuid: {0}'.format(uuid))
        self.ctx = ZmqCtx()
        self.thread = Thread(target = worker, args = (self.ctx, uuid))
        self.thread.start()
        self.async = Async(self.ctx, uuid)
        self.is_closed = False

    def close(self):
        if not self.is_closed:
            self.async.sendAndRecv(CloseRequest())
            log_info('joining')
            self.thread.join()
            self.is_closed = True

    def logon(self, mnem):
        self.async.sendAndRecv(LogonRequest(mnem))

    def logoff(self, mnem):
        self.async.sendAndRecv(LogoffRequest(mnem))

    def accnts(self, *mnems):
        print(self.async.sendAndRecv(AccntRequest(*mnems)))

    def groups(self, mnem):
        print(self.async.sendAndRecv(GroupRequest(mnem)))

    def contrs(self, *mnems):
        print(self.async.sendAndRecv(ContrRequest(*mnems)))

    def orders(self, mnem):
        print(self.async.sendAndRecv(OrderRequest(mnem)))

    def trades(self, mnem):
        print(self.async.sendAndRecv(TradeRequest(mnem)))

    def posns(self):
        log_info('posns')

    def view(self):
        log_info('view')

    def buy(self, lots, price):
        log_info('buy: {0},{1}'.format(lots, price))

    def sell(self, lots, price):
        log_info('sell: {0},{1}'.format(lots, price))

    def revise(self, id, lots):
        log_info('revise: {0},{1}'.format(id, lots))

    def cancel(self, *args):
        log_info('cancel: ' + ','.join(args))

    def ack_trades(self, *args):
        log_info('ack_trades: ' + ','.join(args))

    def echo(self, *args):
        log_info('echo: ' + ','.join(args))

    def env(self):
        log_info('env')

    def set(self, name, value):
        log_info('set: {0},{1}'.format(name, value))

    def unset(self, name):
        log_info('unset: {0}'.format(name))

    def quit(self):
        self.close()

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

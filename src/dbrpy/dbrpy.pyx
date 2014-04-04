from dbr cimport *

cimport string
cimport zmq

from cpython cimport Py_DECREF, Py_INCREF
from cpython.ref cimport PyObject

import uuid

cdef extern from "dbrpy/dbrpy.h":

    ctypedef DbrRbNode DbrpyRbNode
    ctypedef DbrSlNode DbrpySlNode

    ctypedef DbrRec DbrpyRec
    ctypedef DbrOrder DbrpyOrder
    ctypedef DbrExec DbrpyExec
    ctypedef DbrMemb DbrpyMemb
    ctypedef DbrPosn DbrpyPosn
    ctypedef DbrView DbrpyView

    ctypedef DbrHandlerVtbl DbrpyHandlerVtbl
    ctypedef DbrIHandler DbrpyIHandler

from inspect import currentframe, getframeinfo

# Defs

ERRMSG_MAX = DBR_ERRMSG_MAX

# Err

EINTR = DBR_EINTR
EIO = DBR_EIO
ENOMEM = DBR_ENOMEM
EACCES = DBR_EACCES
EBUSY = DBR_EBUSY
EEXIST = DBR_EEXIST
EINVAL = DBR_EINVAL
ETIMEOUT = DBR_ETIMEOUT
ESYSTEM = DBR_ESYSTEM
EUSER = DBR_EUSER

def err_clear():
    dbr_err_clear()

def err_perror(const char* s):
    dbr_err_perror(s)

def err_set(int num, const char* msg):
    fi = getframeinfo(currentframe())
    dbr_err_set_(num, fi.filename, fi.lineno, msg)

def err_print(int num, const char* msg):
    fi = getframeinfo(currentframe())
    dbr_err_print_(num, fi.filename, fi.lineno, msg)

def err_num():
    return dbr_err_num()

def err_file():
    return dbr_err_file()

def err_line():
    return dbr_err_line()

def err_msg():
    return dbr_err_msg()

class Error(Exception):
    def __init__(self):
        self.num = err_num()
        self.file = err_file()
        self.line = err_line()
        self.msg = err_msg()
    def __repr__(self):
        return 'Error({0.num!r}, {0.file!r}, {0.line!r}, {0.msg!r})'.format(self)
    def __str__(self):
        return "{1}:{2}: {3} ({0})".format(self.num, self.file, self.line, self.msg)

# Log

LOG_CRIT = DBR_LOG_CRIT
LOG_ERROR = DBR_LOG_ERROR
LOG_WARN = DBR_LOG_WARN
LOG_NOTICE = DBR_LOG_NOTICE
LOG_INFO = DBR_LOG_INFO
LOG_DEBUG1 = DBR_LOG_DEBUG1
LOG_DEBUG2 = DBR_LOG_DEBUG2
LOG_DEBUG3 = DBR_LOG_DEBUG3

def log_printf(int level, const char* msg):
    dbr_log_printf(level, msg)

def log_crit(const char* msg):
    dbr_log_crit(msg)

def log_error(const char* msg):
    dbr_log_error(msg)

def log_warn(const char* msg):
    dbr_log_warn(msg)

def log_notice(const char* msg):
    dbr_log_notice(msg)

def log_info(const char* msg):
    dbr_log_info(msg)

def log_debug1(const char* msg):
    dbr_log_debug1(msg)

def log_debug2(const char* msg):
    dbr_log_debug2(msg)

def log_debug3(const char* msg):
    dbr_log_debug3(msg)

# Util

def millis():
    return dbr_millis()

def intdig(int i):
    return dbr_intdig(i)

def longdig(long l):
    return dbr_longdig(l)

def intlen(int i):
    return dbr_intlen(i)

def longlen(long l):
    return dbr_longlen(l)

# Types

ENTITY_TRADER = DBR_ENTITY_TRADER
ENTITY_ACCNT = DBR_ENTITY_ACCNT
ENTITY_CONTR = DBR_ENTITY_CONTR
ENTITY_ORDER = DBR_ENTITY_ORDER
ENTITY_EXEC = DBR_ENTITY_EXEC
ENTITY_MEMB = DBR_ENTITY_MEMB
ENTITY_POSN = DBR_ENTITY_POSN

DISPLAY_MAX = DBR_DISPLAY_MAX
EMAIL_MAX = DBR_EMAIL_MAX
MNEM_MAX = DBR_MNEM_MAX
REF_MAX = DBR_REF_MAX

cdef class RecBase(object):
    # Only accesed from clnt thread.
    cdef DbrpyRec* impl_
    cdef public int type
    cdef public DbrIden id
    cdef public bytes mnem
    cdef public bytes display
    def __init__(self):
        raise TypeError("init called")

cdef class TraderRec(RecBase):
    cdef public bytes email
    def __init__(self):
        raise TypeError("init called")
    def __repr__(self):
        return 'TraderRec({0.type!r}, {0.id!r}, {0.mnem!r}, {0.display!r})'.format(self)

cdef class AccntRec(RecBase):
    cdef public bytes email
    def __init__(self):
        raise TypeError("init called")
    def __repr__(self):
        return 'AccntRec({0.type!r}, {0.id!r}, {0.mnem!r}, {0.display!r})'.format(self)

cdef class ContrRec(RecBase):
    cdef public bytes asset_type
    cdef public bytes asset
    cdef public bytes ccy
    cdef public int tick_numer
    cdef public int tick_denom
    cdef public double price_inc
    cdef public int lot_numer
    cdef public int lot_denom
    cdef public double qty_inc
    cdef public int price_dp
    cdef public int pip_dp
    cdef public int qty_dp
    cdef public DbrLots min_lots
    cdef public DbrLots max_lots
    def __init__(self):
        raise TypeError("init called")
    def __repr__(self):
        return 'ContrRec({0.type!r}, {0.id!r}, {0.mnem!r}, {0.display!r})'.format(self)

    def qty_to_lots(self, double qty):
        return dbr_real_to_incs(qty, self.qty_inc)

    def lots_to_qty(self, DbrLots lots):
        return dbr_incs_to_real(lots, self.qty_inc)

    def price_to_ticks(self, double price):
        return dbr_real_to_incs(price, self.price_inc)

    def ticks_to_price(self, DbrTicks ticks):
        return dbr_incs_to_real(ticks, self.price_inc)

cdef inline void set_rec_base(RecBase obj, DbrpyRec* rec):
    obj.impl_ = rec
    obj.type = rec.type
    obj.id = rec.id
    obj.mnem = rec.mnem[:string.strnlen(rec.mnem, DBR_MNEM_MAX)]
    obj.display = rec.display[:string.strnlen(rec.display, DBR_DISPLAY_MAX)]

cdef TraderRec make_trader_rec(DbrpyRec* rec):
    cdef obj = TraderRec.__new__(TraderRec)
    set_rec_base(obj, rec)
    obj.email = rec.trader.email[:string.strnlen(rec.trader.email, DBR_EMAIL_MAX)]
    return obj

cdef AccntRec make_accnt_rec(DbrpyRec* rec):
    cdef obj = AccntRec.__new__(AccntRec)
    set_rec_base(obj, rec)
    obj.email = rec.accnt.email[:string.strnlen(rec.accnt.email, DBR_EMAIL_MAX)]
    return obj

cdef ContrRec make_contr_rec(DbrpyRec* rec):
    cdef obj = ContrRec.__new__(ContrRec)
    set_rec_base(obj, rec)
    obj.asset_type = rec.contr.asset_type[:string.strnlen(rec.contr.asset_type, DBR_MNEM_MAX)]
    obj.asset = rec.contr.asset[:string.strnlen(rec.contr.asset, DBR_MNEM_MAX)]
    obj.ccy = rec.contr.ccy[:string.strnlen(rec.contr.ccy, DBR_MNEM_MAX)]
    obj.tick_numer = rec.contr.tick_numer
    obj.tick_denom = rec.contr.tick_denom
    obj.price_inc = rec.contr.price_inc
    obj.lot_numer = rec.contr.lot_numer
    obj.lot_denom = rec.contr.lot_denom
    obj.qty_inc = rec.contr.qty_inc
    obj.price_dp = rec.contr.price_dp
    obj.pip_dp = rec.contr.pip_dp
    obj.qty_dp = rec.contr.qty_dp
    obj.min_lots = rec.contr.min_lots
    obj.max_lots = rec.contr.max_lots
    return obj

cdef RecBase make_rec(DbrpyRec* rec):
    cdef RecBase obj = None
    if rec.type == DBR_ENTITY_TRADER:
        obj = make_trader_rec(rec)
    elif rec.type == DBR_ENTITY_ACCNT:
        obj = make_accnt_rec(rec)
    elif rec.type == DBR_ENTITY_CONTR:
        obj = make_contr_rec(rec)
    return obj

STATE_NEW = DBR_STATE_NEW
STATE_REVISE = DBR_STATE_REVISE
STATE_CANCEL = DBR_STATE_CANCEL
STATE_TRADE = DBR_STATE_TRADE

ACTION_BUY = DBR_ACTION_BUY
ACTION_SELL = DBR_ACTION_SELL

cdef class Order(object):
    cdef public DbrIden id
    cdef public DbrIden tid
    cdef public DbrIden aid
    cdef public DbrIden cid
    cdef public DbrDate settl_date
    cdef public bytes ref
    cdef public int state
    cdef public int action
    cdef public DbrTicks ticks
    cdef public DbrLots lots
    cdef public DbrLots resd
    cdef public DbrLots exc
    cdef public DbrTicks last_ticks
    cdef public DbrLots last_lots
    cdef public DbrLots min_lots
    cdef public DbrMillis created
    cdef public DbrMillis modified
    def __init__(self):
        raise TypeError("init called")
    def __repr__(self):
        return 'Order({0.id!r})'.format(self)

cdef Order make_order(DbrpyOrder* order):
    cdef obj = Order.__new__(Order)
    obj.id = order.id
    obj.tid = order.c.trader.rec.id
    obj.aid = order.c.accnt.rec.id
    obj.cid = order.c.contr.rec.id
    obj.settl_date = order.c.settl_date
    obj.ref = order.c.ref[:string.strnlen(order.c.ref, DBR_REF_MAX)]
    obj.state = order.c.state
    obj.action = order.c.action
    obj.ticks = order.c.ticks
    obj.lots = order.c.lots
    obj.resd = order.c.resd
    obj.exc = order.c.exc
    obj.last_ticks = order.c.last_ticks
    obj.last_lots = order.c.last_lots
    obj.min_lots = order.c.min_lots
    obj.created = order.created
    obj.modified = order.modified
    return obj

def order_done(Order order):
    return order.resd == 0

cdef class Level(object):
    cdef public DbrTicks ticks
    cdef public DbrLots lots
    cdef public size_t count
    def __cinit__(self, DbrTicks ticks, DbrLots lots, size_t count):
        self.ticks = ticks
        self.lots = lots
        self.count = count
    def __repr__(self):
        return 'Level({0.ticks!r}, {0.lots!r}, {0.count!r})'.format(self)

ROLE_MAKER = DBR_ROLE_MAKER
ROLE_TAKER = DBR_ROLE_TAKER

cdef class Exec(object):
    cdef public DbrIden id
    cdef public DbrIden order
    cdef public DbrIden tid
    cdef public DbrIden aid
    cdef public DbrIden cid
    cdef public DbrDate settl_date
    cdef public bytes ref
    cdef public int state
    cdef public int action
    cdef public DbrTicks ticks
    cdef public DbrLots lots
    cdef public DbrLots resd
    cdef public DbrLots exc
    cdef public DbrTicks last_ticks
    cdef public DbrLots last_lots
    cdef public DbrLots min_lots
    cdef public DbrIden match
    cdef public int role
    cdef public DbrIden cpty
    cdef public DbrMillis created
    def __init__(self):
        raise TypeError("init called")
    def __repr__(self):
        return 'Exec({0.id!r})'.format(self)

cdef Exec make_exec(DbrpyExec* exc):
    cdef obj = Exec.__new__(Exec)
    obj.id = exc.id
    obj.order = exc.order
    obj.tid = exc.c.trader.rec.id
    obj.aid = exc.c.accnt.rec.id
    obj.cid = exc.c.contr.rec.id
    obj.settl_date = exc.c.settl_date
    obj.ref = exc.c.ref[:string.strnlen(exc.c.ref, DBR_REF_MAX)]
    obj.state = exc.c.state
    obj.action = exc.c.action
    obj.ticks = exc.c.ticks
    obj.lots = exc.c.lots
    obj.resd = exc.c.resd
    obj.exc = exc.c.exc
    obj.last_ticks = exc.c.last_ticks
    obj.last_lots = exc.c.last_lots
    obj.min_lots = exc.c.min_lots
    obj.match = exc.match
    obj.role = exc.role
    obj.cpty = exc.cpty.rec.id if exc.cpty.rec is not NULL else 0
    obj.created = exc.created
    return obj

def exec_done(Exec exc):
    return exc.resd == 0

cdef class Memb(object):
    cdef public DbrIden tid
    cdef public DbrIden aid
    def __init__(self):
        raise TypeError("init called")
    def __repr__(self):
        return 'Memb({0.tid!r}, {0.aid!r})'.format(self)

cdef Memb make_memb(DbrpyMemb* memb):
    cdef obj = Memb.__new__(Memb)
    obj.tid = memb.trader.rec.id
    obj.aid = memb.accnt.rec.id
    return obj

cdef class Posn(object):
    cdef public DbrIden aid
    cdef public DbrIden cid
    cdef public DbrDate settl_date
    cdef public DbrLicks buy_licks
    cdef public DbrLots buy_lots
    cdef public DbrLicks sell_licks
    cdef public DbrLots sell_lots
    def __init__(self):
        raise TypeError("init called")
    def __repr__(self):
        return 'Posn({0.aid!r}, {0.cid!r}, {0.settl_date!r})'.format(self)

cdef Posn make_posn(DbrpyPosn* posn):
    cdef obj = Posn.__new__(Posn)
    obj.aid = posn.accnt.rec.id
    obj.cid = posn.contr.rec.id
    obj.settl_date = posn.settl_date
    obj.buy_licks = posn.buy_licks
    obj.buy_lots = posn.buy_lots
    obj.sell_licks = posn.sell_licks
    obj.sell_lots = posn.sell_lots
    return obj

LEVEL_MAX = DBR_LEVEL_MAX

cdef class View(object):
    cdef public DbrIden cid
    cdef public DbrDate settl_date
    cdef public list list_bid
    cdef public list list_ask
    def __init__(self):
        raise TypeError("init called")
    def __repr__(self):
        return 'View({0.cid!r}, {0.settl_date!r}, {0.list_bid[0]!r}, {0.list_ask[0]!r})'.format(self)

cdef View make_view(DbrpyView* view):
    cdef obj = View.__new__(View)
    obj.cid = view.contr.rec.id
    obj.settl_date = view.settl_date
    obj.list_bid = []
    obj.list_ask = []
    cdef size_t i
    for i in range(LEVEL_MAX):
        obj.list_bid.append(Level(view.bid_ticks[i], view.bid_lots[i], view.bid_count[i]))
    for i in range(LEVEL_MAX):
        obj.list_ask.append(Level(view.ask_ticks[i], view.ask_lots[i], view.ask_count[i]))
    return obj

# Pool

cdef class Pool(object):
    cdef DbrPool impl_

    def __cinit__(self, size_t capacity):
        self.impl_ = dbr_pool_create(capacity)
        if self.impl_ is NULL:
            raise Error()

    def __dealloc__(self):
        if self.impl_ is not NULL:
            dbr_pool_destroy(self.impl_)

# Conv

def fract_to_real(int numer, int denom):
    return dbr_fract_to_real(numer, denom)

def real_to_incs(double real, double inc_size):
    return dbr_real_to_incs(real, inc_size)

def incs_to_real(DbrIncs incs, double inc_size):
    return dbr_incs_to_real(incs, inc_size)

def qty_to_lots(double qty, ContrRec crec):
    return dbr_qty_to_lots(qty, crec.impl_)

def lots_to_qty(DbrLots lots, ContrRec crec):
    return dbr_lots_to_qty(lots, crec.impl_)

def price_to_ticks(double price, ContrRec crec):
    return dbr_price_to_ticks(price, crec.impl_)

def ticks_to_price(DbrTicks ticks, ContrRec crec):
    return dbr_ticks_to_price(ticks, crec.impl_)

def real_to_dp(double d):
    return dbr_real_to_dp(d)

def dp_to_real(int dp):
    return dbr_dp_to_real(dp)

# Async

cdef object async_recv(DbrAsync async):
    cdef void* val = NULL
    cdef DbrBool ret
    with nogil:
        ret = dbr_async_recv(async, &val)
    if ret == DBR_FALSE:
        raise Error()
    return <object>val

cdef class Async(object):
    cdef ZmqCtx ctx_
    cdef DbrAsync impl_

    def __cinit__(self, ZmqCtx ctx, uuid):
        self.ctx_ = ctx # Incref.
        self.impl_ = dbr_async_create(ctx.impl_, uuid.bytes)
        if self.impl_ is NULL:
            raise Error()

    def __dealloc__(self):
        if self.impl_ is not NULL:
            dbr_log_info('destroying async')
            dbr_async_destroy(self.impl_)

    def send(self, object val):
        Py_INCREF(val)
        cdef DbrBool ret
        with nogil:
            ret = dbr_async_send(self.impl_, <PyObject*>val)
        if ret == DBR_FALSE:
            Py_DECREF(val)
            raise Error()

    def recv(self):
        cdef object val = async_recv(self.impl_)
        Py_DECREF(val)
        return val

# Trader

cdef class Trader(object):
    cdef DbrTrader impl_
    cdef public TraderRec rec

    def __init__(self):
        raise TypeError("init called")

    # TraderOrder
    def find_order_id(self, DbrIden id):
        cdef DbrpyRbNode* node = dbr_trader_find_order_id(self.impl_, id)
        return make_order(dbr_trader_order_entry(node)) if node is not NULL else None
    def find_order_ref(self, const char* ref):
        cdef DbrpyOrder* order = dbr_trader_find_order_ref(self.impl_, ref)
        return make_order(order) if order is not NULL else None
    def list_order(self):
        orders = []
        cdef DbrpyRbNode* node = dbr_trader_first_order(self.impl_)
        while node is not NULL:
            orders.append(make_order(dbr_trader_order_entry(node)))
            node = dbr_rbnode_next(node)
        return orders
    def empty_order(self):
        return <bint>dbr_trader_empty_order(self.impl_)

    # TraderTrade
    def find_trade_id(self, DbrIden id):
        cdef DbrpyRbNode* node = dbr_trader_find_trade_id(self.impl_, id)
        return make_exec(dbr_trader_trade_entry(node)) if node is not NULL else None
    def list_trade(self):
        trades = []
        cdef DbrpyRbNode* node = dbr_trader_first_trade(self.impl_)
        while node is not NULL:
            trades.append(make_exec(dbr_trader_trade_entry(node)))
            node = dbr_rbnode_next(node)
        return trades
    def empty_trade(self):
        return <bint>dbr_trader_empty_trade(self.impl_)

    # TraderMemb
    def find_memb_id(self, DbrIden id):
        cdef DbrpyRbNode* node = dbr_trader_find_memb_id(self.impl_, id)
        return make_memb(dbr_trader_memb_entry(node)) if node is not NULL else None
    def list_memb(self):
        membs = []
        cdef DbrpyRbNode* node = dbr_trader_first_memb(self.impl_)
        while node is not NULL:
            membs.append(make_memb(dbr_trader_memb_entry(node)))
            node = dbr_rbnode_next(node)
        return membs
    def empty_memb(self):
        return <bint>dbr_trader_empty_memb(self.impl_)
    def logged_on(self):
        return <bint>dbr_trader_logged_on(self.impl_)

cdef Trader make_trader(DbrTrader trader, TraderRec rec):
    cdef Trader obj = Trader.__new__(Trader)
    obj.impl_ = trader
    obj.rec = rec
    return obj

# Accnt

cdef class Accnt(object):
    cdef DbrAccnt impl_
    cdef public AccntRec rec

    def __init__(self):
        raise TypeError("init called")

    # AccntMemb
    def find_memb_id(self, DbrIden id):
        cdef DbrpyRbNode* node = dbr_accnt_find_memb_id(self.impl_, id)
        return make_memb(dbr_accnt_memb_entry(node)) if node is not NULL else None
    def list_memb(self):
        membs = []
        cdef DbrpyRbNode* node = dbr_accnt_first_memb(self.impl_)
        while node is not NULL:
            membs.append(make_memb(dbr_accnt_memb_entry(node)))
            node = dbr_rbnode_next(node)
        return membs
    def empty_memb(self):
        return <bint>dbr_accnt_empty_memb(self.impl_)

    # AccntPosn
    def find_posn_id(self, DbrIden id):
        cdef DbrpyRbNode* node = dbr_accnt_find_posn_id(self.impl_, id)
        return make_posn(dbr_accnt_posn_entry(node)) if node is not NULL else None
    def list_posn(self):
        posns = []
        cdef DbrpyRbNode* node = dbr_accnt_first_posn(self.impl_)
        while node is not NULL:
            posns.append(make_posn(dbr_accnt_posn_entry(node)))
            node = dbr_rbnode_next(node)
        return posns
    def empty_posn(self):
        return <bint>dbr_accnt_empty_posn(self.impl_)

cdef Accnt make_accnt(DbrAccnt accnt, AccntRec rec):
    cdef Accnt obj = Accnt.__new__(Accnt)
    obj.impl_ = accnt
    obj.rec = rec
    return obj

# Handler

cdef struct HandlerImpl:
    PyObject* target
    DbrpyIHandler handler

cdef inline void* handler_target(DbrHandler handler) nogil:
    cdef size_t offset = <size_t>&(<HandlerImpl*>NULL).handler
    cdef HandlerImpl* impl = <HandlerImpl*>(<char*>handler - offset)
    return impl.target

cdef void on_close(DbrHandler handler) with gil:
    (<object>handler_target(handler)).on_close()

cdef void on_ready(DbrHandler handler) with gil:
    (<object>handler_target(handler)).on_ready()

cdef void on_logon(DbrHandler handler, DbrIden tid) with gil:
    (<object>handler_target(handler)).on_logon(tid)

cdef void on_logoff(DbrHandler handler, DbrIden tid) with gil:
    (<object>handler_target(handler)).on_logoff(tid)

cdef void on_reset(DbrHandler handler) with gil:
    (<object>handler_target(handler)).on_reset()

cdef void on_timeout(DbrHandler handler, DbrIden req_id) with gil:
    (<object>handler_target(handler)).on_timeout(req_id)

cdef void on_status(DbrHandler handler, DbrIden req_id, int num,
                    const char* msg) with gil:
    (<object>handler_target(handler)).on_status(req_id, num, msg)

cdef void on_exec(DbrHandler handler, DbrIden req_id, DbrpyExec* exc) with gil:
    (<object>handler_target(handler)).on_exec(req_id, make_exec(exc))

cdef void on_posn(DbrHandler handler, DbrpyPosn* posn) with gil:
    (<object>handler_target(handler)).on_posn(make_posn(posn))

cdef void on_view(DbrHandler handler, DbrpyView* view) with gil:
    (<object>handler_target(handler)).on_view(make_view(view))

cdef void on_flush(DbrHandler handler) with gil:
    (<object>handler_target(handler)).on_flush()

cdef void* on_async(DbrHandler handler, void* arg) with gil:
    cdef object ret = (<object>handler_target(handler)).on_async(<object>arg)
    Py_INCREF(ret)
    Py_DECREF(<object>arg)
    return <void*>ret

cdef class Handler(object):
    cdef DbrpyHandlerVtbl vtbl_
    cdef HandlerImpl impl_

    def __init__(self):
        self.vtbl_.on_close = on_close
        self.vtbl_.on_ready = on_ready
        self.vtbl_.on_logon = on_logon
        self.vtbl_.on_logoff = on_logoff
        self.vtbl_.on_reset = on_reset
        self.vtbl_.on_timeout = on_timeout
        self.vtbl_.on_status = on_status
        self.vtbl_.on_exec = on_exec
        self.vtbl_.on_posn = on_posn
        self.vtbl_.on_view = on_view
        self.vtbl_.on_flush = on_flush
        self.vtbl_.on_async = on_async
        self.impl_.target = <PyObject*>self
        self.impl_.handler.vtbl = &self.vtbl_

    def on_close(self):
        pass

    def on_ready(self):
        pass

    def on_logon(self, tid):
        pass

    def on_logoff(self, tid):
        pass

    def on_reset(self):
        pass

    def on_timeout(self, req_id):
        pass

    def on_status(self, req_id, num, msg):
        pass

    def on_exec(self, req_id, exc):
        pass

    def on_posn(self, posn):
        pass

    def on_view(self, view):
        pass

    def on_flush(self):
        pass

    def on_async(self, arg):
        return arg

# Zmq

cdef class ZmqCtx(object):
    cdef void* impl_

    def __cinit__(self):
        self.impl_ = zmq.zmq_ctx_new()
        if self.impl_ is NULL:
            fi = getframeinfo(currentframe())
            dbr_err_setf_(DBR_EIO, fi.filename, fi.lineno,
                              "zmq_ctx_new() failed: %s", zmq.zmq_strerror(zmq.zmq_errno()))
            raise Error()

    def __dealloc__(self):
        if self.impl_ is not NULL:
            dbr_log_info('destroying ctx')
            zmq.zmq_ctx_destroy(self.impl_)

# Clnt

cdef class Clnt(object):
    cdef ZmqCtx ctx_
    cdef Pool pool_
    cdef DbrClnt impl_

    def __cinit__(self, ZmqCtx ctx, uuid, const char* mdaddr, const char* traddr,
                  DbrIden seed, DbrMillis tmout, Pool pool):
        self.ctx_ = ctx   # Incref.
        self.pool_ = pool # Incref.
        self.impl_ = dbr_clnt_create(ctx.impl_, uuid.bytes, mdaddr, traddr, seed, tmout, pool.impl_)
        if self.impl_ is NULL:
            raise Error()

    def __dealloc__(self):
        if self.impl_ is not NULL:
            dbr_log_info('destroying clnt')
            dbr_clnt_destroy(self.impl_)

    def reset(self):
        dbr_clnt_reset(self.impl_)

    def close(self):
        cdef DbrIden id = dbr_clnt_close(self.impl_)
        if id < 0:
            raise Error()
        return id

    def find_rec_id(self, int type, DbrIden id):
        cdef DbrpySlNode* node = dbr_clnt_find_rec_id(self.impl_, type, id)
        return make_rec(dbr_shared_rec_entry(node)) if node is not NULL else None

    def find_rec_mnem(self, int type, const char* mnem):
        cdef DbrpySlNode* node = dbr_clnt_find_rec_mnem(self.impl_, type, mnem)
        return make_rec(dbr_shared_rec_entry(node)) if node is not NULL else None

    def list_rec(self, int type):
        recs = []
        cdef size_t size = 0
        cdef DbrpySlNode* node = dbr_clnt_first_rec(self.impl_, type, &size)
        while node is not NULL:
            recs.append(make_rec(dbr_shared_rec_entry(node)))
            node = dbr_slnode_next(node)
        return recs

    def empty_rec(self, int type):
        return <bint>dbr_clnt_empty_rec(self.impl_, type)

    def trader(self, TraderRec trec):
        cdef DbrTrader trader = dbr_clnt_trader(self.impl_, trec.impl_)
        if trader is NULL:
            raise Error()
        return make_trader(trader, trec)

    def accnt(self, AccntRec arec):
        cdef DbrAccnt accnt = dbr_clnt_accnt(self.impl_, arec.impl_)
        if accnt is NULL:
            raise Error()
        return make_accnt(accnt, arec)

    def logon(self, TraderRec trec):
        cdef DbrTrader trader = dbr_clnt_trader(self.impl_, trec.impl_)
        if trader is NULL:
            raise Error()
        cdef DbrIden id
        with nogil:
            id = dbr_clnt_logon(self.impl_, trader)
        if id < 0:
            raise Error()
        return id

    def logoff(self, TraderRec trec):
        cdef DbrTrader trader = dbr_clnt_trader(self.impl_, trec.impl_)
        if trader is NULL:
            raise Error()
        cdef DbrIden id
        with nogil:
            id = dbr_clnt_logoff(self.impl_, trader)
        if id < 0:
            raise Error()
        return id

    def place(self, Trader trader, Accnt accnt, ContrRec crec, DbrDate settl_date,
              const char* ref, int action, DbrTicks ticks, DbrLots lots, DbrLots min_lots):
        cdef DbrIden id
        with nogil:
            id = dbr_clnt_place(self.impl_, trader.impl_, accnt.impl_, crec.impl_,
                                settl_date, ref, action, ticks, lots, min_lots)
        if id < 0:
            raise Error()
        return id

    def revise_id(self, Trader trader, DbrIden id, DbrLots lots):
        with nogil:
            id = dbr_clnt_revise_id(self.impl_, trader.impl_, id, lots)
        if id < 0:
            raise Error()
        return id

    def revise_ref(self, Trader trader, const char* ref, DbrLots lots):
        cdef DbrIden id
        with nogil:
            id = dbr_clnt_revise_ref(self.impl_, trader.impl_, ref, lots)
        if id < 0:
            raise Error()
        return id

    def cancel_id(self, Trader trader, DbrIden id):
        with nogil:
            id = dbr_clnt_cancel_id(self.impl_, trader.impl_, id)
        if id < 0:
            raise Error()
        return id

    def cancel_ref(self, Trader trader, const char* ref):
        cdef DbrIden id
        with nogil:
            id = dbr_clnt_cancel_ref(self.impl_, trader.impl_, ref)
        if id < 0:
            raise Error()
        return id

    def ack_trade(self, Trader trader, DbrIden id):
        with nogil:
            id = dbr_clnt_ack_trade(self.impl_, trader.impl_, id)
        if id < 0:
            raise Error()
        return id

    def is_ready(self):
        return <bint>dbr_clnt_is_ready(self.impl_)

    def settimer(self, DbrMillis absms):
        cdef DbrIden id = dbr_clnt_settimer(self.impl_, absms)
        if id < 0:
            raise Error()
        return id

    def canceltimer(self, DbrIden id):
        dbr_clnt_canceltimer(self.impl_, id)

    def dispatch(self, DbrMillis ms, Handler handler):
        cdef DbrBool ret
        with nogil:
            ret = dbr_clnt_dispatch(self.impl_, ms, &handler.impl_.handler)
        if ret < 0:
            raise Error()
        return <bint>ret;

    def mdclear(self):
        dbr_clnt_mdclear(self.impl_)

    def trclear(self):
        dbr_clnt_trclear(self.impl_)

    def clear(self):
        dbr_clnt_clear(self.impl_)

    def find_view(self, DbrIden cid, DbrDate settl_date):
        cdef DbrpyRbNode* node = dbr_clnt_find_view(self.impl_, cid, settl_date)
        return make_view(dbr_clnt_view_entry(node)) if node is not NULL else None
    def list_view(self):
        views = []
        cdef DbrpyRbNode* node = dbr_clnt_first_view(self.impl_)
        while node is not NULL:
            views.append(make_view(dbr_clnt_view_entry(node)))
            node = dbr_rbnode_next(node)
        return views
    def empty_view(self):
        return <bint>dbr_clnt_empty_view(self.impl_)

    def uuid(self):
        return uuid.UUID(bytes = dbr_clnt_uuid(self.impl_))

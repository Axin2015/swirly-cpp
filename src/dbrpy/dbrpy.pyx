from dbr cimport *
cimport zmq
cimport string

from cpython.ref cimport PyObject

cdef extern from "dbrpy/dbrpy.h":

    ctypedef DbrSlNode DbrpySlNode

    ctypedef DbrRec  DbrpyRec
    ctypedef DbrExec DbrpyExec
    ctypedef DbrPosn DbrpyPosn
    ctypedef DbrView DbrpyView

    ctypedef DbrHandlerVtbl DbrpyHandlerVtbl
    ctypedef DbrIHandler DbrpyIHandler

from inspect import currentframe, getframeinfo

EINTR = DBR_EINTR
EIO = DBR_EIO
ENOMEM = DBR_ENOMEM
EACCES = DBR_EACCES
EBUSY = DBR_EBUSY
EEXIST = DBR_EEXIST
EINVAL = DBR_EINVAL
ETIMEOUT = DBR_ETIMEOUT
EUSER = DBR_EUSER

def err_set(int num, const char* msg):
    fi = getframeinfo(currentframe())
    dbr_err_set_(num, fi.filename, fi.lineno, msg)

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

def millis():
    return dbr_millis()

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
            zmq.zmq_ctx_destroy(self.impl_)

ENTITY_TRADER = DBR_ENTITY_TRADER
ENTITY_ACCNT = DBR_ENTITY_ACCNT
ENTITY_CONTR = DBR_ENTITY_CONTR
ENTITY_ORDER = DBR_ENTITY_ORDER
ENTITY_EXEC = DBR_ENTITY_EXEC
ENTITY_MEMB = DBR_ENTITY_MEMB
ENTITY_POSN = DBR_ENTITY_POSN

cdef class RecBase(object):
    # Only accesed from clnt thread.
    cdef DbrpyRec* impl_
    cdef public int type
    cdef public DbrIden id
    cdef public bytes mnem
    cdef public bytes display
    def __init__(self):
        raise TypeError("init called on RecBase")

cdef class TraderRec(RecBase):
    cdef public bytes email
    def __init__(self):
        raise TypeError("init called on TraderRec")
    def __repr__(self):
        return 'TraderRec({0.type!r}, {0.id!r}, {0.mnem!r}, {0.display!r})'.format(self)

cdef class AccntRec(RecBase):
    cdef public bytes email
    def __init__(self):
        raise TypeError("init called on AccntRec")
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
        raise TypeError("init called on ContrRec")
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

ACTION_BUY = DBR_ACTION_BUY
ACTION_SELL = DBR_ACTION_SELL

cdef class Exec(object):
    def __init__(self):
        raise TypeError("init called on Exec")

cdef Exec make_exec(DbrpyExec* exc):
    cdef obj = Exec.__new__(Exec)
    obj.id = exc.id
    obj.ref = exc.c.ref[:string.strnlen(exc.c.ref, DBR_REF_MAX)]
    return obj

cdef class Posn(object):
    def __init__(self):
        raise TypeError("init called on Posn")

cdef Posn make_posn(DbrpyPosn* posn):
    cdef obj = Posn.__new__(Posn)
    return obj

cdef class View(object):
    def __init__(self):
        raise TypeError("init called on View")

cdef View make_view(DbrpyView* view):
    cdef obj = View.__new__(View)
    #intst.cid = view.contr.rec.id
    return obj

cdef class Pool(object):
    cdef DbrPool impl_

    def __cinit__(self, capacity):
        self.impl_ = dbr_pool_create(capacity)
        if self.impl_ is NULL:
            raise Error()

    def __dealloc__(self):
        if self.impl_ is not NULL:
            dbr_pool_destroy(self.impl_)

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

cdef struct HandlerImpl:
    PyObject* target
    DbrpyIHandler handler

cdef inline void* handler_target(DbrHandler handler):
    cdef size_t offset = <size_t>&(<HandlerImpl*>NULL).handler
    cdef HandlerImpl* impl = <HandlerImpl*>(<char*>handler - offset)
    return impl.target

cdef void on_up(DbrHandler handler, int conn):
    (<object>handler_target(handler)).on_up(conn)

cdef void on_down(DbrHandler handler, int conn):
    (<object>handler_target(handler)).on_down(conn)

cdef void on_logon(DbrHandler handler, DbrIden tid):
    (<object>handler_target(handler)).on_logon(tid)

cdef void on_logoff(DbrHandler handler, DbrIden tid):
    (<object>handler_target(handler)).on_logoff(tid)

cdef void on_timeout(DbrHandler handler, DbrIden req_id):
    (<object>handler_target(handler)).on_timeout(req_id)

cdef void on_status(DbrHandler handler, DbrIden req_id, int num, const char* msg):
    (<object>handler_target(handler)).on_status(req_id, num, msg)

cdef void on_exec(DbrHandler handler, DbrIden req_id, DbrpyExec* exc):
    (<object>handler_target(handler)).on_exec(req_id, make_exec(exc))

cdef void on_posn(DbrHandler handler, DbrpyPosn* posn):
    (<object>handler_target(handler)).on_posn(make_posn(posn))

cdef void on_view(DbrHandler handler, DbrpyView* view):
    (<object>handler_target(handler)).on_view(make_view(view))

cdef void on_flush(DbrHandler handler):
    (<object>handler_target(handler)).on_flush()

cdef class Handler(object):
    cdef DbrpyHandlerVtbl vtbl_
    cdef HandlerImpl impl_

    def __init__(self):
        self.vtbl_.on_up = on_up
        self.vtbl_.on_down = on_down
        self.vtbl_.on_logon = on_logon
        self.vtbl_.on_logoff = on_logoff
        self.vtbl_.on_timeout = on_timeout
        self.vtbl_.on_status = on_status
        self.vtbl_.on_exec = on_exec
        self.vtbl_.on_posn = on_posn
        self.vtbl_.on_view = on_view
        self.vtbl_.on_flush = on_flush
        self.impl_.target = <PyObject*>self
        self.impl_.handler.vtbl = &self.vtbl_

    def on_up(self, conn):
        pass

    def on_down(self, conn):
        pass

    def on_logon(self, tid):
        pass

    def on_logoff(self, tid):
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

cdef class Clnt(object):
    cdef DbrClnt impl_

    def __cinit__(self, const char* sess, ZmqCtx ctx, const char* mdaddr,
                  const char* traddr, DbrIden seed, Pool pool):
        self.impl_ = dbr_clnt_create(sess, ctx.impl_, mdaddr, traddr, seed, pool.impl_)
        if self.impl_ is NULL:
            raise Error()

    def __dealloc__(self):
        if self.impl_ is not NULL:
            dbr_clnt_destroy(self.impl_)

    def close(self, DbrMillis ms):
        cdef DbrIden id = dbr_clnt_close(self.impl_, ms)
        if id < 0:
            raise Error()
        return id

    def find_rec_id(self, int type, DbrIden id):
        cdef DbrpySlNode* node = dbr_clnt_find_rec_id(self.impl_, type, id)
        return make_rec(dbr_shared_rec_entry(node)) if node is not NULL else None

    def find_rec_mnem(self, int type, const char* mnem):
        cdef DbrpySlNode* node = dbr_clnt_find_rec_mnem(self.impl_, type, mnem)
        return make_rec(dbr_shared_rec_entry(node)) if node is not NULL else None

    def recs(self, int type):
        cdef size_t size = 0
        cdef DbrpySlNode* node = dbr_clnt_first_rec(self.impl_, type, &size)
        ls = []
        while node is not NULL:
            ls.append(make_rec(dbr_shared_rec_entry(node)))
            node = dbr_slnode_next(node)
        return ls

    def logon(self, TraderRec trec, DbrMillis ms):
        cdef DbrTrader trader = dbr_clnt_trader(self.impl_, trec.impl_)
        if trader is NULL:
            raise Error()
        cdef DbrIden id = dbr_clnt_logon(self.impl_, trader, ms)
        if id < 0:
            raise Error()
        return id

    def logoff(self, TraderRec trec, DbrMillis ms):
        cdef DbrTrader trader = dbr_clnt_trader(self.impl_, trec.impl_)
        if trader is NULL:
            raise Error()
        cdef DbrIden id = dbr_clnt_logoff(self.impl_, trader, ms)
        if id < 0:
            raise Error()
        return id

    def place(self, TraderRec trec, AccntRec arec, ContrRec crec,
              DbrDate settl_date, const char* ref, int action, DbrTicks ticks,
              DbrLots lots, DbrLots min_lots, DbrMillis ms):
        cdef DbrTrader trader = dbr_clnt_trader(self.impl_, trec.impl_)
        if trader is NULL:
            raise Error()
        cdef DbrAccnt accnt = dbr_clnt_accnt(self.impl_, arec.impl_)
        if accnt is NULL:
            raise Error()
        cdef DbrIden id = dbr_clnt_place(self.impl_, trader, accnt, crec.impl_,
                                         settl_date, ref, action, ticks, lots,
                                         min_lots, ms)
        if id < 0:
            raise Error()
        return id

    def is_open(self):
        return <bint>dbr_clnt_is_open(self.impl_)

    def is_ready(self):
        return <bint>dbr_clnt_is_ready(self.impl_)

    def poll(self, DbrMillis ms, Handler handler):
        return dbr_clnt_poll(self.impl_, ms, &handler.impl_.handler)

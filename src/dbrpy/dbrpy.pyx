cimport dbr
cimport zmq
cimport string

from cpython.ref cimport PyObject

cdef extern from "dbrpy/dbrpy.h":

    ctypedef dbr.DbrSlNode DbrpySlNode

    ctypedef dbr.DbrRec  DbrpyRec
    ctypedef dbr.DbrExec DbrpyExec
    ctypedef dbr.DbrPosn DbrpyPosn
    ctypedef dbr.DbrView DbrpyView

    ctypedef dbr.DbrHandlerVtbl DbrpyHandlerVtbl
    ctypedef dbr.DbrIHandler DbrpyIHandler

from inspect import currentframe, getframeinfo

EINTR = dbr.DBR_EINTR
EIO = dbr.DBR_EIO
ENOMEM = dbr.DBR_ENOMEM
EACCES = dbr.DBR_EACCES
EBUSY = dbr.DBR_EBUSY
EEXIST = dbr.DBR_EEXIST
EINVAL = dbr.DBR_EINVAL
ETIMEOUT = dbr.DBR_ETIMEOUT
EUSER = dbr.DBR_EUSER

def err_set(int num, const char* msg):
    fi = getframeinfo(currentframe())
    dbr.dbr_err_set_(num, fi.filename, fi.lineno, msg)

def err_num():
    return dbr.dbr_err_num()

def err_file():
    return dbr.dbr_err_file()

def err_line():
    return dbr.dbr_err_line()

def err_msg():
    return dbr.dbr_err_msg()

class Error(Exception):
    def __init__(self):
        self.num = err_num()
        self.file = err_file()
        self.line = err_line()
        self.msg = err_msg()
    def __str__(self):
        return "{1}:{2}: {3} ({0})".format(self.num, self.file, self.line, self.msg)

def millis():
    return dbr.dbr_millis()

cdef class ZmqCtx(object):
    cdef void* impl_

    def __cinit__(self):
        self.impl_ = zmq.zmq_ctx_new()
        if self.impl_ is NULL:
            fi = getframeinfo(currentframe())
            dbr.dbr_err_setf_(dbr.DBR_EIO, fi.filename, fi.lineno,
                              "zmq_ctx_new() failed: %s", zmq.zmq_strerror(zmq.zmq_errno()))
            raise Error()

    def __dealloc__(self):
        if self.impl_ is not NULL:
            zmq.zmq_ctx_destroy(self.impl_)

ENTITY_TRADER = dbr.DBR_ENTITY_TRADER
ENTITY_ACCNT = dbr.DBR_ENTITY_ACCNT
ENTITY_CONTR = dbr.DBR_ENTITY_CONTR
ENTITY_ORDER = dbr.DBR_ENTITY_ORDER
ENTITY_EXEC = dbr.DBR_ENTITY_EXEC
ENTITY_MEMB = dbr.DBR_ENTITY_MEMB
ENTITY_POSN = dbr.DBR_ENTITY_POSN

cdef class RecBase(object):
    cdef public int type
    cdef public dbr.DbrIden id
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
    cdef public dbr.DbrLots min_lots
    cdef public dbr.DbrLots max_lots
    def __init__(self):
        raise TypeError("init called on ContrRec")
    def __repr__(self):
        return 'ContrRec({0.type!r}, {0.id!r}, {0.mnem!r}, {0.display!r})'.format(self)

cdef inline void set_rec_base(RecBase obj, DbrpyRec* rec):
    obj.type = rec.type
    obj.id = rec.id
    obj.mnem = rec.mnem[:string.strnlen(rec.mnem, dbr.DBR_MNEM_MAX)]
    obj.display = rec.display[:string.strnlen(rec.display, dbr.DBR_DISPLAY_MAX)]

cdef TraderRec make_trader_rec(DbrpyRec* rec):
    cdef obj = TraderRec.__new__(TraderRec)
    set_rec_base(obj, rec)
    obj.email = rec.trader.email[:string.strnlen(rec.trader.email, dbr.DBR_EMAIL_MAX)]
    return obj

cdef AccntRec make_accnt_rec(DbrpyRec* rec):
    cdef obj = AccntRec.__new__(AccntRec)
    set_rec_base(obj, rec)
    obj.email = rec.accnt.email[:string.strnlen(rec.accnt.email, dbr.DBR_EMAIL_MAX)]
    return obj

cdef ContrRec make_contr_rec(DbrpyRec* rec):
    cdef obj = ContrRec.__new__(ContrRec)
    set_rec_base(obj, rec)
    obj.asset_type = rec.contr.asset_type[:string.strnlen(rec.contr.asset_type, dbr.DBR_MNEM_MAX)]
    obj.asset = rec.contr.asset[:string.strnlen(rec.contr.asset, dbr.DBR_MNEM_MAX)]
    obj.ccy = rec.contr.ccy[:string.strnlen(rec.contr.ccy, dbr.DBR_MNEM_MAX)]
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
    if rec.type == dbr.DBR_ENTITY_TRADER:
        obj = make_trader_rec(rec)
    elif rec.type == dbr.DBR_ENTITY_ACCNT:
        obj = make_accnt_rec(rec)
    elif rec.type == dbr.DBR_ENTITY_CONTR:
        obj = make_contr_rec(rec)
    return obj

cdef class Exec(object):
    def __init__(self):
        raise TypeError("init called on Exec")

cdef Exec make_exec(DbrpyExec* exc):
    cdef obj = Exec.__new__(Exec)
    obj.id = exc.id
    obj.ref = exc.c.ref[:string.strnlen(exc.c.ref, dbr.DBR_REF_MAX)]
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
    cdef dbr.DbrPool impl_

    def __cinit__(self, capacity):
        self.impl_ = dbr.dbr_pool_create(capacity)
        if self.impl_ is NULL:
            raise Error()

    def __dealloc__(self):
        if self.impl_ is not NULL:
            dbr.dbr_pool_destroy(self.impl_)

cdef struct HandlerImpl:
    PyObject* target
    DbrpyIHandler handler

cdef inline void* handler_target(dbr.DbrHandler handler):
    cdef size_t offset = <size_t>&(<HandlerImpl*>NULL).handler
    cdef HandlerImpl* impl = <HandlerImpl*>(<char*>handler - offset)
    return impl.target

cdef void on_up(dbr.DbrHandler handler, int conn):
    (<object>handler_target(handler)).on_up(conn)

cdef void on_down(dbr.DbrHandler handler, int conn):
    (<object>handler_target(handler)).on_down(conn)

cdef void on_logon(dbr.DbrHandler handler, dbr.DbrIden tid):
    (<object>handler_target(handler)).on_logon(tid)

cdef void on_logoff(dbr.DbrHandler handler, dbr.DbrIden tid):
    (<object>handler_target(handler)).on_logoff(tid)

cdef void on_timeout(dbr.DbrHandler handler, dbr.DbrIden req_id):
    (<object>handler_target(handler)).on_timeout(req_id)

cdef void on_status(dbr.DbrHandler handler, dbr.DbrIden req_id, int num, const char* msg):
    (<object>handler_target(handler)).on_status(req_id, num, msg)

cdef void on_exec(dbr.DbrHandler handler, dbr.DbrIden req_id, DbrpyExec* exc):
    (<object>handler_target(handler)).on_exec(req_id, req_id, make_exec(exc))

cdef void on_posn(dbr.DbrHandler handler, DbrpyPosn* posn):
    (<object>handler_target(handler)).on_posn(make_posn(posn))

cdef void on_view(dbr.DbrHandler handler, DbrpyView* view):
    (<object>handler_target(handler)).on_view(make_view(view))

cdef void on_flush(dbr.DbrHandler handler):
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
    cdef dbr.DbrClnt impl_

    def __cinit__(self, const char* sess, ZmqCtx ctx, const char* mdaddr,
                  const char* traddr, dbr.DbrIden seed, Pool pool):
        self.impl_ = dbr.dbr_clnt_create(sess, ctx.impl_, mdaddr, traddr, seed, pool.impl_)
        if self.impl_ is NULL:
            raise Error()

    def __dealloc__(self):
        if self.impl_ is not NULL:
            dbr.dbr_clnt_destroy(self.impl_)

    def close(self, dbr.DbrMillis ms):
        if dbr.dbr_clnt_close(self.impl_, ms) == -1:
            raise Error()

    def find_rec_id(self, int type, dbr.DbrIden id):
        cdef DbrpySlNode* node = dbr.dbr_clnt_find_rec_id(self.impl_, type, id)
        return make_rec(dbr.dbr_shared_rec_entry(node)) if not NULL else None

    def is_open(self):
        return <bint>dbr.dbr_clnt_is_open(self.impl_)

    def is_ready(self):
        return <bint>dbr.dbr_clnt_is_ready(self.impl_)

    def poll(self, dbr.DbrMillis ms, Handler handler):
        return dbr.dbr_clnt_poll(self.impl_, ms, &handler.impl_.handler)

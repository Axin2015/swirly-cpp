cimport dbr
cimport zmq

cdef extern from "dbrpy/dbrpy.h":

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
        return repr("{1}:{2}: {3} ({0})".format(self.num, self.file, self.line, self.msg))

def millis():
    return dbr.dbr_millis()

cdef class ZmqCtx:
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

cdef class Pool:
    cdef dbr.DbrPool impl_

    def __cinit__(self, capacity):
        self.impl_ = dbr.dbr_pool_create(capacity)
        if self.impl_ is NULL:
            raise Error()

    def __dealloc__(self):
        if self.impl_ is not NULL:
            dbr.dbr_pool_destroy(self.impl_)

cdef struct HandlerImpl:
    void* target
    DbrpyIHandler handler

cdef void on_up(dbr.DbrHandler handler, int conn):
    cdef size_t offset = <size_t>&(<HandlerImpl*>NULL).handler
    cdef HandlerImpl* impl = <HandlerImpl*>(<char*>handler - offset)
    (<object>impl.target).on_up(conn)

cdef class Handler:
    cdef DbrpyHandlerVtbl vtbl_
    cdef HandlerImpl impl_

    def __init__(self):
        self.vtbl_.on_up = on_up
        self.impl_.target = <void*>self
        self.impl_.handler.vtbl = &self.vtbl_

cdef class Clnt:
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

    def is_open(self):
        return <bint>dbr.dbr_clnt_is_open(self.impl_)

    def is_ready(self):
        return <bint>dbr.dbr_clnt_is_ready(self.impl_)

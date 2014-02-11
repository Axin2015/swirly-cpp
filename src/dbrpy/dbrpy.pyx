cimport dbr
cimport zmq

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

def err_set(num, msg):
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

cdef class Pool:
    cdef dbr.DbrPool impl_

    def __cinit__(self, capacity):
        self.impl_ = dbr.dbr_pool_create(capacity)
        if self.impl_ is NULL:
            raise Error()

    def __dealloc__(self):
        if self.impl_ is not NULL:
            dbr.dbr_pool_destroy(self.impl_)

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

# def err_msg(ZmqCtx ctx):
#     return ctx.impl_ is NULL

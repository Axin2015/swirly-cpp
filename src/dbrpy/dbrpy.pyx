cimport cdbrpy

from inspect import currentframe, getframeinfo

def err_set(num, msg):
    fi = getframeinfo(currentframe())
    cdbrpy.dbr_err_set_(num, fi.filename, fi.lineno, msg)

def err_num():
    return cdbrpy.dbr_err_num()

def err_file():
    return cdbrpy.dbr_err_file()

def err_line():
    return cdbrpy.dbr_err_line()

def err_msg():
    return cdbrpy.dbr_err_msg()

class Error(Exception):
    def __init__(self):
        self.num = err_num()
        self.fname = err_file()
        self.line = err_line()
        self.msg = err_msg()
    def __str__(self):
        return repr("%s:%d: %s (%d)" % (self.fname, self.line, self.msg, self.num))

cdef class Pool:
    cdef cdbrpy.DbrPool _c_pool

    def __cinit__(self, capacity):
        self._c_pool = cdbrpy.dbr_pool_create(capacity)
        if self._c_pool is NULL:
            raise Error()

    def __dealloc__(self):
        if self._c_pool is not NULL:
            cdbrpy.dbr_pool_destroy(self._c_pool)

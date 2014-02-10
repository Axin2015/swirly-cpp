cimport cdbr

def err_num():
    return cdbr.dbr_err_num()

def dbr_err_file():
    return cdbr.dbr_err_file()

def err_line():
    return cdbr.dbr_err_int()

def err_msg():
    return cdbr.dbr_err_msg()

cdef class Pool:
    cdef cdbr.DbrPool _c_pool

    def __cinit__(self, capacity):
        self._c_pool = cdbr.dbr_pool_create(capacity)
        if self._c_pool is NULL:
            raise MemoryError()

    def __dealloc__(self):
        if self._c_pool is not NULL:
            cdbr.dbr_pool_destroy(self._c_pool)


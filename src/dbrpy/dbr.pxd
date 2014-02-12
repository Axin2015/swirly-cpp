cdef extern from "dbr/defs.h":

    cdef enum:
        DBR_FALSE
        DBR_TRUE

    ctypedef DbrBool
    ctypedef DbrIden
    ctypedef DbrKey

cdef extern from "dbr/err.h":

    cdef enum DbrErrno:
        DBR_EINTR
        DBR_EIO
        DBR_ENOMEM
        DBR_EACCES
        DBR_EBUSY
        DBR_EEXIST
        DBR_EINVAL
        DBR_ETIMEOUT
        DBR_EUSER

    void dbr_err_set_(int num, const char* file, int line, const char* msg)

    void dbr_err_setf_(int num, const char* file, int line, const char* format, ...)

    int dbr_err_num()

    const char* dbr_err_file()

    int dbr_err_line()

    const char* dbr_err_msg()

cdef extern from "dbr/util.h":

    long dbr_millis()

cdef extern from "dbr/types.h":

    ctypedef DbrMillis

cdef extern from "dbr/pool.h":

    ctypedef struct ElmPool:
        pass

    ctypedef ElmPool* DbrPool

    DbrPool dbr_pool_create(size_t capacity)

    void dbr_pool_destroy(DbrPool pool)

cdef extern from "dbr/clnt.h":

    ctypedef struct FigClnt:
        pass

    ctypedef FigClnt* DbrClnt

    DbrClnt dbr_clnt_create(const char* sess, void* ctx, const char* mdaddr,
                            const char* traddr, DbrIden seed, DbrPool pool)

    void dbr_clnt_destroy(DbrClnt clnt)

    int dbr_clnt_close(DbrClnt clnt, DbrMillis ms)

cdef extern from "dbr/err.h":

    void dbr_err_set_(int num, const char* fname, int line, const char* msg)

    int dbr_err_num()

    const char* dbr_err_file()

    int dbr_err_line()

    const char* dbr_err_msg()

cdef extern from "dbr/pool.h":

    ctypedef struct ElmPool:
        pass

    ctypedef ElmPool* DbrPool

    DbrPool dbr_pool_create(size_t capacity)

    void dbr_pool_destroy(DbrPool pool)

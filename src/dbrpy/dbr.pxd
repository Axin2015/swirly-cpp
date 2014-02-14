cdef extern from "dbr/slnode.h":

    ctypedef struct DbrSlNode

    ctypedef struct DbrSlNode:
        DbrSlNode* next

cdef extern from "dbr/defs.h":

    cdef enum:
        DBR_FALSE
        DBR_TRUE

    ctypedef int DbrBool
    ctypedef long DbrIden
    ctypedef long DbrKey

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

    ctypedef int DbrDate
    ctypedef long DbrIncs
    ctypedef long DbrMillis
    ctypedef DbrIncs DbrLots
    ctypedef DbrIncs DbrTicks
    ctypedef DbrIncs DbrLicks

    cdef enum DbrEntity:
        DBR_ENTITY_TRADER
        DBR_ENTITY_ACCNT
        DBR_ENTITY_CONTR
        DBR_ENTITY_ORDER
        DBR_ENTITY_EXEC
        DBR_ENTITY_MEMB
        DBR_ENTITY_POSN

    ctypedef struct FigTrader:
        pass

    ctypedef FigTrader* DbrTrader

    ctypedef struct FigAccnt:
        pass

    ctypedef FigAccnt* DbrAccnt

    cdef enum:
        DBR_DISPLAY_MAX
        DBR_EMAIL_MAX
        DBR_MNEM_MAX
        DBR_REF_MAX

    ctypedef char DbrDisplay[DBR_DISPLAY_MAX]
    ctypedef char DbrEmail[DBR_EMAIL_MAX]
    ctypedef char DbrMnem[DBR_MNEM_MAX]
    ctypedef char DbrRef[DBR_REF_MAX]

    ctypedef struct DbrRec

    ctypedef union DbrURec:
        DbrIden id_only
        DbrRec* rec

    ctypedef struct DbrTraderAnon:
        DbrEmail email
        DbrTrader state

    ctypedef struct DbrAccntAnon:
        DbrEmail email
        DbrAccnt state

    ctypedef struct DbrContrAnon:
        DbrMnem asset_type
        DbrMnem asset
        DbrMnem ccy
        int tick_numer
        int tick_denom
        double price_inc
        int lot_numer
        int lot_denom
        double qty_inc
        int price_dp
        int pip_dp
        int qty_dp
        DbrLots min_lots
        DbrLots max_lots

    ctypedef struct DbrRec:
        int type
        DbrIden id
        DbrMnem mnem
        DbrDisplay display
        DbrTraderAnon trader
        DbrAccntAnon accnt
        DbrContrAnon contr

    DbrRec* dbr_shared_rec_entry(DbrSlNode* node)

    ctypedef struct DbrCommon:
        DbrRef ref

    ctypedef struct DbrExec:
        DbrIden id
        DbrIden order
        DbrCommon c

    ctypedef struct DbrPosn:
        pass

    ctypedef struct DbrView:
        pass

cdef extern from "dbr/pool.h":

    ctypedef struct ElmPool:
        pass

    ctypedef ElmPool* DbrPool

    DbrPool dbr_pool_create(size_t capacity)

    void dbr_pool_destroy(DbrPool pool)

cdef extern from "dbr/handler.h":

    ctypedef struct DbrHandlerVtbl

    ctypedef struct DbrIHandler:
        const DbrHandlerVtbl* vtbl

    ctypedef DbrIHandler* DbrHandler

    ctypedef struct DbrHandlerVtbl:

        void on_up(DbrHandler handler, int conn)

        void on_down(DbrHandler handler, int conn)

        void on_logon(DbrHandler handler, DbrIden tid)

        void on_logoff(DbrHandler handler, DbrIden tid)

        void on_timeout(DbrHandler handler, DbrIden req_id)

        void on_status(DbrHandler handler, DbrIden req_id, int num, const char* msg)

        void on_exec(DbrHandler handler, DbrIden req_id, DbrExec* exc)

        void on_posn(DbrHandler handler, DbrPosn* posn)

        void on_view(DbrHandler handler, DbrView* view)

        void on_flush(DbrHandler handler)

cdef extern from "dbr/clnt.h":

    ctypedef struct FigClnt:
        pass

    ctypedef FigClnt* DbrClnt

    DbrClnt dbr_clnt_create(const char* sess, void* ctx, const char* mdaddr,
                            const char* traddr, DbrIden seed, DbrPool pool)

    void dbr_clnt_destroy(DbrClnt clnt)

    int dbr_clnt_close(DbrClnt clnt, DbrMillis ms)

    DbrSlNode* dbr_clnt_find_rec_id(DbrClnt clnt, int type, DbrIden id)

    DbrBool dbr_clnt_is_open(DbrClnt clnt)

    DbrBool dbr_clnt_is_ready(DbrClnt clnt)

    int dbr_clnt_poll(DbrClnt clnt, DbrMillis ms, DbrHandler handler)

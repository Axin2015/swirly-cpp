cdef extern from "dbr/rbnode.h":

    ctypedef struct DbrRbNode

    DbrRbNode* dbr_rbnode_next(DbrRbNode* node)

    DbrRbNode* dbr_rbnode_prev(DbrRbNode* node)

cdef extern from "dbr/slnode.h":

    ctypedef struct DbrSlNode

    ctypedef struct DbrSlNode:
        DbrSlNode* next

    DbrSlNode* dbr_slnode_next(DbrSlNode* node)

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

    cdef enum DbrState:
        DBR_STATE_NEW
        DBR_STATE_REVISE
        DBR_STATE_CANCEL
        DBR_STATE_TRADE

    cdef enum DbrAction:
        DBR_ACTION_BUY
        DBR_ACTION_SELL

    ctypedef struct DbrCommon:
        DbrURec trader
        DbrURec accnt
        DbrURec contr
        DbrDate settl_date
        DbrRef ref
        int state
        int action
        DbrTicks ticks
        DbrLots lots
        DbrLots resd
        DbrLots exc "exec"
        DbrTicks last_ticks
        DbrLots last_lots
        DbrLots min_lots

    ctypedef struct DbrOrder:
        DbrIden id
        DbrCommon c
        DbrMillis created
        DbrMillis modified

    cdef enum DbrRole:
        DBR_ROLE_MAKER
        DBR_ROLE_TAKER

    ctypedef struct DbrExec:
        DbrIden id
        DbrIden order
        DbrCommon c
        DbrIden match
        int role
        DbrURec cpty
        DbrMillis created

    ctypedef struct DbrPosn:
        DbrURec accnt
        DbrURec contr
        DbrDate settl_date
        DbrLicks buy_licks
        DbrLots buy_lots
        DbrLicks sell_licks
        DbrLots sell_lots

    ctypedef struct DbrView:
        pass

cdef extern from "dbr/pool.h":

    ctypedef struct ElmPool:
        pass

    ctypedef ElmPool* DbrPool

    DbrPool dbr_pool_create(size_t capacity)

    void dbr_pool_destroy(DbrPool pool)

cdef extern from "dbr/conv.h":

    double dbr_fract_to_real(int numer, int denom)

    DbrIncs dbr_real_to_incs(double real, double inc_size)

    double dbr_incs_to_real(DbrIncs incs, double inc_size)

    DbrLots dbr_qty_to_lots(double qty, DbrRec* crec)

    double dbr_lots_to_qty(DbrLots lots, DbrRec* crec)

    DbrTicks dbr_price_to_ticks(double price, DbrRec* crec)

    double dbr_ticks_to_price(DbrTicks ticks, DbrRec* crec)

    int dbr_real_to_dp(double d)

    double dbr_dp_to_real(int dp)

cdef extern from "dbr/trader.h":

    DbrOrder* dbr_trader_order_entry(DbrRbNode* node)

    DbrRbNode* dbr_trader_find_order_id(DbrTrader trader, DbrIden id)

    DbrOrder* dbr_trader_find_order_ref(DbrTrader trader, const char* ref)

cdef extern from "dbr/accnt.h":

    DbrPosn* dbr_accnt_posn_entry(DbrRbNode* node)

    DbrRbNode* dbr_accnt_find_posn_id(DbrAccnt accnt, DbrIden id)

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

    DbrIden dbr_clnt_close(DbrClnt clnt, DbrMillis ms)

    DbrSlNode* dbr_clnt_find_rec_id(DbrClnt clnt, int type, DbrIden id)

    DbrSlNode* dbr_clnt_find_rec_mnem(DbrClnt clnt, int type, const char* mnem)

    DbrSlNode* dbr_clnt_first_rec(DbrClnt clnt, int type, size_t* size)

    DbrBool dbr_clnt_empty_rec(DbrClnt clnt, int type)

    DbrTrader dbr_clnt_trader(DbrClnt clnt, DbrRec* trec)

    DbrAccnt dbr_clnt_accnt(DbrClnt clnt, DbrRec* arec)

    DbrIden dbr_clnt_logon(DbrClnt clnt, DbrTrader trader, DbrMillis ms)

    DbrIden dbr_clnt_logoff(DbrClnt clnt, DbrTrader trader, DbrMillis ms)

    DbrIden dbr_clnt_place(DbrClnt clnt, DbrTrader trader, DbrAccnt accnt,
                           DbrRec* crec, DbrDate settl_date, const char* ref,
                           int action, DbrTicks ticks, DbrLots lots,
                           DbrLots min_lots, DbrMillis ms)

    DbrBool dbr_clnt_is_open(DbrClnt clnt)

    DbrBool dbr_clnt_is_ready(DbrClnt clnt)

    int dbr_clnt_poll(DbrClnt clnt, DbrMillis ms, DbrHandler handler)

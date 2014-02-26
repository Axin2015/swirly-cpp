cdef extern from "dbr/defs.h":

    cdef enum:
        DBR_FALSE
        DBR_TRUE

    ctypedef int DbrBool
    ctypedef long DbrIden
    ctypedef long DbrKey

    cdef enum:
        DBR_ERRMSG_MAX

cdef extern from "dbr/rbnode.h":

    ctypedef struct DbrRbNode

    DbrRbNode* dbr_rbnode_next(DbrRbNode* node) nogil

    DbrRbNode* dbr_rbnode_prev(DbrRbNode* node) nogil

cdef extern from "dbr/slnode.h":

    ctypedef struct DbrSlNode

    ctypedef struct DbrSlNode:
        DbrSlNode* next

    DbrSlNode* dbr_slnode_next(DbrSlNode* node) nogil

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

    void dbr_err_clear() nogil

    void dbr_err_print() nogil

    void dbr_err_prints(const char* s) nogil

    void dbr_err_set_(int num, const char* file, int line, const char* msg) nogil

    void dbr_err_setf_(int num, const char* file, int line, const char* format, ...) nogil

    int dbr_err_num() nogil

    const char* dbr_err_file() nogil

    int dbr_err_line() nogil

    const char* dbr_err_msg() nogil

cdef extern from "dbr/util.h":

    int dbr_intdig(int i) nogil

    int dbr_longdig(long l) nogil

    size_t dbr_intlen(int i) nogil

    size_t dbr_longlen(long l) nogil

    long dbr_millis() nogil

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

    DbrRec* dbr_shared_rec_entry(DbrSlNode* node) nogil

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

    DbrBool dbr_order_done(const DbrOrder* order) nogil

    ctypedef struct DbrLevel:
        pass

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

    DbrBool dbr_exec_done(const DbrExec* exc) nogil

    ctypedef struct DbrMatch:
        pass

    ctypedef struct DbrTrans:
        pass

    ctypedef struct DbrMemb:
        DbrURec trader
        DbrURec accnt

    ctypedef struct DbrPosn:
        DbrURec accnt
        DbrURec contr
        DbrDate settl_date
        DbrLicks buy_licks
        DbrLots buy_lots
        DbrLicks sell_licks
        DbrLots sell_lots

    cdef enum:
        DBR_LEVEL_MAX

    ctypedef struct DbrView:
        DbrURec contr
        DbrDate settl_date
        DbrTicks bid_ticks[DBR_LEVEL_MAX]
        DbrLots bid_lots[DBR_LEVEL_MAX]
        size_t bid_count[DBR_LEVEL_MAX]
        DbrTicks ask_ticks[DBR_LEVEL_MAX]
        DbrLots ask_lots[DBR_LEVEL_MAX]
        size_t ask_count[DBR_LEVEL_MAX]

    ctypedef struct DbrSide:
        pass

cdef extern from "dbr/pool.h":

    ctypedef struct ElmPool:
        pass

    ctypedef ElmPool* DbrPool

    DbrPool dbr_pool_create(size_t capacity) nogil

    void dbr_pool_destroy(DbrPool pool) nogil

cdef extern from "dbr/conv.h":

    double dbr_fract_to_real(int numer, int denom) nogil

    DbrIncs dbr_real_to_incs(double real, double inc_size) nogil

    double dbr_incs_to_real(DbrIncs incs, double inc_size) nogil

    DbrLots dbr_qty_to_lots(double qty, DbrRec* crec) nogil

    double dbr_lots_to_qty(DbrLots lots, DbrRec* crec) nogil

    DbrTicks dbr_price_to_ticks(double price, DbrRec* crec) nogil

    double dbr_ticks_to_price(DbrTicks ticks, DbrRec* crec) nogil

    int dbr_real_to_dp(double d) nogil

    double dbr_dp_to_real(int dp) nogil

cdef extern from "dbr/async.h":

    ctypedef struct ElmAsync:
        pass

    ctypedef ElmAsync* DbrAsync

    DbrAsync dbr_async_create(void* ctx, const char* sess) nogil

    void dbr_async_destroy(DbrAsync async) nogil

    DbrBool dbr_async_send(DbrAsync async, void* val) nogil

    DbrBool dbr_async_recv(DbrAsync async, void** val) nogil

cdef extern from "dbr/trader.h":

    DbrRec* dbr_trader_rec(DbrTrader trader) nogil

    DbrOrder* dbr_trader_order_entry(DbrRbNode* node) nogil

    DbrRbNode* dbr_trader_find_order_id(DbrTrader trader, DbrIden id) nogil

    DbrOrder* dbr_trader_find_order_ref(DbrTrader trader, const char* ref) nogil

    DbrRbNode* dbr_trader_first_order(DbrTrader trader) nogil

    DbrRbNode* dbr_trader_last_order(DbrTrader trader) nogil

    DbrBool dbr_trader_empty_order(DbrTrader trader) nogil

    DbrExec* dbr_trader_trade_entry(DbrRbNode* node) nogil

    DbrRbNode* dbr_trader_find_trade_id(DbrTrader trader, DbrIden id) nogil

    DbrRbNode* dbr_trader_first_trade(DbrTrader trader) nogil

    DbrRbNode* dbr_trader_last_trade(DbrTrader trader) nogil

    DbrBool dbr_trader_empty_trade(DbrTrader trader) nogil

    DbrMemb* dbr_trader_memb_entry(DbrRbNode* node) nogil

    DbrRbNode* dbr_trader_find_memb_id(DbrTrader trader, DbrIden id) nogil

    DbrRbNode* dbr_trader_first_memb(DbrTrader trader) nogil

    DbrRbNode* dbr_trader_last_memb(DbrTrader trader) nogil

    DbrBool dbr_trader_empty_memb(DbrTrader trader) nogil

    DbrBool dbr_trader_logged_on(DbrTrader trader) nogil

cdef extern from "dbr/accnt.h":

    DbrRec* dbr_accnt_rec(DbrAccnt accnt) nogil

    DbrMemb* dbr_accnt_memb_entry(DbrRbNode* node) nogil

    DbrRbNode* dbr_accnt_find_memb_id(DbrAccnt accnt, DbrIden id) nogil

    DbrRbNode* dbr_accnt_first_memb(DbrAccnt accnt) nogil

    DbrRbNode* dbr_accnt_last_memb(DbrAccnt accnt) nogil

    DbrBool dbr_accnt_empty_memb(DbrAccnt accnt) nogil

    DbrPosn* dbr_accnt_posn_entry(DbrRbNode* node) nogil

    DbrRbNode* dbr_accnt_find_posn_id(DbrAccnt accnt, DbrIden id) nogil

    DbrRbNode* dbr_accnt_first_posn(DbrAccnt accnt) nogil

    DbrRbNode* dbr_accnt_last_posn(DbrAccnt accnt) nogil

    DbrBool dbr_accnt_empty_posn(DbrAccnt accnt) nogil

cdef extern from "dbr/handler.h":

    cdef enum DbrConn:
        DBR_CONN_TR
        DBR_CONN_MD

    ctypedef struct DbrHandlerVtbl

    ctypedef struct DbrIHandler:
        const DbrHandlerVtbl* vtbl

    ctypedef DbrIHandler* DbrHandler

    ctypedef struct DbrHandlerVtbl:

        void on_up(DbrHandler handler, int conn) nogil

        void on_down(DbrHandler handler, int conn) nogil

        void on_logon(DbrHandler handler, DbrIden tid) nogil

        void on_logoff(DbrHandler handler, DbrIden tid) nogil

        void on_timeout(DbrHandler handler, DbrIden req_id) nogil

        void on_status(DbrHandler handler, DbrIden req_id, int num, const char* msg) nogil

        void on_exec(DbrHandler handler, DbrIden req_id, DbrExec* exc) nogil

        void on_posn(DbrHandler handler, DbrPosn* posn) nogil

        void on_view(DbrHandler handler, DbrView* view) nogil

        void on_flush(DbrHandler handler) nogil

        void* on_async(DbrHandler handler, void* val) nogil

cdef extern from "dbr/clnt.h":

    ctypedef struct FigClnt:
        pass

    ctypedef FigClnt* DbrClnt

    DbrClnt dbr_clnt_create(void* ctx, const char* sess, const char* mdaddr,
                            const char* traddr, DbrIden seed, DbrPool pool) nogil

    void dbr_clnt_destroy(DbrClnt clnt) nogil

    DbrRec* dbr_clnt_rec_entry(DbrSlNode* node) nogil

    DbrIden dbr_clnt_close(DbrClnt clnt, DbrMillis ms) nogil

    DbrSlNode* dbr_clnt_find_rec_id(DbrClnt clnt, int type, DbrIden id) nogil

    DbrSlNode* dbr_clnt_find_rec_mnem(DbrClnt clnt, int type, const char* mnem) nogil

    DbrSlNode* dbr_clnt_first_rec(DbrClnt clnt, int type, size_t* size) nogil

    DbrBool dbr_clnt_empty_rec(DbrClnt clnt, int type) nogil

    DbrTrader dbr_clnt_trader(DbrClnt clnt, DbrRec* trec) nogil

    DbrAccnt dbr_clnt_accnt(DbrClnt clnt, DbrRec* arec) nogil

    DbrIden dbr_clnt_logon(DbrClnt clnt, DbrTrader trader, DbrMillis ms) nogil

    DbrIden dbr_clnt_logoff(DbrClnt clnt, DbrTrader trader, DbrMillis ms) nogil

    DbrIden dbr_clnt_place(DbrClnt clnt, DbrTrader trader, DbrAccnt accnt,
                           DbrRec* crec, DbrDate settl_date, const char* ref,
                           int action, DbrTicks ticks, DbrLots lots,
                           DbrLots min_lots, DbrMillis ms) nogil

    DbrIden dbr_clnt_revise_id(DbrClnt clnt, DbrTrader trader, DbrIden id, DbrLots lots, DbrMillis ms) nogil

    DbrIden dbr_clnt_revise_ref(DbrClnt clnt, DbrTrader trader, const char* ref, DbrLots lots, DbrMillis ms) nogil

    DbrIden dbr_clnt_cancel_id(DbrClnt clnt, DbrTrader trader, DbrIden id, DbrMillis ms) nogil

    DbrIden dbr_clnt_cancel_ref(DbrClnt clnt, DbrTrader trader, const char* ref, DbrMillis ms) nogil

    DbrIden dbr_clnt_ack_trade(DbrClnt clnt, DbrTrader trader, DbrIden id, DbrMillis ms) nogil

    DbrBool dbr_clnt_is_open(DbrClnt clnt) nogil

    DbrBool dbr_clnt_is_ready(DbrClnt clnt) nogil

    DbrIden dbr_clnt_settimer(DbrClnt clnt, DbrMillis absms) nogil

    void dbr_clnt_canceltimer(DbrClnt clnt, DbrIden id) nogil

    DbrBool dbr_clnt_dispatch(DbrClnt clnt, DbrMillis ms, DbrHandler handler) nogil

    void dbr_clnt_mdclear(DbrClnt clnt) nogil

    void dbr_clnt_trclear(DbrClnt clnt) nogil

    void dbr_clnt_clear(DbrClnt clnt) nogil

    DbrView* dbr_clnt_view_entry(DbrRbNode* node) nogil

    DbrRbNode* dbr_clnt_find_view(DbrClnt clnt, DbrIden cid, DbrDate settl_date) nogil

    DbrRbNode* dbr_clnt_first_view(DbrClnt clnt) nogil

    DbrRbNode* dbr_clnt_last_view(DbrClnt clnt) nogil

    DbrBool dbr_clnt_empty_view(DbrClnt clnt) nogil

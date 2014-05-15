cdef extern from "dbr/defs.h":

    cdef enum:
        DBR_FALSE
        DBR_TRUE

    cdef enum:
        DBR_ERRMSG_MAX

    ctypedef int DbrBool
    ctypedef long DbrIden
    ctypedef unsigned long DbrKey
    ctypedef unsigned char DbrUuid[16]

cdef extern from "dbr/date.h":

    ctypedef int DbrIsoDate
    ctypedef int DbrJd

    ctypedef struct DbrYmd:
        int year
        int mon
        int mday

    DbrIsoDate dbr_ymd_to_iso(int year, int mon, int mday) nogil

    DbrYmd* dbr_iso_to_ymd(DbrIsoDate iso, DbrYmd* ymd) nogil

    DbrJd dbr_ymd_to_jd(int year, int mon, int mday) nogil

    DbrYmd* dbr_jd_to_ymd(DbrJd jd, DbrYmd* ymd) nogil

    DbrJd dbr_jd_to_mjd(DbrJd jd) nogil

    DbrJd dbr_mjd_to_jd(DbrJd mjd) nogil

    DbrJd dbr_jd_to_tjd(DbrJd jd) nogil

    DbrJd dbr_tjd_to_jd(DbrJd tjd) nogil

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
        DBR_ESYSTEM
        DBR_EUSER

    void dbr_err_clear() nogil

    void dbr_err_perror(const char* s) nogil

    void dbr_err_set_(int num, const char* file, int line, const char* msg) nogil

    void dbr_err_setf_(int num, const char* file, int line, const char* format, ...) nogil

    void dbr_err_print_(int num, const char* file, int line, const char* msg) nogil

    void dbr_err_printf_(int num, const char* file, int line, const char* format, ...) nogil

    int dbr_err_num() nogil

    const char* dbr_err_file() nogil

    int dbr_err_line() nogil

    const char* dbr_err_msg() nogil

cdef extern from "dbr/log.h":

    cdef enum DbrLog:
        DBR_LOG_CRIT
        DBR_LOG_ERROR
        DBR_LOG_WARN
        DBR_LOG_NOTICE
        DBR_LOG_INFO
        DBR_LOG_DEBUG1
        DBR_LOG_DEBUG2
        DBR_LOG_DEBUG3


    int dbr_log_level() nogil

    int dbr_log_setlevel(int level) nogil

    void dbr_log_printf(int level, const char* format, ...) nogil

    void dbr_log_crit(const char* format, ...) nogil

    void dbr_log_error(const char* format, ...) nogil

    void dbr_log_warn(const char* format, ...) nogil

    void dbr_log_notice(const char* format, ...) nogil

    void dbr_log_info(const char* format, ...) nogil

    void dbr_log_debug1(const char* format, ...) nogil

    void dbr_log_debug2(const char* format, ...) nogil

    void dbr_log_debug3(const char* format, ...) nogil

cdef extern from "dbr/util.h":

    int dbr_intdig(int i) nogil

    int dbr_longdig(long l) nogil

    size_t dbr_intlen(int i) nogil

    size_t dbr_longlen(long l) nogil

    long dbr_millis() nogil

cdef extern from "dbr/types.h":

    ctypedef long DbrIncs
    ctypedef long DbrMillis
    ctypedef DbrIncs DbrLots
    ctypedef DbrIncs DbrTicks
    ctypedef DbrIncs DbrLicks

    cdef enum DbrEntity:
        DBR_ENTITY_ACCNT
        DBR_ENTITY_CONTR
        DBR_ENTITY_MEMB
        DBR_ENTITY_ORDER
        DBR_ENTITY_EXEC
        DBR_ENTITY_POSN

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

    ctypedef struct DbrAccntAnon:
        DbrEmail email
        DbrAccnt state

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
        DbrAccntAnon accnt
        DbrContrAnon contr

    DbrRec* dbr_shared_rec_entry(DbrSlNode* node) nogil

    ctypedef struct DbrMemb:
        DbrURec group
        DbrURec user

    cdef enum DbrState:
        DBR_STATE_NEW
        DBR_STATE_REVISE
        DBR_STATE_CANCEL
        DBR_STATE_TRADE

    cdef enum DbrAction:
        DBR_ACTION_BUY
        DBR_ACTION_SELL

    ctypedef struct DbrCommon:
        DbrURec user
        DbrURec group
        DbrURec contr
        DbrJd settl_day
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

    ctypedef struct DbrPosn:
        DbrURec accnt
        DbrURec contr
        DbrJd settl_day
        DbrLicks buy_licks
        DbrLots buy_lots
        DbrLicks sell_licks
        DbrLots sell_lots

    cdef enum:
        DBR_LEVEL_MAX

    ctypedef struct DbrView:
        DbrURec contr
        DbrJd settl_day
        DbrTicks bid_ticks[DBR_LEVEL_MAX]
        DbrLots bid_lots[DBR_LEVEL_MAX]
        size_t bid_count[DBR_LEVEL_MAX]
        DbrTicks offer_ticks[DBR_LEVEL_MAX]
        DbrLots offer_lots[DBR_LEVEL_MAX]
        size_t offer_count[DBR_LEVEL_MAX]

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

    DbrAsync dbr_async_create(void* zctx, const DbrUuid uuid) nogil

    void dbr_async_destroy(DbrAsync async) nogil

    DbrBool dbr_async_send(DbrAsync async, void* val) nogil

    DbrBool dbr_async_recv(DbrAsync async, void** val) nogil

cdef extern from "dbr/accnt.h":

    DbrRec* dbr_accnt_rec(DbrAccnt accnt) nogil

    DbrMemb* dbr_accnt_user_entry(DbrRbNode* node) nogil

    DbrRbNode* dbr_accnt_find_user_id(DbrAccnt accnt, DbrIden id) nogil

    DbrRbNode* dbr_accnt_first_user(DbrAccnt accnt) nogil

    DbrRbNode* dbr_accnt_last_user(DbrAccnt accnt) nogil

    DbrBool dbr_accnt_empty_user(DbrAccnt accnt) nogil

    DbrMemb* dbr_accnt_group_entry(DbrRbNode* node) nogil

    DbrRbNode* dbr_accnt_find_group_id(DbrAccnt accnt, DbrIden id) nogil

    DbrRbNode* dbr_accnt_first_group(DbrAccnt accnt) nogil

    DbrRbNode* dbr_accnt_last_group(DbrAccnt accnt) nogil

    DbrBool dbr_accnt_empty_group(DbrAccnt accnt) nogil

    DbrOrder* dbr_accnt_order_entry(DbrRbNode* node) nogil

    DbrRbNode* dbr_accnt_find_order_id(DbrAccnt accnt, DbrIden id) nogil

    DbrOrder* dbr_accnt_find_order_ref(DbrAccnt accnt, const char* ref) nogil

    DbrRbNode* dbr_accnt_first_order(DbrAccnt accnt) nogil

    DbrRbNode* dbr_accnt_last_order(DbrAccnt accnt) nogil

    DbrBool dbr_accnt_empty_order(DbrAccnt accnt) nogil

    DbrExec* dbr_accnt_trade_entry(DbrRbNode* node) nogil

    DbrRbNode* dbr_accnt_find_trade_id(DbrAccnt accnt, DbrIden id) nogil

    DbrRbNode* dbr_accnt_first_trade(DbrAccnt accnt) nogil

    DbrRbNode* dbr_accnt_last_trade(DbrAccnt accnt) nogil

    DbrBool dbr_accnt_empty_trade(DbrAccnt accnt) nogil

    DbrPosn* dbr_accnt_posn_entry(DbrRbNode* node) nogil

    DbrRbNode* dbr_accnt_find_posn_id(DbrAccnt accnt, DbrIden id) nogil

    DbrRbNode* dbr_accnt_first_posn(DbrAccnt accnt) nogil

    DbrRbNode* dbr_accnt_last_posn(DbrAccnt accnt) nogil

    DbrBool dbr_accnt_empty_posn(DbrAccnt accnt) nogil

    DbrBool dbr_accnt_logged_on(DbrAccnt accnt) nogil

cdef extern from "dbr/clnt.h":

    ctypedef struct FigClnt:
        pass

    ctypedef FigClnt* DbrClnt

    DbrClnt dbr_clnt_create(void* zctx, const DbrUuid uuid, const char* mdaddr,
                            const char* traddr, DbrIden seed, DbrMillis tmout,
                            DbrPool pool) nogil

    void dbr_clnt_destroy(DbrClnt clnt) nogil

    void dbr_clnt_reset(DbrClnt clnt) nogil

    DbrIden dbr_clnt_close(DbrClnt clnt) nogil

    DbrRec* dbr_clnt_rec_entry(DbrSlNode* node) nogil

    DbrSlNode* dbr_clnt_find_rec_id(DbrClnt clnt, int type, DbrIden id) nogil

    DbrSlNode* dbr_clnt_find_rec_mnem(DbrClnt clnt, int type, const char* mnem) nogil

    DbrSlNode* dbr_clnt_first_rec(DbrClnt clnt, int type, size_t* size) nogil

    DbrBool dbr_clnt_empty_rec(DbrClnt clnt, int type) nogil

    DbrAccnt dbr_clnt_accnt(DbrClnt clnt, DbrRec* arec) nogil

    DbrIden dbr_clnt_logon(DbrClnt clnt, DbrAccnt user) nogil

    DbrIden dbr_clnt_logoff(DbrClnt clnt, DbrAccnt user) nogil

    DbrIden dbr_clnt_place(DbrClnt clnt, DbrAccnt user, DbrAccnt group, DbrRec* crec,
                           DbrJd settl_day, const char* ref, int action, DbrTicks ticks,
                           DbrLots lots, DbrLots min_lots) nogil

    DbrIden dbr_clnt_revise_id(DbrClnt clnt, DbrAccnt user, DbrIden id, DbrLots lots) nogil

    DbrIden dbr_clnt_revise_ref(DbrClnt clnt, DbrAccnt user, const char* ref,
                                DbrLots lots) nogil

    DbrIden dbr_clnt_cancel_id(DbrClnt clnt, DbrAccnt user, DbrIden id) nogil

    DbrIden dbr_clnt_cancel_ref(DbrClnt clnt, DbrAccnt user, const char* ref) nogil

    DbrIden dbr_clnt_ack_trade(DbrClnt clnt, DbrAccnt user, DbrIden id) nogil

    DbrBool dbr_clnt_is_ready(DbrClnt clnt) nogil

    DbrIden dbr_clnt_settimer(DbrClnt clnt, DbrMillis absms) nogil

    void dbr_clnt_canceltimer(DbrClnt clnt, DbrIden id) nogil

    DbrView* dbr_clnt_view_entry(DbrRbNode* node) nogil

    DbrRbNode* dbr_clnt_find_view(DbrClnt clnt, DbrIden cid, DbrJd settl_day) nogil

    DbrRbNode* dbr_clnt_first_view(DbrClnt clnt) nogil

    DbrRbNode* dbr_clnt_last_view(DbrClnt clnt) nogil

    DbrBool dbr_clnt_empty_view(DbrClnt clnt) nogil

    const unsigned char* dbr_clnt_uuid(DbrClnt clnt) nogil

cdef extern from "dbr/handler.h":

    ctypedef struct DbrHandlerVtbl

    ctypedef struct DbrIHandler:
        const DbrHandlerVtbl* vtbl

    ctypedef DbrIHandler* DbrHandler

    ctypedef struct DbrHandlerVtbl:

        void on_close(DbrHandler handler, DbrClnt clnt) nogil

        void on_ready(DbrHandler handler, DbrClnt clnt) nogil

        void on_logon(DbrHandler handler, DbrClnt clnt, DbrIden req_id, DbrIden uid) nogil

        void on_logoff(DbrHandler handler, DbrClnt clnt, DbrIden req_id, DbrIden uid) nogil

        void on_reset(DbrHandler handler, DbrClnt clnt) nogil

        void on_timeout(DbrHandler handler, DbrClnt clnt, DbrIden req_id) nogil

        void on_status(DbrHandler handler, DbrClnt clnt, DbrIden req_id, int num,
                       const char* msg) nogil

        void on_exec(DbrHandler handler, DbrClnt clnt, DbrIden req_id, DbrExec* exc) nogil

        void on_posn(DbrHandler handler, DbrClnt clnt, DbrPosn* posn) nogil

        void on_view(DbrHandler handler, DbrClnt clnt, DbrView* view) nogil

        void on_flush(DbrHandler handler, DbrClnt clnt) nogil

        void* on_async(DbrHandler handler, DbrClnt clnt, void* val) nogil

cdef extern from "dbr/dispatch.h":

    int dbr_clnt_dispatch(DbrClnt clnt, DbrMillis ms, DbrHandler handler) nogil

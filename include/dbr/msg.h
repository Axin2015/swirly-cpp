#ifndef DBR_MSG_H
#define DBR_MSG_H

#include <dbr/pool.h>
#include <dbr/types.h>

enum {
	DBR_INSERT_ORDER = 1,
	DBR_UPDATE_ORDER,
	DBR_ARCHIVE_ORDER,
	DBR_INSERT_TRADE,
	DBR_ARCHIVE_TRADE
};

struct DbrStmt {
    /**
     * @publicsection
     */
    int type;
    union {
        struct {
            DbrIden id;
            int rev;
            int status;
            DbrIden tid;
            DbrIden aid;
            DbrIden cid;
            DbrDate settl_date;
            DbrRef ref;
            int action;
            DbrTicks ticks;
            DbrLots resd;
            DbrLots exec;
            DbrLots lots;
            DbrLots min;
            DbrFlags flags;
            DbrMillis created;
            DbrMillis modified;
        } insert_order;
        struct {
            DbrIden id;
            int rev;
            int status;
            DbrLots resd;
            DbrLots exec;
            DbrLots lots;
            DbrMillis now;
        } update_order;
        struct {
            DbrIden id;
            DbrMillis now;
        } archive_order;
        struct {
            DbrIden id;
            DbrIden match;
            DbrIden order;
            int order_rev;
            DbrIden tid;
            DbrIden aid;
            DbrIden cid;
            DbrDate settl_date;
            DbrRef ref;
            DbrIden cpty;
            int role;
            int action;
            DbrTicks ticks;
            DbrLots resd;
            DbrLots exec;
            DbrLots lots;
            DbrMillis created;
            DbrMillis modified;
        } insert_trade;
        struct {
            DbrIden id;
            DbrMillis now;
        } archive_trade;
    };
    /**
     * @privatesection
     */
    // Singly-linked for transaction.
    struct DbrSlNode trans_node_;
};

static inline void
dbr_stmt_init(struct DbrStmt* stmt)
{
    dbr_slnode_init(&stmt->trans_node_);
}

enum {
    DBR_READ_ENTITY_REQ = 1,
    DBR_READ_ENTITY_REP,
    DBR_WRITE_TRANS_REQ,
    DBR_WRITE_TRANS_REP
};

struct DbrMsg {
	int type;
	union {
        struct {
            int type;
        } read_entity_req;
        struct {
            int type;
            // Set by dbr_msg_len();
            size_t count;
            struct DbrSlNode* first;
        } read_entity_rep;
    };
};

DBR_EXTERN DbrBool
dbr_recv_msg(void* sock, DbrPool pool, struct DbrMsg* msg);

DBR_EXTERN DbrBool
dbr_send_msg(void* sock, struct DbrMsg* msg);

#endif // DBR_MSG_H

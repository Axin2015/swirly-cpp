#ifndef DBR_MSG_H
#define DBR_MSG_H

#include <dbr/pool.h>
#include <dbr/types.h>

enum {
    DBR_READ_ENTITY_REQ = 1,
    DBR_READ_ENTITY_REP
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

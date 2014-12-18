/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef ELM_MSG_H
#define ELM_MSG_H

#include <dbr/elm/msg.h>

DBR_EXTERN size_t
elm_body_len(struct DbrBody* body, DbrBool enriched);

DBR_EXTERN char*
elm_write_body(char* buf, const struct DbrBody* body, DbrBool enriched);

DBR_EXTERN const char*
elm_read_body(const char* buf, DbrPool pool, struct DbrBody* body);

DBR_EXTERN DbrBool
elm_send_body(void* sock, struct DbrBody* body, DbrBool enriched);

DBR_EXTERN DbrBool
elm_send_msg(void* sock, const DbrUuid uuid, struct DbrBody* body, DbrBool enriched);

DBR_EXTERN DbrBool
elm_recv_body(void* sock, DbrPool pool, struct DbrBody* body);

DBR_EXTERN DbrBool
elm_recv_msg(void* sock, DbrPool pool, struct DbrMsg* msg);

#endif // ELM_MSG_H

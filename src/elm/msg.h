/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef ELM_MSG_H
#define ELM_MSG_H

#include <sc/elm/msg.h>

SC_EXTERN size_t
elm_body_len(struct ScBody* body, ScBool enriched);

SC_EXTERN char*
elm_write_body(char* buf, const struct ScBody* body, ScBool enriched);

SC_EXTERN const char*
elm_read_body(const char* buf, ScPool pool, struct ScBody* body);

SC_EXTERN ScBool
elm_send_body(void* sock, struct ScBody* body, ScBool enriched);

SC_EXTERN ScBool
elm_send_msg(void* sock, const ScUuid uuid, struct ScBody* body, ScBool enriched);

SC_EXTERN ScBool
elm_recv_body(void* sock, ScPool pool, struct ScBody* body);

SC_EXTERN ScBool
elm_recv_msg(void* sock, ScPool pool, struct ScMsg* msg);

#endif // ELM_MSG_H

/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
 *
 *  This file is part of Doobry written by Mark Aylett.
 *
 *  Doobry is free software; you can redistribute it and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Doobry is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program; if
 *  not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA.
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

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
#include <dbr/json.h>

#include <dbr/format.h>
#include <dbr/types.h>

#include <assert.h>
#include <string.h>

DBR_API size_t
dbr_json_accnt_len(const struct DbrRec* rec)
{
    assert(rec->type == DBR_ENTITY_ACCNT);
    enum {
        ACCNT_SIZE =
        sizeof("{\"mnem\":\"\","
               "\"display\":\"\","
               "\"email\":\"\"}") - 1
    };

    return ACCNT_SIZE
        + strnlen(rec->mnem, DBR_MNEM_MAX)
        + strnlen(rec->display, DBR_DISPLAY_MAX)
        + strnlen(rec->accnt.email, DBR_EMAIL_MAX);
}

DBR_API char*
dbr_json_write_accnt(char* buf, const struct DbrRec* rec)
{
    assert(rec->type == DBR_ENTITY_ACCNT);
    static const char ACCNT_FORMAT[] =
        "{\"mnem\":\"%m\","
        "\"display\":\"%s\","
        "\"email\":\"%s\"}";

    return dbr_format(buf, ACCNT_FORMAT,
                      rec->mnem,
                      DBR_DISPLAY_MAX, rec->display,
                      DBR_EMAIL_MAX, rec->accnt.email);
}

DBR_API size_t
dbr_json_contr_len(const struct DbrRec* rec)
{
    return 0;
}

DBR_API char*
dbr_json_write_contr(char* buf, const struct DbrRec* rec)
{
    return NULL;
}

DBR_API size_t
dbr_json_memb_len(const struct DbrMemb* memb, DbrBool enriched)
{
    return 0;
}

DBR_API char*
dbr_json_write_memb(char* buf, const struct DbrMemb* memb, DbrBool enriched)
{
    return NULL;
}

DBR_API size_t
dbr_json_order_len(const struct DbrOrder* order, DbrBool enriched)
{
    return 0;
}

DBR_API char*
dbr_json_write_order(char* buf, const struct DbrOrder* order, DbrBool enriched)
{
    return NULL;
}

DBR_API size_t
dbr_json_exec_len(const struct DbrExec* exec, DbrBool enriched)
{
    return 0;
}

DBR_API char*
dbr_json_write_exec(char* buf, const struct DbrExec* exec, DbrBool enriched)
{
    return NULL;
}

DBR_API size_t
dbr_json_posn_len(const struct DbrPosn* posn, DbrBool enriched)
{
    return 0;
}

DBR_API char*
dbr_json_write_posn(char* buf, const struct DbrPosn* posn, DbrBool enriched)
{
    return NULL;
}

DBR_API size_t
dbr_json_view_len(const struct DbrView* view, DbrBool enriched)
{
    return 0;
}

DBR_API char*
dbr_json_write_view(char* buf, const struct DbrView* view, DbrBool enriched)
{
    return NULL;
}

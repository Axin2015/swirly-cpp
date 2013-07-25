/*
 *  Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
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
#ifndef DBR_MSG_H
#define DBR_MSG_H

#include <dbr/defs.h>

struct DbrRec;
struct DbrOrder;
struct DbrMemb;
struct DbrTrade;
struct DbrPosn;

/**
 * @addtogroup Msg
 * @{
 */

DBR_API char*
dbr_writeinstr(char* buf, const struct DbrRec* rec);

DBR_API const char*
dbr_readinstr(const char* buf, struct DbrRec* rec);

DBR_API int
dbr_instrlen(const struct DbrRec* rec);

DBR_API char*
dbr_writemarket(char* buf, const struct DbrRec* rec);

DBR_API const char*
dbr_readmarket(const char* buf, struct DbrRec* rec);

DBR_API int
dbr_marketlen(const struct DbrRec* rec);

DBR_API char*
dbr_writetrader(char* buf, const struct DbrRec* rec);

DBR_API const char*
dbr_readtrader(const char* buf, struct DbrRec* rec);

DBR_API int
dbr_traderlen(const struct DbrRec* rec);

DBR_API char*
dbr_writeaccnt(char* buf, const struct DbrRec* rec);

DBR_API const char*
dbr_readaccnt(const char* buf, struct DbrRec* rec);

DBR_API int
dbr_accntlen(const struct DbrRec* rec);

DBR_API char*
dbr_writerec(char* buf, const struct DbrRec* rec);

DBR_API const char*
dbr_readrec(const char* buf, struct DbrRec* rec);

DBR_API int
dbr_reclen(const struct DbrRec* rec);

DBR_API char*
dbr_writeorder(char* buf, const struct DbrOrder* order);

DBR_API const char*
dbr_readorder(const char* buf, struct DbrOrder* order);

DBR_API int
dbr_orderlen(const struct DbrOrder* order);

DBR_API char*
dbr_writememb(char* buf, const struct DbrMemb* memb);

DBR_API const char*
dbr_readmemb(const char* buf, struct DbrMemb* memb);

DBR_API int
dbr_memblen(const struct DbrMemb* memb);

DBR_API char*
dbr_writetrade(char* buf, const struct DbrTrade* trade);

DBR_API const char*
dbr_readtrade(const char* buf, struct DbrTrade* trade);

DBR_API int
dbr_tradelen(const struct DbrTrade* trade);

DBR_API char*
dbr_writeposn(char* buf, const struct DbrPosn* posn);

DBR_API const char*
dbr_readposn(const char* buf, struct DbrPosn* posn);

DBR_API int
dbr_posnlen(const char* buf, const struct DbrPosn* posn);

/** @} */

#endif // DBR_MSG_H

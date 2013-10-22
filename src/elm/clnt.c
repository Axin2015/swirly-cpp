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
#include <dbr/clnt.h>

#include <dbr/err.h>

#include <stdlib.h> // malloc()

struct ElmClnt {
    int i;
};

DBR_API DbrClnt
dbr_clnt_create(void)
{
    DbrClnt clnt = malloc(sizeof(struct ElmClnt));
    if (dbr_unlikely(!clnt)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }
    return clnt;
 fail1:
    return NULL;
}

DBR_API void
dbr_clnt_destroy(DbrClnt clnt)
{
    if (clnt) {
        free(clnt);
    }
}

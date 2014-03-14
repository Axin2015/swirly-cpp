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
#include "config.h"

#include <dbr/err.h>
#include <dbr/util.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

struct Tok {
    char* begin;
    char* end;
};

DBR_EXTERN DbrBool
parse_line(char* begin, char* end, int line, struct Config* config)
{
    begin = (char*)dbr_ltrim(begin, end);

    if (*begin == '\0' || *begin == '#')
        return DBR_TRUE;

    char* sep = strchr(begin, '=');
    if (!sep) {
        dbr_err_setf(DBR_EINVAL, "missing separator at line %d", line);
        return DBR_FALSE;
    }

    struct Tok key = { .begin = begin, .end = sep };
    struct Tok val = { .begin = sep + 1, .end = end };

    key.end = (char*)dbr_rtrim(key.begin, key.end);

    val.begin = (char*)dbr_ltrim(val.begin, val.end);
    val.end = (char*)dbr_rtrim(val.begin, val.end);

    // Null terminated strings simplify lexical conversions.
    *key.end = '\0';
    *val.end = '\0';

    if (strcmp(key.begin, "x") == 0) {
        config->x = atoi(val.begin);
    } else {
        dbr_err_setf(DBR_EINVAL, "invalid key '%s' at line %d", key.begin, line);
        return DBR_FALSE;
    }
    return DBR_TRUE;
}

DBR_EXTERN DbrBool
parse_stream(FILE* stream, struct Config* config)
{
    char* buf = NULL;
    size_t capacity = 0;
    int line = 0;
    for (;;) {
        const ssize_t len = getline(&buf, &capacity, stdin);
        if (len == -1)
            break;
        if (!parse_line(buf, buf + len, ++line, config))
            goto fail1;
    }
    free(buf);
    return DBR_TRUE;
 fail1:
    free(buf);
    return DBR_FALSE;
}

DBR_EXTERN DbrBool
parse_file(const char* path, struct Config* config)
{
    DbrBool ret;
    FILE* file = fopen(path, "r");
    if (file) {
        ret = parse_stream(file, config);
        fclose(file);
    } else {
        dbr_err_setf(DBR_EIO, "fopen() failed: %s", strerror(errno));
        ret = DBR_FALSE;
    }
    return ret;
}

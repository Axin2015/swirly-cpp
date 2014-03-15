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

static int
parse_bool(const char* begin, const char* end, int line)
{
    int ret;
    switch (end - begin) {
    case 1:
        // 0 or 1.
        switch (*begin) {
        case '0':
            ret = DBR_FALSE;
            break;
        case '1':
            ret = DBR_TRUE;
            break;
        default:
            goto fail1;
        }
        break;
    case 2:
        // no.
        if (strcmp(begin, "no") != 0)
            goto fail1;
        ret = DBR_FALSE;
        break;
    case 3:
        // yes.
        if (strcmp(begin, "yes") != 0)
            goto fail1;
        ret = DBR_TRUE;
        break;
    case 4:
        // true.
        if (strcmp(begin, "true") != 0)
            goto fail1;
        ret = DBR_TRUE;
        break;
    case 5:
        // false.
        if (strcmp(begin, "false") != 0)
            goto fail1;
        ret = DBR_FALSE;
        break;
    default:
        goto fail1;
    }
    return ret;
 fail1:
    dbr_err_setf(DBR_EINVAL, "invalid boolean value '%s' at line %d", begin, line);
    return -1;
}

DBR_EXTERN DbrBool
parse_line(char* begin, char* end, int line, struct Config* config)
{
    begin = (char*)dbr_ltrim(begin, end);

    if (*begin == '\0' || *begin == '#')
        return DBR_TRUE;

    char* sep = strchr(begin, '=');
    if (!sep) {
        dbr_err_setf(DBR_EINVAL, "missing separator at line %d", line);
        goto fail1;
    }

    struct Tok key = { .begin = begin, .end = sep };
    struct Tok val = { .begin = sep + 1, .end = end };

    key.end = (char*)dbr_rtrim(key.begin, key.end);

    val.begin = (char*)dbr_ltrim(val.begin, val.end);
    val.end = (char*)dbr_rtrim(val.begin, val.end);

    // Null terminated strings simplify lexical conversions.
    *key.end = '\0';
    *val.end = '\0';

    if (strcmp(key.begin, "daemon") == 0) {
        const int ret = parse_bool(val.begin, val.end, line);
        if (ret < 0)
            goto fail1;
        config->daemon = ret;
    } else {
        dbr_err_setf(DBR_EINVAL, "invalid key '%s' at line %d", key.begin, line);
        goto fail1;
    }
    return DBR_TRUE;
 fail1:
    return DBR_FALSE;
}

DBR_EXTERN DbrBool
parse_stream(FILE* stream, struct Config* config)
{
    char* buf = NULL;
    size_t capacity = 0;
    int line = 0;
    for (;;) {
        const ssize_t len = getline(&buf, &capacity, stream);
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

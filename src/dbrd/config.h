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
#ifndef CONFIG_H
#define CONFIG_H

#include <dbr/ash/defs.h>

#include <limits.h> // PATH_MAX
#include <stdio.h>

struct Config {
    DbrBool daemon;

    char prefix[PATH_MAX + 1];
    char bankfile[PATH_MAX + 1];
    char dbfile[PATH_MAX + 1];
    char logfile[PATH_MAX + 1];

    int journsize;
    int poolsize;
};

DBR_EXTERN DbrBool
parse_line(char* begin, char* end, int line, struct Config* config);

DBR_EXTERN DbrBool
parse_stream(FILE* stream, struct Config* config);

DBR_EXTERN DbrBool
parse_file(const char* path, struct Config* config);

#endif // CONFIG_H

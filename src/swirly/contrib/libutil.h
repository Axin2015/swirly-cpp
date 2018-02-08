/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#ifndef SWIRLY_CONTRIB_LIBUTIL_H
#define SWIRLY_CONTRIB_LIBUTIL_H

/* See https://github.com/LuaDist/libbsd */

static const char* getprogname(void) { return program_invocation_short_name; }

struct pidfh {
    int pf_fd;
    char* pf_path;
    dev_t pf_dev;
    ino_t pf_ino;
};

int flopen(const char *path, int flags, ...);
int pidfile_write(struct pidfh* pfh);
int pidfile_close(struct pidfh* pfh);
int pidfile_remove(struct pidfh* pfh);

#endif // SWIRLY_CONTRIB_LIBUTIL_H

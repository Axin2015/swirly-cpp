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
#include <dbr/store.h>

#include <dbr/err.h>

#include <errno.h>
#include <stdlib.h> // free()
#include <string.h> // strerror()

#include <sys/types.h>
#include <sys/mman.h>

#include <fcntl.h>  // open()
#include <unistd.h> // close()

DBR_API void
dbr_store_term(struct DbrStore* store)
{
    if (store->fd >= 0) {
        if (munmap(store->arr, store->len * sizeof(long)) < 0) {
            dbr_err_setf(DBR_EIO, "munmap() failed: %s", strerror(errno));
            dbr_err_print();
        }
        close(store->fd);
    } else
        free(store->arr);
}

DBR_API DbrBool
dbr_store_init(struct DbrStore* store, const char* path, size_t len)
{
    int fd;
    void* arr;
    if (path != NULL) {
        fd = open(path, O_RDWR | O_CREAT, (mode_t)0600);
        if (fd < 0) {
            dbr_err_setf(DBR_EIO, "open() failed: %s", strerror(errno));
            goto fail1;
        }
        if (ftruncate(fd, len * sizeof(long)) < 0) {
            dbr_err_setf(DBR_EIO, "ftruncate() failed: %s", strerror(errno));
            close(fd);
            goto fail1;
        }
        arr = mmap(NULL, len * sizeof(long), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (arr == MAP_FAILED) {
            dbr_err_setf(DBR_EIO, "mmap() failed: %s", strerror(errno));
            close(fd);
            goto fail1;
        }
    } else {
        fd = -1;
        arr = malloc(len * sizeof(long));
        if (!arr) {
            dbr_err_set(DBR_ENOMEM, "out of memory");
            goto fail1;
        }
    }
    store->fd = fd;
    store->len = len;
    store->arr = arr;
    return DBR_TRUE;
 fail1:
    return DBR_FALSE;
}

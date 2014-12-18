/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <dbr/ash/bank.h>

#include <dbr/ash/err.h>

#include <errno.h>
#include <stdlib.h> // free()
#include <string.h> // strerror()

#include <sys/types.h>
#include <sys/mman.h>

#include <fcntl.h>  // open()
#include <unistd.h> // close()

DBR_API void
dbr_bank_term(struct DbrBank* bank)
{
    if (bank->fd >= 0) {
        if (munmap(bank->arr, bank->len * sizeof(long)) < 0)
            dbr_err_printf(DBR_ESYSTEM, "munmap() failed: %s", strerror(errno));
        close(bank->fd);
    } else
        free(bank->arr);
}

DBR_API DbrBool
dbr_bank_init(struct DbrBank* bank, const char* path, size_t len)
{
    int fd;
    long* arr;
    if (path != NULL) {
        fd = open(path, O_RDWR | O_CREAT, (mode_t)0664);
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
    bank->fd = fd;
    bank->len = len;
    bank->arr = arr;
    return DBR_TRUE;
 fail1:
    return DBR_FALSE;
}

/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_ASH_DAEMON_H
#define DBR_ASH_DAEMON_H

#include <dbr/ash/defs.h>

#include <sys/types.h> // mode_t

/**
 * Run program as daemon process.
 * @addtogroup Daemon
 * @{
 */

DBR_API DbrBool
dbr_daemon(const char* wd, mode_t mask);

/** @} */

#endif // DBR_ASH_DAEMON_H

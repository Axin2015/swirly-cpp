/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_DAEMON_H
#define SC_ASH_DAEMON_H

#include <sc/ash/defs.h>

#include <sys/types.h> // mode_t

/**
 * Run program as daemon process.
 * @addtogroup Daemon
 * @{
 */

SC_API ScBool
sc_daemon(const char* wd, mode_t mask);

/** @} */

#endif // SC_ASH_DAEMON_H

/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "test.hpp"

#include <dbr/ash/err.h>

TEST_CASE(err_clear)
{
    dbr_err_clear();
    check(dbr_err_num() == 0);
    check(*dbr_err_msg() == '\0');
}

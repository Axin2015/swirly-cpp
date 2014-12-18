/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "test.hpp"

#include <sc/ash/err.h>

TEST_CASE(err_clear)
{
    sc_err_clear();
    check(sc_err_num() == 0);
    check(*sc_err_msg() == '\0');
}

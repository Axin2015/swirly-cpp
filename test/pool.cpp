/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "test.hpp"

#include <dbrpp/elm/pool.hpp>

using namespace dbr;

TEST_CASE(pool_create)
{
    Pool pool(8 * 1024 * 1024);
}

TEST_CASE(pool_destroy)
{
    dbr_pool_destroy(NULL);
}

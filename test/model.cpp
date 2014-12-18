/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "mock.hpp"
#include "test.hpp"

#include <dbrpp/elm/pool.hpp>

#include <dbr/fig/accnt.h>

#include <algorithm> // find_if()

using namespace dbr;

TEST_CASE(model_accnt)
{
    Model model;
    Pool pool(8 * 1024 * 1024);
    auto recs = read_entity<DBR_ENTITY_ACCNT>(&model, pool);
    auto it = std::find_if(recs.begin(), recs.end(), [](const DbrRec& rec) {
            return strncmp(rec.mnem, "DBRA", DBR_MNEM_MAX) == 0;
        });
    check(it != recs.end());
}

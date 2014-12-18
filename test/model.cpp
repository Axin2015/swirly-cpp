/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "mock.hpp"
#include "test.hpp"

#include <scpp/elm/pool.hpp>

#include <sc/fig/accnt.h>

#include <algorithm> // find_if()

using namespace sc;

TEST_CASE(model_accnt)
{
    Model model;
    Pool pool(8 * 1024 * 1024);
    auto recs = read_entity<SC_ENTITY_ACCNT>(&model, pool);
    auto it = std::find_if(recs.begin(), recs.end(), [](const ScRec& rec) {
            return strncmp(rec.mnem, "SCA", SC_MNEM_MAX) == 0;
        });
    check(it != recs.end());
}

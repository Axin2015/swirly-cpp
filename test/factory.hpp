 /*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef FACTORY_HPP
#define FACTORY_HPP

#include <sc/elm/types.h>

#include <memory>

namespace sc {
class Pool;
}

std::shared_ptr<ScRec>
create_accnt(sc::Pool& pool, ScIden id, const char* mnem, const char* display,
             const char* email);

std::shared_ptr<ScRec>
create_contr(sc::Pool& pool, ScIden id, const char* mnem, const char* display,
             const char* asset_type, const char* asset, const char* ccy, int tick_numer,
             int tick_denom, int lot_numer, int lot_denom, int pip_dp, ScLots min_lots,
             ScLots max_lots);

std::shared_ptr<ScPerm>
create_perm(sc::Pool& pool, ScIden tid, ScIden gid);

std::shared_ptr<ScOrder>
create_order(sc::Pool& pool, ScIden id, ScRec& trader, ScRec& giveup, ScRec& contr,
             ScJd settl_day, const char* ref, int action, ScTicks ticks, ScLots lots,
             ScLots min_lots, ScMillis now);

std::shared_ptr<ScOrder>
create_order(sc::Pool& pool, ScIden id, ScIden tid, ScIden gid, ScIden cid,
             ScJd settl_day, const char* ref, int action, ScTicks ticks, ScLots lots,
             ScLots min_lots, ScMillis now);

std::shared_ptr<ScExec>
create_trade(sc::Pool& pool, ScIden id, ScIden order, ScRec& trader, ScRec& giveup,
             ScRec& contr, ScJd settl_day, const char* ref, int action, ScTicks ticks,
             ScLots lots, ScLots resd, ScLots exec, ScTicks last_ticks, ScLots last_lots,
             ScIden match, int role, ScRec& cpty, ScMillis now);

std::shared_ptr<ScExec>
create_trade(sc::Pool& pool, ScIden id, ScIden order, ScIden tid, ScIden gid,
             ScIden cid, ScJd settl_day, const char* ref, int action, ScTicks ticks,
             ScLots lots, ScLots resd, ScLots exec, ScTicks last_ticks, ScLots last_lots,
             ScIden match, int role, ScIden cpty, ScMillis now);

inline std::shared_ptr<ScRec>
create_wramirez(sc::Pool& pool)
{
    return create_accnt(pool, 1, "WRAMIREZ", "Wayne Ramirez", "wayne.ramirez@swirlycloud.com");
}

inline std::shared_ptr<ScRec>
create_sflores(sc::Pool& pool)
{
    return create_accnt(pool, 2, "SFLORES", "Steven Flores", "steven.flores@swirlycloud.com");
}

inline std::shared_ptr<ScRec>
create_sca(sc::Pool& pool)
{
    return create_accnt(pool, 3, "SCA", "Account A", "sca@swirlycloud.com");
}

inline std::shared_ptr<ScRec>
create_scb(sc::Pool& pool)
{
    return create_accnt(pool, 4, "SCB", "Account B", "scb@swirlycloud.com");
}

inline std::shared_ptr<ScRec>
create_eurusd(sc::Pool& pool)
{
    return create_contr(pool, 1, "EURUSD", "EURUSD", "CURRENCY", "EUR", "USD",
                        1, 10000, 1000000, 1, 4, 1, 10);
}

inline std::shared_ptr<ScRec>
create_gbpusd(sc::Pool& pool)
{
    return create_contr(pool, 2, "GBPUSD", "GBPUSD", "CURRENCY", "GBP", "USD",
                        1, 10000, 1000000, 1, 4, 1, 10);
}

#endif // FACTORY_HPP

/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef FIG_MATCH_H
#define FIG_MATCH_H

#include <dbr/elm/types.h>

struct DbrBank;
struct FigOrdIdx;

// Used by matching engine to build a list of matches.

// Match taker with list of maker orders.

DBR_EXTERN DbrBool
fig_match_orders(struct DbrBook* book, struct DbrOrder* taker, struct DbrBank* bank,
                 struct FigOrdIdx* ordidx, DbrPool pool, struct DbrTrans* trans);

#endif // FIG_MATCH_H

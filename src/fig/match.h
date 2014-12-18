/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef FIG_MATCH_H
#define FIG_MATCH_H

#include <sc/elm/types.h>

struct ScBank;
struct FigOrdIdx;

// Used by matching engine to build a list of matches.

// Match taker with list of maker orders.

SC_EXTERN ScBool
fig_match_orders(struct ScBook* book, struct ScOrder* taker, struct ScBank* bank,
                 struct FigOrdIdx* ordidx, ScPool pool, struct ScTrans* trans);

#endif // FIG_MATCH_H

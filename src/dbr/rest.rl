// -*- c -*-
/**
 *  Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
 *
 *  This file is part of Doobry written by Mark Aylett.
 *
 *  Doobry is free software; you can redistribute it and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Doobry is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program; if
 *  not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA.
 */
#include <dbr/rest.h>

#include <math.h> // pow()

static long
inum(const struct DbrRest* rest)
{
    return rest->num.sign * rest->num.idigits;
}

#if 0
static double
fnum(const struct DbrRest* rest)
{
    const double fract = rest->num.fdigits / pow(10, rest->num.fcount);
    return rest->num.sign * (rest->num.idigits + fract);
}
#endif

%%{
    machine json;

    action begin_num {
        rest->num.sign = 1;
        rest->num.idigits = 0;
        rest->num.fdigits = 0;
        rest->num.fcount = 0;
    }
    action negate {
        rest->num.sign = -1;
    }
    action add_idigit {
        rest->num.idigits = rest->num.idigits * 10 + (fc - '0');
    }
    action add_fdigit {
        rest->num.fdigits = rest->num.fdigits * 10 + (fc - '0');
        ++rest->num.fcount;
    }

    inum = (
        start: (
            '-' @negate ->req_idigit
          | '+' ->req_idigit
          | [0-9] @add_idigit ->opt_idigit
        ),
        req_idigit: (
            [0-9] @add_idigit ->opt_idigit
        ),
        opt_idigit: (
            [0-9] @add_idigit ->opt_idigit
          | '' -> final
        )
    ) >begin_num;

    fnum = (
        start: (
            '-' @negate ->req_idigit
          | '+' ->req_idigit
          | [0-9] @add_idigit ->opt_idigit
          | '.' -> req_fdigit
        ),
        req_idigit: (
            [0-9] @add_idigit ->opt_idigit
          | '.' -> req_fdigit
        ),
        opt_idigit: (
            [0-9] @add_idigit ->opt_idigit
          | '.' -> opt_fdigit
          | '' ->final
        ),
        req_fdigit: (
            [0-9] @add_fdigit ->opt_fdigit
        ),
        opt_fdigit: (
            [0-9] @add_fdigit ->opt_fdigit
          | '' ->final
        )
    ) >begin_num;

    action begin_str {
        rest->str.count = 0;
    }
    action add_char {
        if (rest->str.count < rest->str.max)
            rest->str.buf[rest->str.count++] = fc;
        else {
            DBR_DEBUG1F("exceed max string length of %ld", (long)rest->str.max);
            cs = json_error;
            fbreak;
        }
    }
    action end_str {
        if (rest->str.count < rest->str.max)
            rest->str.buf[rest->str.count] = '\0';
    }
    str = (
        start: (
            '"' ->char
        ),
        char: (
            [^"\\] @add_char -> char
          | '\\' any @add_char ->char
          | '"' ->final
        )
    ) >begin_str %end_str;

    action delete_method {
        rest->func = (rest->func & ~DBR_METHOD_MASK) | DBR_DELETE;
    }
    action get_method {
        rest->func = (rest->func & ~DBR_METHOD_MASK) | DBR_GET;
    }
    action head_method {
        rest->func = (rest->func & ~DBR_METHOD_MASK) | DBR_HEAD;
    }
    action post_method {
        rest->func = (rest->func & ~DBR_METHOD_MASK) | DBR_POST;
    }
    action put_method {
        rest->func = (rest->func & ~DBR_METHOD_MASK) | DBR_PUT;
    }
    method  = '"delete"'i %delete_method
            | '"get"'i %get_method
            | '"head"'i %head_method
            | '"post"'i %post_method
            | '"put"'i %put_method;

    action instr_entity {
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_INSTR;
    }
    action market_entity {
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_MARKET;
    }
    action trader_entity {
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_TRADER;
    }
    action accnt_entity {
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_ACCNT;
    }
    action order_entity {
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_ORDER;
    }
    action trade_entity {
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_TRADE;
    }
    action posn_entity {
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_POSN;
    }
    entity  = '"instr"'i %instr_entity
            | '"market"'i %market_entity
            | '"trader"'i %trader_entity
            | '"accnt"'i %accnt_entity
            | '"order"'i %order_entity
            | '"trade"'i %trade_entity
            | '"posn"'i %posn_entity;

    action buy_action {
        rest->action = DBR_BUY;
    }
    action sell_action {
        rest->action = DBR_SELL;
    }
    action_ = '"buy"'i %buy_action
            | '"sell"'i %sell_action;

    colon = space* ':' space*;
    comma = space* ',' space*;

    action begin_method {
        if ((rest->func & DBR_METHOD_MASK) != 0) {
            DBR_DEBUG1("method already specified");
            cs = json_error;
            fbreak;
        }
    }
    action begin_entity {
        if ((rest->func & DBR_ENTITY_MASK) != 0) {
            DBR_DEBUG1("entity already specified");
            cs = json_error;
            fbreak;
        }
    }
    action begin_id {
        if (rest->func & DBR_ID_PARAM) {
            DBR_DEBUG1("id already specified");
            cs = json_error;
            fbreak;
        }
    }
    action end_id {
        rest->func |= DBR_ID_PARAM;
        rest->id = inum(rest);
    }
    action begin_mnem {
        if (rest->func & DBR_MNEM_PARAM) {
            DBR_DEBUG1("mnem already specified");
            cs = json_error;
            fbreak;
        }
        rest->str.buf = rest->mnem;
        rest->str.max = DBR_MNEM_MAX;
    }
    action end_mnem {
        rest->func |= DBR_MNEM_PARAM;
    }
    action begin_market {
        if (rest->func & DBR_MARKET_PARAM) {
            DBR_DEBUG1("market already specified");
            cs = json_error;
            fbreak;
        }
        rest->str.buf = rest->market;
        rest->str.max = DBR_MNEM_MAX;
    }
    action end_market {
        rest->func |= DBR_MARKET_PARAM;
    }
    action begin_trader {
        if (rest->func & DBR_TRADER_PARAM) {
            DBR_DEBUG1("trader already specified");
            cs = json_error;
            fbreak;
        }
        rest->str.buf = rest->trader;
        rest->str.max = DBR_MNEM_MAX;
    }
    action end_trader {
        rest->func |= DBR_TRADER_PARAM;
    }
    action begin_accnt {
        if (rest->func & DBR_ACCNT_PARAM) {
            DBR_DEBUG1("accnt already specified");
            cs = json_error;
            fbreak;
        }
        rest->str.buf = rest->accnt;
        rest->str.max = DBR_MNEM_MAX;
    }
    action end_accnt {
        rest->func |= DBR_ACCNT_PARAM;
    }
    action begin_ref {
        if (rest->func & DBR_REF_PARAM) {
            DBR_DEBUG1("ref already specified");
            cs = json_error;
            fbreak;
        }
        rest->str.buf = rest->ref;
        rest->str.max = DBR_REF_MAX;
    }
    action end_ref {
        rest->func |= DBR_REF_PARAM;
    }
    action begin_action {
        if (rest->func & DBR_ACTION_PARAM) {
            DBR_DEBUG1("action already specified");
            cs = json_error;
            fbreak;
        }
    }
    action end_action {
        rest->func |= DBR_ACTION_PARAM;
    }
    action begin_ticks {
        if (rest->func & DBR_TICKS_PARAM) {
            DBR_DEBUG1("ticks already specified");
            cs = json_error;
            fbreak;
        }
    }
    action end_ticks {
        rest->func |= DBR_TICKS_PARAM;
        rest->ticks = inum(rest);
    }
    action begin_lots {
        if (rest->func & DBR_LOTS_PARAM) {
            DBR_DEBUG1("lots already specified");
            cs = json_error;
            fbreak;
        }
    }
    action end_lots {
        rest->func |= DBR_LOTS_PARAM;
        rest->lots = inum(rest);
    }
    pair = '"method"'i colon method >begin_method
         | '"entity"'i colon entity >begin_entity
         | '"id"'i colon inum >begin_id %end_id
         | '"mnem"'i colon str >begin_mnem %end_mnem
         | '"market"'i colon str >begin_market %end_market
         | '"trader"'i colon str >begin_trader %end_trader
         | '"accnt"'i colon str >begin_accnt %end_accnt
         | '"ref"'i colon str >begin_ref %end_ref
         | '"action"'i colon action_ >begin_action %end_action
         | '"ticks"'i colon inum >begin_ticks %end_ticks
         | '"lots"'i colon inum >begin_lots %end_lots;

    members = pair (comma pair)*;

    object = '{' space* '}'
           | '{' space* members space* '}';

    main := space* object space*;
}%%

%% write data;

DBR_API void
dbr_rest_init(struct DbrRest* rest)
{
    int cs;
    %% write init;
    rest->cs = cs;
    rest->func = 0;
}

DBR_API int
dbr_rest_json(struct DbrRest* rest, const char* buf, size_t size)
{
	const char* p = buf;
	const char* pe = p + size;

    int cs = rest->cs;
	%% write exec;
    rest->cs = cs;

	if (cs == json_error)
		return -1;

	if (cs < json_first_final)
		return 0;

	return 1;
}

%%{
    machine rurl;

    action begin_inum {
        inum = 0;
    }
    action add_idigit {
        inum = inum * 10 + (fc - '0');
    }
    inum = ([0-9]+ $add_idigit) >begin_inum;

    action begin_str {
        str.count = 0;
    }
    action add_char {
        if (str.count < str.max)
            str.buf[str.count++] = fc;
        else {
            DBR_DEBUG1F("exceed max string length of %ld", (long)rest->str.max);
            cs = rurl_error;
            fbreak;
        }
    }
    action end_str {
        if (str.count < str.max)
            str.buf[str.count] = '\0';
    }
    str = ([^/]+ $add_char) >begin_str %end_str;

    action begin_mnem {
        if (rest->func & DBR_MNEM_PARAM) {
            DBR_DEBUG1("mnem already specified");
            cs = rurl_error;
            fbreak;
        }
        str.buf = rest->mnem;
        str.max = DBR_MNEM_MAX;
    }
    action end_mnem {
        rest->func |= DBR_MNEM_PARAM;
    }
    opt_mnem = ('/' str) >begin_mnem %end_mnem
             | '';

    action begin_market {
        if (rest->func & DBR_MARKET_PARAM) {
            DBR_DEBUG1("market already specified");
            cs = rurl_error;
            fbreak;
        }
        str.buf = rest->market;
        str.max = DBR_MNEM_MAX;
    }
    action end_market {
        rest->func |= DBR_MARKET_PARAM;
    }
    opt_market = ('/' str) >begin_market %end_market
               | '';

    action begin_trader {
        if (rest->func & DBR_TRADER_PARAM) {
            DBR_DEBUG1("trader already specified");
            cs = rurl_error;
            fbreak;
        }
        str.buf = rest->trader;
        str.max = DBR_MNEM_MAX;
    }
    action end_trader {
        rest->func |= DBR_TRADER_PARAM;
    }
    opt_trader = ('/' str) >begin_trader %end_trader
               | '';
    req_trader = ('/' str) >begin_trader %end_trader;

    action begin_accnt {
        if (rest->func & DBR_ACCNT_PARAM) {
            DBR_DEBUG1("accnt already specified");
            cs = rurl_error;
            fbreak;
        }
        str.buf = rest->accnt;
        str.max = DBR_MNEM_MAX;
    }
    action end_accnt {
        rest->func |= DBR_ACCNT_PARAM;
    }
    opt_accnt = ('/' str) >begin_accnt %end_accnt
              | '';
    req_accnt = ('/' str) >begin_accnt %end_accnt;

    action begin_id {
        if (rest->func & DBR_ID_PARAM) {
            DBR_DEBUG1("id already specified");
            cs = rurl_error;
            fbreak;
        }
    }
    action end_id {
        rest->func |= DBR_ID_PARAM;
        rest->id = inum;
    }
    opt_id = ('/' inum) >begin_id %end_id
           | '';

    action instr_entity {
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_INSTR;
    }
    action market_entity {
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_MARKET;
    }
    action trader_entity {
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_TRADER;
    }
    action accnt_entity {
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_ACCNT;
    }
    action depth_entity {
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_DEPTH;
    }
    action order_entity {
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_ORDER;
    }
    action trade_entity {
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_TRADE;
    }
    action posn_entity {
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_POSN;
    }

    entity = ('instr' opt_mnem) %instr_entity
           | ('market' opt_mnem) %market_entity
           | ('trader' opt_mnem) %trader_entity
           | ('accnt' opt_mnem) %accnt_entity
           | ('depth' opt_mnem) %depth_entity
           | ('order' req_trader opt_id) %order_entity
           | ('trade' req_accnt opt_id) %trade_entity
           | ('posn' req_accnt) %posn_entity;

    action begin_entity {
        if ((rest->func & DBR_ENTITY_MASK) != 0) {
            DBR_DEBUG1("entity already specified");
            cs = rurl_error;
            fbreak;
        }
    }

    main := ([/] entity [/]?) >begin_entity;
}%%

%% write data;

DBR_API int
dbr_rest_rurl(struct DbrRest* rest, const char* buf, size_t size)
{
    int cs;
	const char* p = buf;
	const char* pe = p + size;
    const char* eof = pe;

    long inum;
    struct {
        char* buf;
        size_t count;
        size_t max;
    } str;

    %% write init;
    %% write exec;

	if (cs == rurl_error)
		return -1;

	if (cs < rurl_first_final)
		return 0;

	return 1;
}

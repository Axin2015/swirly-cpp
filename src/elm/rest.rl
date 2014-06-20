// -*- c -*-
/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
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
#include <dbr/elm/rest.h>

#include <dbr/ash/err.h>

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
            cs = json_error; msg = "max string length exceeded";
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
        rest->fields = (rest->fields & ~DBR_METHOD_MASK) | DBR_METHOD_DELETE;
    }
    action get_method {
        rest->fields = (rest->fields & ~DBR_METHOD_MASK) | DBR_METHOD_GET;
    }
    action head_method {
        rest->fields = (rest->fields & ~DBR_METHOD_MASK) | DBR_METHOD_HEAD;
    }
    action post_method {
        rest->fields = (rest->fields & ~DBR_METHOD_MASK) | DBR_METHOD_POST;
    }
    action put_method {
        rest->fields = (rest->fields & ~DBR_METHOD_MASK) | DBR_METHOD_PUT;
    }
    method  = '"delete"'i %delete_method
            | '"get"'i %get_method
            | '"head"'i %head_method
            | '"post"'i %post_method
            | '"put"'i %put_method;

    action logon_resrc {
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_LOGON;
    }
    action logoff_resrc {
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_LOGOFF;
    }
    action accnt_resrc {
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_ACCNT;
    }
    action contr_resrc {
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_CONTR;
    }
    action user_resrc {
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_USER;
    }
    action group_resrc {
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_GROUP;
    }
    action order_resrc {
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_ORDER;
    }
    action trade_resrc {
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_TRADE;
    }
    action posn_resrc {
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_POSN;
    }
    action market_resrc {
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_MARKET;
    }
    resrc  =  '"logon"'i %logon_resrc
            | '"logoff"'i %logoff_resrc
            | '"accnt"'i %accnt_resrc
            | '"contr"'i %contr_resrc
            | '"user"'i %user_resrc
            | '"group"'i %group_resrc
            | '"order"'i %order_resrc
            | '"trade"'i %trade_resrc
            | '"posn"'i %posn_resrc
            | '"market"'i %market_resrc;

    action buy_action {
        rest->action = DBR_ACTION_BUY;
    }
    action sell_action {
        rest->action = DBR_ACTION_SELL;
    }
    action_ = '"buy"'i %buy_action
            | '"sell"'i %sell_action;

    colon = space* ':' space*;
    comma = space* ',' space*;

    action begin_method {
        if ((rest->fields & DBR_METHOD_MASK) != 0) {
            cs = json_error; msg = "method already specified";
            fbreak;
        }
    }
    action begin_resrc {
        if ((rest->fields & DBR_RESRC_MASK) != 0) {
            cs = json_error; msg = "resrc already specified";
            fbreak;
        }
    }
    action begin_id {
        if (rest->fields & DBR_PARAM_ID) {
            cs = json_error; msg = "id already specified";
            fbreak;
        }
    }
    action end_id {
        rest->fields |= DBR_PARAM_ID;
        rest->id = inum(rest);
    }
    action begin_accnt {
        if (rest->fields & DBR_PARAM_ACCNT) {
            cs = json_error; msg = "accnt already specified";
            fbreak;
        }
        rest->str.buf = rest->accnt;
        rest->str.max = DBR_MNEM_MAX;
    }
    action end_accnt {
        rest->fields |= DBR_PARAM_ACCNT;
    }
    action begin_group {
        if (rest->fields & DBR_PARAM_GROUP) {
            cs = json_error; msg = "group already specified";
            fbreak;
        }
        rest->str.buf = rest->group;
        rest->str.max = DBR_MNEM_MAX;
    }
    action end_group {
        rest->fields |= DBR_PARAM_GROUP;
    }
    action begin_contr {
        if (rest->fields & DBR_PARAM_CONTR) {
            cs = json_error; msg = "contr already specified";
            fbreak;
        }
        rest->str.buf = rest->contr;
        rest->str.max = DBR_MNEM_MAX;
    }
    action end_contr {
        rest->fields |= DBR_PARAM_CONTR;
    }
    action begin_settl_date {
        if (rest->fields & DBR_PARAM_SETTL_DATE) {
            cs = json_error; msg = "settl-date already specified";
            fbreak;
        }
    }
    action end_settl_date {
        rest->fields |= DBR_PARAM_SETTL_DATE;
        rest->settl_date = inum(rest);
    }
    action begin_ref {
        if (rest->fields & DBR_PARAM_REF) {
            cs = json_error; msg = "ref already specified";
            fbreak;
        }
        rest->str.buf = rest->ref;
        rest->str.max = DBR_REF_MAX;
    }
    action end_ref {
        rest->fields |= DBR_PARAM_REF;
    }
    action begin_action {
        if (rest->fields & DBR_PARAM_ACTION) {
            cs = json_error; msg = "action already specified";
            fbreak;
        }
    }
    action end_action {
        rest->fields |= DBR_PARAM_ACTION;
        rest->action = inum(rest);
    }
    action begin_ticks {
        if (rest->fields & DBR_PARAM_TICKS) {
            cs = json_error; msg = "ticks already specified";
            fbreak;
        }
    }
    action end_ticks {
        rest->fields |= DBR_PARAM_TICKS;
        rest->ticks = inum(rest);
    }
    action begin_lots {
        if (rest->fields & DBR_PARAM_LOTS) {
            cs = json_error; msg = "lots already specified";
            fbreak;
        }
    }
    action end_lots {
        rest->fields |= DBR_PARAM_LOTS;
        rest->lots = inum(rest);
    }
    action begin_min_lots {
        if (rest->fields & DBR_PARAM_MIN_LOTS) {
            cs = json_error; msg = "min-lots already specified";
            fbreak;
        }
    }
    action end_min_lots {
        rest->fields |= DBR_PARAM_MIN_LOTS;
        rest->min_lots = inum(rest);
    }
    pair = '"method"'i colon method >begin_method
         | '"resrc"'i colon resrc >begin_resrc
         | '"id"'i colon inum >begin_id %end_id
         | '"accnt"'i colon str >begin_accnt %end_accnt
         | '"group"'i colon str >begin_group %end_group
         | '"contr"'i colon str >begin_contr %end_contr
         | '"settl_date"'i colon inum >begin_settl_date %end_settl_date
         | '"ref"'i colon str >begin_ref %end_ref
         | '"action"'i colon action_ >begin_action %end_action
         | '"ticks"'i colon inum >begin_ticks %end_ticks
         | '"lots"'i colon inum >begin_lots %end_lots
         | '"min_lots"'i colon inum >begin_min_lots %end_min_lots;

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
    rest->fields = 0;
}

DBR_API int
dbr_rest_json(struct DbrRest* rest, const char* buf, size_t size)
{
	const char* p = buf;
	const char* pe = p + size;
    const char* msg = "parse error";

    int cs = rest->cs;
	%% write exec;
    rest->cs = cs;

	if (cs == json_error) {
        dbr_err_set(DBR_EINVAL, msg);
		return -1;
    }

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
            cs = rurl_error; msg = "max string length exceeded";
            fbreak;
        }
    }
    action end_str {
        if (str.count < str.max)
            str.buf[str.count] = '\0';
    }
    str = ([^/]+ $add_char) >begin_str %end_str;

    action begin_id {
        if (rest->fields & DBR_PARAM_ID) {
            cs = rurl_error; msg = "id already specified";
            fbreak;
        }
    }
    action end_id {
        rest->fields |= DBR_PARAM_ID;
        rest->id = inum;
    }
    opt_id = ('/' inum) >begin_id %end_id
           | '';

    action begin_accnt {
        if (rest->fields & DBR_PARAM_ACCNT) {
            cs = rurl_error; msg = "accnt already specified";
            fbreak;
        }
        str.buf = rest->accnt;
        str.max = DBR_MNEM_MAX;
    }
    action end_accnt {
        rest->fields |= DBR_PARAM_ACCNT;
    }
    opt_accnt = ('/' str) >begin_accnt %end_accnt
               | '';
    req_accnt = ('/' str) >begin_accnt %end_accnt;

    action begin_group {
        if (rest->fields & DBR_PARAM_GROUP) {
            cs = rurl_error; msg = "group already specified";
            fbreak;
        }
        str.buf = rest->group;
        str.max = DBR_MNEM_MAX;
    }
    action end_group {
        rest->fields |= DBR_PARAM_GROUP;
    }
    req_group = ('/' str) >begin_group %end_group;

    action begin_contr {
        if (rest->fields & DBR_PARAM_CONTR) {
            cs = rurl_error; msg = "contr already specified";
            fbreak;
        }
        str.buf = rest->contr;
        str.max = DBR_MNEM_MAX;
    }
    action end_contr {
        rest->fields |= DBR_PARAM_CONTR;
    }
    opt_contr = ('/' str) >begin_contr %end_contr
               | '';

    action logon_resrc {
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_LOGON;
    }
    action logoff_resrc {
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_LOGOFF;
    }
    action accnt_resrc {
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_ACCNT;
    }
    action contr_resrc {
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_CONTR;
    }
    action user_resrc {
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_USER;
    }
    action group_resrc {
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_GROUP;
    }
    action order_resrc {
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_ORDER;
    }
    action trade_resrc {
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_TRADE;
    }
    action posn_resrc {
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_POSN;
    }
    action market_resrc {
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_MARKET;
    }

    resrc =  'logon' %logon_resrc
           | 'logoff' %logoff_resrc
           | ('accnt' opt_accnt) %accnt_resrc
           | ('contr' opt_contr) %contr_resrc
           | ('user' req_accnt) %user_resrc
           | ('group' req_accnt) %group_resrc
           | ('order' req_accnt opt_id) %order_resrc
           | ('trade' req_accnt opt_id) %trade_resrc
           | ('posn' req_group) %posn_resrc
           | ('market' opt_contr) %market_resrc;

    action begin_resrc {
        if ((rest->fields & DBR_RESRC_MASK) != 0) {
            cs = rurl_error; msg = "resrc already specified";
            fbreak;
        }
    }

    main := ([/] resrc [/]?) >begin_resrc;
}%%

%% write data;

DBR_API int
dbr_rest_rurl(struct DbrRest* rest, const char* buf, size_t size)
{
    int cs;
	const char* p = buf;
	const char* pe = p + size;
    const char* eof = pe;
    const char* msg = "parse error";

    long inum;
    struct {
        char* buf;
        size_t count;
        size_t max;
    } str;

    %% write init;
    %% write exec;

	if (cs == rurl_error) {
        dbr_err_set(DBR_EINVAL, msg);
		return -1;
    }

	if (cs < rurl_first_final)
		return 0;

	return 1;
}

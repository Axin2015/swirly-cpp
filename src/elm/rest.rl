// -*- c -*-
/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <sc/elm/rest.h>

#include <sc/ash/err.h>

static long
inum(const struct ScRest* rest)
{
    return rest->num.sign * rest->num.idigits;
}

#if 0
static double
fnum(const struct ScRest* rest)
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
        sc_rest_set_method(rest, SC_METHOD_DELETE);
    }
    action get_method {
        sc_rest_set_method(rest, SC_METHOD_GET);
    }
    action head_method {
        sc_rest_set_method(rest, SC_METHOD_HEAD);
    }
    action post_method {
        sc_rest_set_method(rest, SC_METHOD_POST);
    }
    action put_method {
        sc_rest_set_method(rest, SC_METHOD_PUT);
    }
    method  = '"delete"'i %delete_method
            | '"get"'i %get_method
            | '"head"'i %head_method
            | '"post"'i %post_method
            | '"put"'i %put_method;

    action logon_resrc {
        sc_rest_set_resrc(rest, SC_RESRC_LOGON);
    }
    action logoff_resrc {
        sc_rest_set_resrc(rest, SC_RESRC_LOGOFF);
    }
    action accnt_resrc {
        sc_rest_set_resrc(rest, SC_RESRC_ACCNT);
    }
    action contr_resrc {
        sc_rest_set_resrc(rest, SC_RESRC_CONTR);
    }
    action trader_resrc {
        sc_rest_set_resrc(rest, SC_RESRC_TRADER);
    }
    action giveup_resrc {
        sc_rest_set_resrc(rest, SC_RESRC_GIVEUP);
    }
    action order_resrc {
        sc_rest_set_resrc(rest, SC_RESRC_ORDER);
    }
    action trade_resrc {
        sc_rest_set_resrc(rest, SC_RESRC_TRADE);
    }
    action posn_resrc {
        sc_rest_set_resrc(rest, SC_RESRC_POSN);
    }
    action view_resrc {
        sc_rest_set_resrc(rest, SC_RESRC_VIEW);
    }
    resrc  =  '"logon"'i %logon_resrc
            | '"logoff"'i %logoff_resrc
            | '"accnt"'i %accnt_resrc
            | '"contr"'i %contr_resrc
            | '"trader"'i %trader_resrc
            | '"giveup"'i %giveup_resrc
            | '"order"'i %order_resrc
            | '"trade"'i %trade_resrc
            | '"posn"'i %posn_resrc
            | '"view"'i %view_resrc;

    action buy_action {
        rest->action = SC_ACTION_BUY;
    }
    action sell_action {
        rest->action = SC_ACTION_SELL;
    }
    action_ = '"buy"'i %buy_action
            | '"sell"'i %sell_action;

    colon = space* ':' space*;
    comma = space* ',' space*;

    action begin_method {
        if ((rest->fields & SC_METHOD_MASK) != 0) {
            cs = json_error; msg = "method already specified";
            fbreak;
        }
    }
    action begin_resrc {
        if ((rest->fields & SC_RESRC_MASK) != 0) {
            cs = json_error; msg = "resrc already specified";
            fbreak;
        }
    }
    action begin_id {
        if (rest->fields & SC_PARAM_ID) {
            cs = json_error; msg = "id already specified";
            fbreak;
        }
    }
    action end_id {
        sc_rest_set_param(rest, SC_PARAM_ID);
        rest->id = inum(rest);
    }
    action begin_accnt {
        if (rest->fields & SC_PARAM_ACCNT) {
            cs = json_error; msg = "accnt already specified";
            fbreak;
        }
        rest->str.buf = rest->accnt;
        rest->str.max = SC_MNEM_MAX;
    }
    action end_accnt {
        sc_rest_set_param(rest, SC_PARAM_ACCNT);
    }
    action begin_giveup {
        if (rest->fields & SC_PARAM_GIVEUP) {
            cs = json_error; msg = "giveup already specified";
            fbreak;
        }
        rest->str.buf = rest->giveup;
        rest->str.max = SC_MNEM_MAX;
    }
    action end_giveup {
        sc_rest_set_param(rest, SC_PARAM_GIVEUP);
    }
    action begin_contr {
        if (rest->fields & SC_PARAM_CONTR) {
            cs = json_error; msg = "contr already specified";
            fbreak;
        }
        rest->str.buf = rest->contr;
        rest->str.max = SC_MNEM_MAX;
    }
    action end_contr {
        sc_rest_set_param(rest, SC_PARAM_CONTR);
    }
    action begin_settl_date {
        if (rest->fields & SC_PARAM_SETTL_DATE) {
            cs = json_error; msg = "settl-date already specified";
            fbreak;
        }
    }
    action end_settl_date {
        sc_rest_set_param(rest, SC_PARAM_SETTL_DATE);
        rest->settl_date = inum(rest);
    }
    action begin_ref {
        if (rest->fields & SC_PARAM_REF) {
            cs = json_error; msg = "ref already specified";
            fbreak;
        }
        rest->str.buf = rest->ref;
        rest->str.max = SC_REF_MAX;
    }
    action end_ref {
        sc_rest_set_param(rest, SC_PARAM_REF);
    }
    action begin_action {
        if (rest->fields & SC_PARAM_ACTION) {
            cs = json_error; msg = "action already specified";
            fbreak;
        }
    }
    action end_action {
        sc_rest_set_param(rest, SC_PARAM_ACTION);
    }
    action begin_ticks {
        if (rest->fields & SC_PARAM_TICKS) {
            cs = json_error; msg = "ticks already specified";
            fbreak;
        }
    }
    action end_ticks {
        sc_rest_set_param(rest, SC_PARAM_TICKS);
        rest->ticks = inum(rest);
    }
    action begin_lots {
        if (rest->fields & SC_PARAM_LOTS) {
            cs = json_error; msg = "lots already specified";
            fbreak;
        }
    }
    action end_lots {
        sc_rest_set_param(rest, SC_PARAM_LOTS);
        rest->lots = inum(rest);
    }
    action begin_min_lots {
        if (rest->fields & SC_PARAM_MIN_LOTS) {
            cs = json_error; msg = "min-lots already specified";
            fbreak;
        }
    }
    action end_min_lots {
        sc_rest_set_param(rest, SC_PARAM_MIN_LOTS);
        rest->min_lots = inum(rest);
    }
    pair = '"method"'i colon method >begin_method
         | '"resrc"'i colon resrc >begin_resrc
         | '"id"'i colon inum >begin_id %end_id
         | '"accnt"'i colon str >begin_accnt %end_accnt
         | '"giveup"'i colon str >begin_giveup %end_giveup
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

SC_API void
sc_rest_init(struct ScRest* rest)
{
    int cs;
    %% write init;
    rest->cs = cs;
    rest->fields = 0;
}

SC_API int
sc_rest_json(struct ScRest* rest, const char* buf, size_t size)
{
	const char* p = buf;
	const char* pe = p + size;
    const char* msg = "parse error";

    int cs = rest->cs;
	%% write exec;
    rest->cs = cs;

	if (cs == json_error) {
        sc_err_set(SC_EINVAL, msg);
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
        if (rest->fields & SC_PARAM_ID) {
            cs = rurl_error; msg = "id already specified";
            fbreak;
        }
    }
    action end_id {
        sc_rest_set_param(rest, SC_PARAM_ID);
        rest->id = inum;
    }
    opt_id = ('/' inum) >begin_id %end_id
           | '';

    action begin_accnt {
        if (rest->fields & SC_PARAM_ACCNT) {
            cs = rurl_error; msg = "accnt already specified";
            fbreak;
        }
        str.buf = rest->accnt;
        str.max = SC_MNEM_MAX;
    }
    action end_accnt {
        sc_rest_set_param(rest, SC_PARAM_ACCNT);
    }
    opt_accnt = ('/' str) >begin_accnt %end_accnt
               | '';
    req_accnt = ('/' str) >begin_accnt %end_accnt;

    action begin_contr {
        if (rest->fields & SC_PARAM_CONTR) {
            cs = rurl_error; msg = "contr already specified";
            fbreak;
        }
        str.buf = rest->contr;
        str.max = SC_MNEM_MAX;
    }
    action end_contr {
        sc_rest_set_param(rest, SC_PARAM_CONTR);
    }
    opt_contr = ('/' str) >begin_contr %end_contr
               | '';

    action logon_resrc {
        sc_rest_set_resrc(rest, SC_RESRC_LOGON);
    }
    action logoff_resrc {
        sc_rest_set_resrc(rest, SC_RESRC_LOGOFF);
    }
    action accnt_resrc {
        sc_rest_set_resrc(rest, SC_RESRC_ACCNT);
    }
    action contr_resrc {
        sc_rest_set_resrc(rest, SC_RESRC_CONTR);
    }
    action trader_resrc {
        sc_rest_set_resrc(rest, SC_RESRC_TRADER);
    }
    action giveup_resrc {
        sc_rest_set_resrc(rest, SC_RESRC_GIVEUP);
    }
    action order_resrc {
        sc_rest_set_resrc(rest, SC_RESRC_ORDER);
    }
    action trade_resrc {
        sc_rest_set_resrc(rest, SC_RESRC_TRADE);
    }
    action posn_resrc {
        sc_rest_set_resrc(rest, SC_RESRC_POSN);
    }
    action view_resrc {
        sc_rest_set_resrc(rest, SC_RESRC_VIEW);
    }

    resrc =  'logon' %logon_resrc
           | 'logoff' %logoff_resrc
           | ('accnt' opt_accnt) %accnt_resrc
           | ('contr' opt_contr) %contr_resrc
           | ('trader' req_accnt) %trader_resrc
           | ('giveup' req_accnt) %giveup_resrc
           | ('order' req_accnt opt_id) %order_resrc
           | ('trade' req_accnt opt_id) %trade_resrc
           | ('posn' req_accnt) %posn_resrc
           | ('view' opt_contr) %view_resrc;

    action begin_resrc {
        if ((rest->fields & SC_RESRC_MASK) != 0) {
            cs = rurl_error; msg = "resrc already specified";
            fbreak;
        }
    }

    main := ([/] resrc [/]?) >begin_resrc;
}%%

%% write data;

SC_API int
sc_rest_rurl(struct ScRest* rest, const char* buf, size_t size)
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
        sc_err_set(SC_EINVAL, msg);
		return -1;
    }

	if (cs < rurl_first_final)
		return 0;

	return 1;
}

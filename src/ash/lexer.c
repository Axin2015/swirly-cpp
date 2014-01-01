
#line 1 "/Users/marayl/repo/doobry/src/ash/lexer.rl"
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
#include <dbr/lexer.h>
#include <dbr/err.h>

#include <stddef.h> // NULL


#line 82 "/Users/marayl/repo/doobry/src/ash/lexer.rl"



#line 33 "/Users/marayl/repo/doobry/src/ash/lexer.c"
static const char _lexer_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 2, 0, 1, 2, 3, 
	0, 2, 3, 4, 3, 3, 0, 1
	
};

static const char _lexer_key_offsets[] = {
	0, 0, 7, 14, 21, 23, 24, 25, 
	27, 28, 29, 36, 37
};

static const char _lexer_trans_keys[] = {
	32, 39, 92, 9, 10, 34, 35, 9, 
	10, 32, 34, 35, 39, 92, 9, 10, 
	32, 34, 35, 39, 92, 34, 92, 10, 
	10, 39, 92, 10, 10, 32, 34, 35, 
	39, 92, 9, 10, 10, 9, 10, 32, 
	34, 35, 39, 92, 0
};

static const char _lexer_single_lengths[] = {
	0, 3, 7, 7, 2, 1, 1, 2, 
	1, 1, 5, 1, 7
};

static const char _lexer_range_lengths[] = {
	0, 2, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 0, 0
};

static const char _lexer_index_offsets[] = {
	0, 0, 6, 14, 22, 25, 27, 29, 
	32, 34, 36, 43, 45
};

static const char _lexer_trans_targs[] = {
	2, 2, 11, 2, 2, 1, 3, 12, 
	3, 4, 6, 7, 9, 1, 3, 12, 
	3, 4, 6, 7, 9, 1, 2, 5, 
	4, 4, 4, 12, 6, 2, 8, 7, 
	7, 7, 10, 1, 0, 4, 0, 7, 
	9, 0, 1, 1, 1, 3, 12, 3, 
	4, 6, 7, 9, 1, 0
};

static const char _lexer_trans_actions[] = {
	5, 5, 0, 5, 5, 3, 7, 17, 
	7, 14, 7, 14, 14, 20, 0, 9, 
	0, 1, 0, 1, 1, 11, 0, 0, 
	3, 0, 3, 9, 0, 0, 0, 3, 
	0, 3, 0, 3, 0, 0, 0, 0, 
	0, 0, 3, 0, 3, 0, 9, 0, 
	1, 0, 1, 1, 11, 0
};

static const int lexer_start = 12;
static const int lexer_error = 0;

static const int lexer_en_main = 12;


#line 85 "/Users/marayl/repo/doobry/src/ash/lexer.rl"

DBR_API void
dbr_lexer_init(struct DbrLexer* lexer, void (*cb)(void*, const char*, size_t), void* ctx)
{
    lexer->cb = cb;
    lexer->ctx = ctx;

    int cs;
    
#line 106 "/Users/marayl/repo/doobry/src/ash/lexer.c"
	{
	cs = lexer_start;
	}

#line 94 "/Users/marayl/repo/doobry/src/ash/lexer.rl"
    lexer->cs = cs;
}

DBR_API void
dbr_lexer_reset(struct DbrLexer* lexer)
{
    int cs;
    
#line 120 "/Users/marayl/repo/doobry/src/ash/lexer.c"
	{
	cs = lexer_start;
	}

#line 102 "/Users/marayl/repo/doobry/src/ash/lexer.rl"
    lexer->cs = cs;
}

DBR_API DbrBool
dbr_lexer_exec(struct DbrLexer* lexer, const char* buf, size_t size)
{
	const char* p = buf;
	const char* pe = p + size;

    int cs = lexer->cs;
	
#line 137 "/Users/marayl/repo/doobry/src/ash/lexer.c"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _lexer_trans_keys + _lexer_key_offsets[cs];
	_trans = _lexer_index_offsets[cs];

	_klen = _lexer_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _lexer_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	cs = _lexer_trans_targs[_trans];

	if ( _lexer_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _lexer_actions + _lexer_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 30 "/Users/marayl/repo/doobry/src/ash/lexer.rl"
	{
        lexer->len = 0;
    }
	break;
	case 1:
#line 33 "/Users/marayl/repo/doobry/src/ash/lexer.rl"
	{
        if (lexer->len < DBR_TOK_MAX)
            lexer->tok[lexer->len++] = (*p);
        else {
            cs = lexer_error;
            {p++; goto _out; }
        }
    }
	break;
	case 2:
#line 41 "/Users/marayl/repo/doobry/src/ash/lexer.rl"
	{
        p--;
    }
	break;
	case 3:
#line 44 "/Users/marayl/repo/doobry/src/ash/lexer.rl"
	{
        lexer->tok[lexer->len] = '\0';
        lexer->cb(lexer->ctx, lexer->tok, lexer->len);
    }
	break;
	case 4:
#line 48 "/Users/marayl/repo/doobry/src/ash/lexer.rl"
	{
        lexer->cb(lexer->ctx, NULL, 0);
    }
	break;
#line 246 "/Users/marayl/repo/doobry/src/ash/lexer.c"
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}

#line 113 "/Users/marayl/repo/doobry/src/ash/lexer.rl"
    lexer->cs = cs;

    if (cs == lexer_error) {
        dbr_err_set(DBR_EINVAL, "lexical error");
        dbr_lexer_reset(lexer);
        return DBR_FALSE;
    }
	return DBR_TRUE;
}


#line 1 "/Users/marayl/repo/twirlyc/src/ash/shlex.rl"
// -*- c -*-
/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <sc/ash/shlex.h>
#include <sc/ash/err.h>

#include <stddef.h> // NULL


#line 67 "/Users/marayl/repo/twirlyc/src/ash/shlex.rl"



#line 18 "/Users/marayl/repo/twirlyc/src/ash/shlex.c"
static const char _shlex_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 2, 0, 1, 2, 3, 
	0, 2, 3, 4, 3, 3, 0, 1
	
};

static const char _shlex_key_offsets[] = {
	0, 0, 7, 14, 21, 23, 24, 25, 
	27, 28, 29, 36, 37
};

static const char _shlex_trans_keys[] = {
	32, 39, 92, 9, 10, 34, 35, 9, 
	10, 32, 34, 35, 39, 92, 9, 10, 
	32, 34, 35, 39, 92, 34, 92, 10, 
	10, 39, 92, 10, 10, 32, 34, 35, 
	39, 92, 9, 10, 10, 9, 10, 32, 
	34, 35, 39, 92, 0
};

static const char _shlex_single_lengths[] = {
	0, 3, 7, 7, 2, 1, 1, 2, 
	1, 1, 5, 1, 7
};

static const char _shlex_range_lengths[] = {
	0, 2, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 0, 0
};

static const char _shlex_index_offsets[] = {
	0, 0, 6, 14, 22, 25, 27, 29, 
	32, 34, 36, 43, 45
};

static const char _shlex_trans_targs[] = {
	2, 2, 11, 2, 2, 1, 3, 12, 
	3, 4, 6, 7, 9, 1, 3, 12, 
	3, 4, 6, 7, 9, 1, 2, 5, 
	4, 4, 4, 12, 6, 2, 8, 7, 
	7, 7, 10, 1, 0, 4, 0, 7, 
	9, 0, 1, 1, 1, 3, 12, 3, 
	4, 6, 7, 9, 1, 0
};

static const char _shlex_trans_actions[] = {
	5, 5, 0, 5, 5, 3, 7, 17, 
	7, 14, 7, 14, 14, 20, 0, 9, 
	0, 1, 0, 1, 1, 11, 0, 0, 
	3, 0, 3, 9, 0, 0, 0, 3, 
	0, 3, 0, 3, 0, 0, 0, 0, 
	0, 0, 3, 0, 3, 0, 9, 0, 
	1, 0, 1, 1, 11, 0
};

static const int shlex_start = 12;
static const int shlex_error = 0;

static const int shlex_en_main = 12;


#line 70 "/Users/marayl/repo/twirlyc/src/ash/shlex.rl"

SC_API void
sc_shlex_init(struct ScShlex* shlex, void (*cb)(void*, const char*, size_t), void* ctx)
{
    shlex->cb = cb;
    shlex->ctx = ctx;

    int cs;
    
#line 91 "/Users/marayl/repo/twirlyc/src/ash/shlex.c"
	{
	cs = shlex_start;
	}

#line 79 "/Users/marayl/repo/twirlyc/src/ash/shlex.rl"
    shlex->cs = cs;
}

SC_API void
sc_shlex_reset(struct ScShlex* shlex)
{
    int cs;
    
#line 105 "/Users/marayl/repo/twirlyc/src/ash/shlex.c"
	{
	cs = shlex_start;
	}

#line 87 "/Users/marayl/repo/twirlyc/src/ash/shlex.rl"
    shlex->cs = cs;
}

SC_API ScBool
sc_shlex_exec(struct ScShlex* shlex, const char* buf, size_t size)
{
	const char* p = buf;
	const char* pe = p + size;

    int cs = shlex->cs;
	
#line 122 "/Users/marayl/repo/twirlyc/src/ash/shlex.c"
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
	_keys = _shlex_trans_keys + _shlex_key_offsets[cs];
	_trans = _shlex_index_offsets[cs];

	_klen = _shlex_single_lengths[cs];
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

	_klen = _shlex_range_lengths[cs];
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
	cs = _shlex_trans_targs[_trans];

	if ( _shlex_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _shlex_actions + _shlex_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 15 "/Users/marayl/repo/twirlyc/src/ash/shlex.rl"
	{
        shlex->len = 0;
    }
	break;
	case 1:
#line 18 "/Users/marayl/repo/twirlyc/src/ash/shlex.rl"
	{
        if (shlex->len < SC_TOK_MAX)
            shlex->tok[shlex->len++] = (*p);
        else {
            cs = shlex_error;
            {p++; goto _out; }
        }
    }
	break;
	case 2:
#line 26 "/Users/marayl/repo/twirlyc/src/ash/shlex.rl"
	{
        p--;
    }
	break;
	case 3:
#line 29 "/Users/marayl/repo/twirlyc/src/ash/shlex.rl"
	{
        shlex->tok[shlex->len] = '\0';
        shlex->cb(shlex->ctx, shlex->tok, shlex->len);
    }
	break;
	case 4:
#line 33 "/Users/marayl/repo/twirlyc/src/ash/shlex.rl"
	{
        shlex->cb(shlex->ctx, NULL, 0);
    }
	break;
#line 231 "/Users/marayl/repo/twirlyc/src/ash/shlex.c"
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

#line 98 "/Users/marayl/repo/twirlyc/src/ash/shlex.rl"
    shlex->cs = cs;

    if (cs == shlex_error) {
        sc_err_set(SC_EINVAL, "lexical error");
        sc_shlex_reset(shlex);
        return SC_FALSE;
    }
	return SC_TRUE;
}

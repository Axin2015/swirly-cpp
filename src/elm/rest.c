
#line 1 "/Users/marayl/repo/doobry/src/elm/rest.rl"
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
#include <dbr/rest.h>

#include <dbr/err.h>

static long
inum(const struct DbrRest* rest)
{
    return rest->num.sign * rest->num.idigits;
}


#line 286 "/Users/marayl/repo/doobry/src/elm/rest.rl"



#line 37 "/Users/marayl/repo/doobry/src/elm/rest.c"
static const char _json_actions[] = {
	0, 1, 2, 1, 4, 1, 6, 1, 
	7, 1, 8, 1, 9, 1, 10, 1, 
	11, 1, 12, 1, 13, 1, 14, 1, 
	15, 1, 16, 1, 17, 1, 18, 1, 
	19, 1, 20, 1, 23, 1, 24, 1, 
	26, 1, 31, 1, 34, 1, 36, 2, 
	5, 28, 2, 5, 30, 2, 21, 32, 
	2, 22, 32, 2, 25, 0, 2, 27, 
	3, 2, 29, 3, 2, 33, 0, 2, 
	35, 0, 3, 25, 0, 1, 3, 25, 
	0, 2, 3, 33, 0, 1, 3, 33, 
	0, 2, 3, 35, 0, 1, 3, 35, 
	0, 2
};

static const short _json_key_offsets[] = {
	0, 0, 4, 9, 21, 23, 27, 29, 
	31, 32, 36, 40, 42, 47, 52, 56, 
	56, 58, 60, 62, 63, 67, 71, 75, 
	77, 79, 80, 85, 87, 89, 91, 92, 
	97, 99, 100, 104, 111, 113, 120, 122, 
	124, 126, 127, 131, 138, 140, 147, 149, 
	151, 153, 155, 157, 158, 162, 166, 174, 
	176, 178, 180, 182, 184, 185, 190, 192, 
	194, 195, 200, 202, 204, 206, 207, 212, 
	216, 218, 220, 221, 226, 228, 229, 234, 
	236, 240, 241, 245, 249, 251, 256, 256, 
	258, 260, 261, 265, 269, 287, 289, 291, 
	293, 295, 296, 301, 303, 305, 307, 309, 
	310, 315, 317, 319, 321, 323, 324, 329, 
	331, 333, 335, 339, 341, 342, 347, 348, 
	353, 355, 357, 359, 361, 363, 364, 369, 
	371, 373, 375, 377, 378, 383, 385, 387, 
	389, 390, 395, 397, 399, 401, 403, 404, 
	409, 411, 413, 415, 416, 421, 423, 425, 
	427, 429, 430, 434, 441, 443, 450
};

static const char _json_trans_keys[] = {
	32, 123, 9, 13, 32, 34, 125, 9, 
	13, 65, 73, 76, 77, 82, 84, 97, 
	105, 108, 109, 114, 116, 67, 99, 67, 
	84, 99, 116, 78, 110, 84, 116, 34, 
	32, 58, 9, 13, 32, 34, 9, 13, 
	34, 92, 32, 44, 125, 9, 13, 32, 
	44, 125, 9, 13, 32, 34, 9, 13, 
	73, 105, 79, 111, 78, 110, 34, 32, 
	58, 9, 13, 32, 34, 9, 13, 66, 
	83, 98, 115, 85, 117, 89, 121, 34, 
	32, 44, 125, 9, 13, 69, 101, 76, 
	108, 76, 108, 34, 32, 44, 125, 9, 
	13, 68, 100, 34, 32, 58, 9, 13, 
	32, 43, 45, 9, 13, 48, 57, 48, 
	57, 32, 44, 125, 9, 13, 48, 57, 
	79, 111, 84, 116, 83, 115, 34, 32, 
	58, 9, 13, 32, 43, 45, 9, 13, 
	48, 57, 48, 57, 32, 44, 125, 9, 
	13, 48, 57, 69, 101, 84, 116, 72, 
	104, 79, 111, 68, 100, 34, 32, 58, 
	9, 13, 32, 34, 9, 13, 68, 71, 
	72, 80, 100, 103, 104, 112, 69, 101, 
	76, 108, 69, 101, 84, 116, 69, 101, 
	34, 32, 44, 125, 9, 13, 69, 101, 
	84, 116, 34, 32, 44, 125, 9, 13, 
	69, 101, 65, 97, 68, 100, 34, 32, 
	44, 125, 9, 13, 79, 85, 111, 117, 
	83, 115, 84, 116, 34, 32, 44, 125, 
	9, 13, 84, 116, 34, 32, 44, 125, 
	9, 13, 69, 101, 70, 83, 102, 115, 
	34, 32, 58, 9, 13, 32, 34, 9, 
	13, 34, 92, 32, 44, 125, 9, 13, 
	82, 114, 67, 99, 34, 32, 58, 9, 
	13, 32, 34, 9, 13, 65, 67, 71, 
	76, 77, 79, 80, 84, 85, 97, 99, 
	103, 108, 109, 111, 112, 116, 117, 67, 
	99, 67, 99, 78, 110, 84, 116, 34, 
	32, 44, 125, 9, 13, 79, 111, 78, 
	110, 84, 116, 82, 114, 34, 32, 44, 
	125, 9, 13, 82, 114, 79, 111, 85, 
	117, 80, 112, 34, 32, 44, 125, 9, 
	13, 79, 111, 71, 103, 79, 111, 70, 
	78, 102, 110, 70, 102, 34, 32, 44, 
	125, 9, 13, 34, 32, 44, 125, 9, 
	13, 65, 97, 82, 114, 75, 107, 69, 
	101, 84, 116, 34, 32, 44, 125, 9, 
	13, 82, 114, 68, 100, 69, 101, 82, 
	114, 34, 32, 44, 125, 9, 13, 79, 
	111, 83, 115, 78, 110, 34, 32, 44, 
	125, 9, 13, 82, 114, 65, 97, 68, 
	100, 69, 101, 34, 32, 44, 125, 9, 
	13, 83, 115, 69, 101, 82, 114, 34, 
	32, 44, 125, 9, 13, 73, 105, 67, 
	99, 75, 107, 83, 115, 34, 32, 58, 
	9, 13, 32, 43, 45, 9, 13, 48, 
	57, 48, 57, 32, 44, 125, 9, 13, 
	48, 57, 32, 9, 13, 0
};

static const char _json_single_lengths[] = {
	0, 2, 3, 12, 2, 4, 2, 2, 
	1, 2, 2, 2, 3, 3, 2, 0, 
	2, 2, 2, 1, 2, 2, 4, 2, 
	2, 1, 3, 2, 2, 2, 1, 3, 
	2, 1, 2, 3, 0, 3, 2, 2, 
	2, 1, 2, 3, 0, 3, 2, 2, 
	2, 2, 2, 1, 2, 2, 8, 2, 
	2, 2, 2, 2, 1, 3, 2, 2, 
	1, 3, 2, 2, 2, 1, 3, 4, 
	2, 2, 1, 3, 2, 1, 3, 2, 
	4, 1, 2, 2, 2, 3, 0, 2, 
	2, 1, 2, 2, 18, 2, 2, 2, 
	2, 1, 3, 2, 2, 2, 2, 1, 
	3, 2, 2, 2, 2, 1, 3, 2, 
	2, 2, 4, 2, 1, 3, 1, 3, 
	2, 2, 2, 2, 2, 1, 3, 2, 
	2, 2, 2, 1, 3, 2, 2, 2, 
	1, 3, 2, 2, 2, 2, 1, 3, 
	2, 2, 2, 1, 3, 2, 2, 2, 
	2, 1, 2, 3, 0, 3, 1
};

static const char _json_range_lengths[] = {
	0, 1, 1, 0, 0, 0, 0, 0, 
	0, 1, 1, 0, 1, 1, 1, 0, 
	0, 0, 0, 0, 1, 1, 0, 0, 
	0, 0, 1, 0, 0, 0, 0, 1, 
	0, 0, 1, 2, 1, 2, 0, 0, 
	0, 0, 1, 2, 1, 2, 0, 0, 
	0, 0, 0, 0, 1, 1, 0, 0, 
	0, 0, 0, 0, 0, 1, 0, 0, 
	0, 1, 0, 0, 0, 0, 1, 0, 
	0, 0, 0, 1, 0, 0, 1, 0, 
	0, 0, 1, 1, 0, 1, 0, 0, 
	0, 0, 1, 1, 0, 0, 0, 0, 
	0, 0, 1, 0, 0, 0, 0, 0, 
	1, 0, 0, 0, 0, 0, 1, 0, 
	0, 0, 0, 0, 0, 1, 0, 1, 
	0, 0, 0, 0, 0, 0, 1, 0, 
	0, 0, 0, 0, 1, 0, 0, 0, 
	0, 1, 0, 0, 0, 0, 0, 1, 
	0, 0, 0, 0, 1, 0, 0, 0, 
	0, 0, 1, 2, 1, 2, 1
};

static const short _json_index_offsets[] = {
	0, 0, 4, 9, 22, 25, 30, 33, 
	36, 38, 42, 46, 49, 54, 59, 63, 
	64, 67, 70, 73, 75, 79, 83, 88, 
	91, 94, 96, 101, 104, 107, 110, 112, 
	117, 120, 122, 126, 132, 134, 140, 143, 
	146, 149, 151, 155, 161, 163, 169, 172, 
	175, 178, 181, 184, 186, 190, 194, 203, 
	206, 209, 212, 215, 218, 220, 225, 228, 
	231, 233, 238, 241, 244, 247, 249, 254, 
	259, 262, 265, 267, 272, 275, 277, 282, 
	285, 290, 292, 296, 300, 303, 308, 309, 
	312, 315, 317, 321, 325, 344, 347, 350, 
	353, 356, 358, 363, 366, 369, 372, 375, 
	377, 382, 385, 388, 391, 394, 396, 401, 
	404, 407, 410, 415, 418, 420, 425, 427, 
	432, 435, 438, 441, 444, 447, 449, 454, 
	457, 460, 463, 466, 468, 473, 476, 479, 
	482, 484, 489, 492, 495, 498, 501, 503, 
	508, 511, 514, 517, 519, 524, 527, 530, 
	533, 536, 538, 542, 548, 550, 556
};

static const unsigned char _json_indicies[] = {
	0, 2, 0, 1, 2, 3, 4, 2, 
	1, 5, 6, 7, 8, 9, 10, 5, 
	6, 7, 8, 9, 10, 1, 11, 11, 
	1, 12, 13, 12, 13, 1, 14, 14, 
	1, 15, 15, 1, 16, 1, 16, 17, 
	16, 1, 17, 18, 17, 1, 20, 21, 
	19, 22, 23, 24, 22, 1, 25, 26, 
	4, 25, 1, 26, 3, 26, 1, 19, 
	27, 27, 1, 28, 28, 1, 29, 29, 
	1, 30, 1, 30, 31, 30, 1, 31, 
	32, 31, 1, 33, 34, 33, 34, 1, 
	35, 35, 1, 36, 36, 1, 37, 1, 
	38, 39, 40, 38, 1, 41, 41, 1, 
	42, 42, 1, 43, 43, 1, 44, 1, 
	45, 46, 47, 45, 1, 48, 48, 1, 
	49, 1, 49, 50, 49, 1, 50, 51, 
	52, 50, 53, 1, 54, 1, 55, 56, 
	57, 55, 54, 1, 58, 58, 1, 59, 
	59, 1, 60, 60, 1, 61, 1, 61, 
	62, 61, 1, 62, 63, 64, 62, 65, 
	1, 66, 1, 67, 68, 69, 67, 66, 
	1, 70, 70, 1, 71, 71, 1, 72, 
	72, 1, 73, 73, 1, 74, 74, 1, 
	75, 1, 75, 76, 75, 1, 76, 77, 
	76, 1, 78, 79, 80, 81, 78, 79, 
	80, 81, 1, 82, 82, 1, 83, 83, 
	1, 84, 84, 1, 85, 85, 1, 86, 
	86, 1, 87, 1, 88, 89, 90, 88, 
	1, 91, 91, 1, 92, 92, 1, 93, 
	1, 94, 95, 96, 94, 1, 97, 97, 
	1, 98, 98, 1, 99, 99, 1, 100, 
	1, 101, 102, 103, 101, 1, 104, 105, 
	104, 105, 1, 106, 106, 1, 107, 107, 
	1, 108, 1, 109, 110, 111, 109, 1, 
	112, 112, 1, 113, 1, 114, 115, 116, 
	114, 1, 117, 117, 1, 118, 119, 118, 
	119, 1, 120, 1, 120, 121, 120, 1, 
	121, 122, 121, 1, 124, 125, 123, 126, 
	127, 128, 126, 1, 123, 129, 129, 1, 
	130, 130, 1, 131, 1, 131, 132, 131, 
	1, 132, 133, 132, 1, 134, 135, 136, 
	137, 138, 139, 140, 141, 142, 134, 135, 
	136, 137, 138, 139, 140, 141, 142, 1, 
	143, 143, 1, 144, 144, 1, 145, 145, 
	1, 146, 146, 1, 147, 1, 148, 149, 
	150, 148, 1, 151, 151, 1, 152, 152, 
	1, 153, 153, 1, 154, 154, 1, 155, 
	1, 156, 157, 158, 156, 1, 159, 159, 
	1, 160, 160, 1, 161, 161, 1, 162, 
	162, 1, 163, 1, 164, 165, 166, 164, 
	1, 167, 167, 1, 168, 168, 1, 169, 
	169, 1, 170, 171, 170, 171, 1, 172, 
	172, 1, 173, 1, 174, 175, 176, 174, 
	1, 177, 1, 178, 179, 180, 178, 1, 
	181, 181, 1, 182, 182, 1, 183, 183, 
	1, 184, 184, 1, 185, 185, 1, 186, 
	1, 187, 188, 189, 187, 1, 190, 190, 
	1, 191, 191, 1, 192, 192, 1, 193, 
	193, 1, 194, 1, 195, 196, 197, 195, 
	1, 198, 198, 1, 199, 199, 1, 200, 
	200, 1, 201, 1, 202, 203, 204, 202, 
	1, 205, 205, 1, 206, 206, 1, 207, 
	207, 1, 208, 208, 1, 209, 1, 210, 
	211, 212, 210, 1, 213, 213, 1, 214, 
	214, 1, 215, 215, 1, 216, 1, 217, 
	218, 219, 217, 1, 220, 220, 1, 221, 
	221, 1, 222, 222, 1, 223, 223, 1, 
	224, 1, 224, 225, 224, 1, 225, 226, 
	227, 225, 228, 1, 229, 1, 230, 231, 
	232, 230, 229, 1, 4, 4, 1, 0
};

static const unsigned char _json_trans_targs[] = {
	1, 0, 2, 3, 158, 4, 32, 38, 
	46, 79, 149, 5, 6, 16, 7, 8, 
	9, 10, 11, 11, 12, 15, 13, 14, 
	158, 13, 14, 17, 18, 19, 20, 21, 
	22, 23, 27, 24, 25, 26, 13, 14, 
	158, 28, 29, 30, 31, 13, 14, 158, 
	33, 34, 35, 36, 36, 37, 37, 13, 
	14, 158, 39, 40, 41, 42, 43, 44, 
	44, 45, 45, 13, 14, 158, 47, 48, 
	49, 50, 51, 52, 53, 54, 55, 62, 
	66, 71, 56, 57, 58, 59, 60, 61, 
	13, 14, 158, 63, 64, 65, 13, 14, 
	158, 67, 68, 69, 70, 13, 14, 158, 
	72, 76, 73, 74, 75, 13, 14, 158, 
	77, 78, 13, 14, 158, 80, 81, 87, 
	82, 83, 84, 84, 85, 86, 13, 14, 
	158, 88, 89, 90, 91, 92, 93, 99, 
	105, 111, 120, 127, 133, 138, 144, 94, 
	95, 96, 97, 98, 13, 14, 158, 100, 
	101, 102, 103, 104, 13, 14, 158, 106, 
	107, 108, 109, 110, 13, 14, 158, 112, 
	113, 114, 115, 118, 116, 117, 13, 14, 
	158, 119, 13, 14, 158, 121, 122, 123, 
	124, 125, 126, 13, 14, 158, 128, 129, 
	130, 131, 132, 13, 14, 158, 134, 135, 
	136, 137, 13, 14, 158, 139, 140, 141, 
	142, 143, 13, 14, 158, 145, 146, 147, 
	148, 13, 14, 158, 150, 151, 152, 153, 
	154, 155, 156, 156, 157, 157, 13, 14, 
	158
};

static const char _json_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 62, 3, 0, 0, 47, 47, 
	47, 0, 0, 0, 0, 0, 0, 0, 
	41, 0, 0, 0, 0, 0, 53, 53, 
	53, 0, 0, 0, 0, 56, 56, 56, 
	0, 0, 0, 59, 74, 78, 1, 39, 
	39, 39, 0, 0, 0, 0, 0, 71, 
	90, 94, 1, 45, 45, 45, 0, 0, 
	0, 0, 0, 0, 0, 35, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	5, 5, 5, 0, 0, 0, 7, 7, 
	7, 0, 0, 0, 0, 9, 9, 9, 
	0, 0, 0, 0, 0, 11, 11, 11, 
	0, 0, 13, 13, 13, 0, 0, 0, 
	0, 0, 65, 3, 0, 0, 50, 50, 
	50, 0, 0, 0, 0, 37, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 19, 19, 19, 0, 
	0, 0, 0, 0, 21, 21, 21, 0, 
	0, 0, 0, 0, 25, 25, 25, 0, 
	0, 0, 0, 0, 0, 0, 17, 17, 
	17, 0, 15, 15, 15, 0, 0, 0, 
	0, 0, 0, 33, 33, 33, 0, 0, 
	0, 0, 0, 27, 27, 27, 0, 0, 
	0, 0, 31, 31, 31, 0, 0, 0, 
	0, 0, 29, 29, 29, 0, 0, 0, 
	0, 23, 23, 23, 0, 0, 0, 0, 
	0, 0, 68, 82, 86, 1, 43, 43, 
	43
};

static const int json_start = 1;
static const int json_first_final = 158;
static const int json_error = 0;

static const int json_en_main = 1;


#line 289 "/Users/marayl/repo/doobry/src/elm/rest.rl"

DBR_API void
dbr_rest_init(struct DbrRest* rest)
{
    int cs;
    
#line 359 "/Users/marayl/repo/doobry/src/elm/rest.c"
	{
	cs = json_start;
	}

#line 295 "/Users/marayl/repo/doobry/src/elm/rest.rl"
    rest->cs = cs;
    rest->fields = 0;
}

DBR_API int
dbr_rest_json(struct DbrRest* rest, const char* buf, size_t size)
{
	const char* p = buf;
	const char* pe = p + size;

    int cs = rest->cs;
	
#line 377 "/Users/marayl/repo/doobry/src/elm/rest.c"
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
	_keys = _json_trans_keys + _json_key_offsets[cs];
	_trans = _json_index_offsets[cs];

	_klen = _json_single_lengths[cs];
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

	_klen = _json_range_lengths[cs];
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
	_trans = _json_indicies[_trans];
	cs = _json_trans_targs[_trans];

	if ( _json_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _json_actions + _json_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 32 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->num.sign = 1;
        rest->num.idigits = 0;
        rest->num.fdigits = 0;
        rest->num.fcount = 0;
    }
	break;
	case 1:
#line 38 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->num.sign = -1;
    }
	break;
	case 2:
#line 41 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->num.idigits = rest->num.idigits * 10 + ((*p) - '0');
    }
	break;
	case 3:
#line 89 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->str.count = 0;
    }
	break;
	case 4:
#line 92 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->str.count < rest->str.max)
            rest->str.buf[rest->str.count++] = (*p);
        else {
            dbr_err_set(DBR_EINVAL, "max string length exceeded");
            cs = json_error;
            {p++; goto _out; }
        }
    }
	break;
	case 5:
#line 101 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->str.count < rest->str.max)
            rest->str.buf[rest->str.count] = '\0';
    }
	break;
	case 6:
#line 116 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_METHOD_MASK) | DBR_METHOD_DELETE;
    }
	break;
	case 7:
#line 119 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_METHOD_MASK) | DBR_METHOD_GET;
    }
	break;
	case 8:
#line 122 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_METHOD_MASK) | DBR_METHOD_HEAD;
    }
	break;
	case 9:
#line 125 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_METHOD_MASK) | DBR_METHOD_POST;
    }
	break;
	case 10:
#line 128 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_METHOD_MASK) | DBR_METHOD_PUT;
    }
	break;
	case 11:
#line 137 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_LOGON;
    }
	break;
	case 12:
#line 140 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_LOGOFF;
    }
	break;
	case 13:
#line 143 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_ACCNT;
    }
	break;
	case 14:
#line 146 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_CONTR;
    }
	break;
	case 15:
#line 149 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_USER;
    }
	break;
	case 16:
#line 152 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_GROUP;
    }
	break;
	case 17:
#line 155 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_ORDER;
    }
	break;
	case 18:
#line 158 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_TRADE;
    }
	break;
	case 19:
#line 161 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_POSN;
    }
	break;
	case 20:
#line 164 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_MARKET;
    }
	break;
	case 21:
#line 178 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->action = DBR_ACTION_BUY;
    }
	break;
	case 22:
#line 181 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->action = DBR_ACTION_SELL;
    }
	break;
	case 23:
#line 190 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if ((rest->fields & DBR_METHOD_MASK) != 0) {
            dbr_err_set(DBR_EINVAL, "method already specified");
            cs = json_error;
            {p++; goto _out; }
        }
    }
	break;
	case 24:
#line 197 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if ((rest->fields & DBR_RESRC_MASK) != 0) {
            dbr_err_set(DBR_EINVAL, "resrc already specified");
            cs = json_error;
            {p++; goto _out; }
        }
    }
	break;
	case 25:
#line 204 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_ID) {
            dbr_err_set(DBR_EINVAL, "id already specified");
            cs = json_error;
            {p++; goto _out; }
        }
    }
	break;
	case 26:
#line 211 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_ID;
        rest->id = inum(rest);
    }
	break;
	case 27:
#line 215 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_ACCNT) {
            dbr_err_set(DBR_EINVAL, "accnt already specified");
            cs = json_error;
            {p++; goto _out; }
        }
        rest->str.buf = rest->accnt;
        rest->str.max = DBR_MNEM_MAX;
    }
	break;
	case 28:
#line 224 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_ACCNT;
    }
	break;
	case 29:
#line 227 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_REF) {
            dbr_err_set(DBR_EINVAL, "ref already specified");
            cs = json_error;
            {p++; goto _out; }
        }
        rest->str.buf = rest->ref;
        rest->str.max = DBR_REF_MAX;
    }
	break;
	case 30:
#line 236 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_REF;
    }
	break;
	case 31:
#line 239 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_ACTION) {
            dbr_err_set(DBR_EINVAL, "action already specified");
            cs = json_error;
            {p++; goto _out; }
        }
    }
	break;
	case 32:
#line 246 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_ACTION;
    }
	break;
	case 33:
#line 249 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_TICKS) {
            dbr_err_set(DBR_EINVAL, "ticks already specified");
            cs = json_error;
            {p++; goto _out; }
        }
    }
	break;
	case 34:
#line 256 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_TICKS;
        rest->ticks = inum(rest);
    }
	break;
	case 35:
#line 260 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_LOTS) {
            dbr_err_set(DBR_EINVAL, "lots already specified");
            cs = json_error;
            {p++; goto _out; }
        }
    }
	break;
	case 36:
#line 267 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_LOTS;
        rest->lots = inum(rest);
    }
	break;
#line 722 "/Users/marayl/repo/doobry/src/elm/rest.c"
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

#line 307 "/Users/marayl/repo/doobry/src/elm/rest.rl"
    rest->cs = cs;

	if (cs == json_error)
		return -1;

	if (cs < json_first_final)
		return 0;

	return 1;
}


#line 441 "/Users/marayl/repo/doobry/src/elm/rest.rl"



#line 752 "/Users/marayl/repo/doobry/src/elm/rest.c"
static const char _rurl_actions[] = {
	0, 1, 1, 1, 3, 1, 7, 1, 
	11, 1, 12, 1, 13, 1, 14, 1, 
	20, 1, 21, 2, 0, 1, 2, 2, 
	3, 2, 6, 17, 2, 6, 18, 2, 
	9, 20, 3, 4, 8, 15, 3, 4, 
	8, 16, 3, 4, 8, 17, 3, 4, 
	8, 18, 3, 4, 8, 19, 3, 4, 
	10, 20, 4, 4, 8, 5, 17, 4, 
	4, 8, 5, 18
};

static const char _rurl_key_offsets[] = {
	0, 0, 1, 10, 11, 12, 13, 14, 
	15, 16, 17, 18, 19, 20, 21, 22, 
	23, 24, 25, 26, 27, 29, 30, 31, 
	32, 33, 34, 35, 36, 37, 38, 39, 
	40, 41, 42, 43, 44, 45, 46, 47, 
	48, 49, 50, 51, 52, 53, 54, 55, 
	56, 57, 58, 58, 59, 60, 61, 62, 
	63, 64, 65, 66, 68, 71, 72, 73, 
	75, 78
};

static const char _rurl_trans_keys[] = {
	47, 97, 99, 103, 108, 109, 111, 112, 
	116, 117, 99, 99, 110, 116, 111, 110, 
	116, 114, 114, 111, 117, 112, 47, 47, 
	111, 103, 111, 102, 110, 102, 97, 114, 
	107, 101, 116, 114, 100, 101, 114, 47, 
	47, 111, 115, 110, 47, 47, 114, 97, 
	100, 101, 47, 47, 115, 101, 114, 47, 
	47, 47, 47, 47, 47, 47, 47, 47, 
	47, 47, 48, 57, 47, 48, 57, 47, 
	47, 48, 57, 47, 48, 57, 47, 0
};

static const char _rurl_single_lengths[] = {
	0, 1, 9, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 2, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 0, 1, 1, 1, 1, 1, 
	1, 1, 1, 0, 1, 1, 1, 0, 
	1, 1
};

static const char _rurl_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 1, 1, 0, 0, 1, 
	1, 0
};

static const unsigned char _rurl_index_offsets[] = {
	0, 0, 2, 12, 14, 16, 18, 20, 
	22, 24, 26, 28, 30, 32, 34, 36, 
	38, 40, 42, 44, 46, 49, 51, 53, 
	55, 57, 59, 61, 63, 65, 67, 69, 
	71, 73, 75, 77, 79, 81, 83, 85, 
	87, 89, 91, 93, 95, 97, 99, 101, 
	103, 105, 107, 108, 110, 112, 114, 116, 
	118, 120, 122, 124, 126, 129, 131, 133, 
	135, 138
};

static const char _rurl_trans_targs[] = {
	2, 0, 3, 7, 11, 17, 22, 27, 
	33, 38, 44, 0, 4, 0, 5, 0, 
	6, 0, 49, 0, 8, 0, 9, 0, 
	10, 0, 51, 0, 12, 0, 13, 0, 
	14, 0, 15, 0, 16, 0, 0, 52, 
	18, 0, 19, 0, 20, 0, 21, 54, 
	0, 53, 0, 23, 0, 24, 0, 25, 
	0, 26, 0, 55, 0, 28, 0, 29, 
	0, 30, 0, 31, 0, 32, 0, 0, 
	58, 34, 0, 35, 0, 36, 0, 37, 
	0, 0, 61, 39, 0, 40, 0, 41, 
	0, 42, 0, 43, 0, 0, 62, 45, 
	0, 46, 0, 47, 0, 48, 0, 0, 
	65, 50, 0, 0, 50, 0, 50, 52, 
	50, 0, 50, 0, 56, 0, 0, 57, 
	50, 57, 59, 58, 60, 0, 50, 60, 
	0, 50, 61, 63, 62, 64, 0, 50, 
	64, 0, 50, 65, 0
};

static const char _rurl_trans_actions[] = {
	17, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 5, 0, 0, 22, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 5, 0, 0, 
	22, 0, 0, 0, 0, 0, 0, 5, 
	0, 0, 22, 0, 0, 0, 0, 0, 
	0, 0, 0, 5, 0, 0, 22, 0, 
	0, 0, 0, 0, 0, 5, 0, 0, 
	22, 11, 0, 0, 13, 0, 38, 3, 
	9, 0, 7, 0, 31, 0, 0, 22, 
	54, 3, 58, 3, 19, 0, 25, 1, 
	0, 50, 3, 63, 3, 19, 0, 28, 
	1, 0, 34, 3, 0
};

static const char _rurl_eof_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 11, 0, 13, 38, 9, 7, 15, 
	0, 54, 42, 0, 25, 50, 46, 0, 
	28, 34
};

static const int rurl_start = 1;
static const int rurl_first_final = 49;
static const int rurl_error = 0;

static const int rurl_en_main = 1;


#line 444 "/Users/marayl/repo/doobry/src/elm/rest.rl"

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

    
#line 905 "/Users/marayl/repo/doobry/src/elm/rest.c"
	{
	cs = rurl_start;
	}

#line 461 "/Users/marayl/repo/doobry/src/elm/rest.rl"
    
#line 912 "/Users/marayl/repo/doobry/src/elm/rest.c"
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
	_keys = _rurl_trans_keys + _rurl_key_offsets[cs];
	_trans = _rurl_index_offsets[cs];

	_klen = _rurl_single_lengths[cs];
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

	_klen = _rurl_range_lengths[cs];
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
	cs = _rurl_trans_targs[_trans];

	if ( _rurl_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _rurl_actions + _rurl_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 321 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        inum = 0;
    }
	break;
	case 1:
#line 324 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        inum = inum * 10 + ((*p) - '0');
    }
	break;
	case 2:
#line 329 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        str.count = 0;
    }
	break;
	case 3:
#line 332 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (str.count < str.max)
            str.buf[str.count++] = (*p);
        else {
            dbr_err_set(DBR_EINVAL, "max string length exceeded");
            cs = rurl_error;
            {p++; goto _out; }
        }
    }
	break;
	case 4:
#line 341 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (str.count < str.max)
            str.buf[str.count] = '\0';
    }
	break;
	case 5:
#line 347 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_ID) {
            dbr_err_set(DBR_EINVAL, "id already specified");
            cs = rurl_error;
            {p++; goto _out; }
        }
    }
	break;
	case 6:
#line 354 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_ID;
        rest->id = inum;
    }
	break;
	case 7:
#line 361 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_ACCNT) {
            dbr_err_set(DBR_EINVAL, "accnt already specified");
            cs = rurl_error;
            {p++; goto _out; }
        }
        str.buf = rest->accnt;
        str.max = DBR_MNEM_MAX;
    }
	break;
	case 8:
#line 370 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_ACCNT;
    }
	break;
	case 9:
#line 375 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_CONTR) {
            dbr_err_set(DBR_EINVAL, "contr already specified");
            cs = rurl_error;
            {p++; goto _out; }
        }
        str.buf = rest->contr;
        str.max = DBR_MNEM_MAX;
    }
	break;
	case 10:
#line 384 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_CONTR;
    }
	break;
	case 11:
#line 390 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_LOGON;
    }
	break;
	case 12:
#line 393 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_LOGOFF;
    }
	break;
	case 13:
#line 396 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_ACCNT;
    }
	break;
	case 14:
#line 399 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_CONTR;
    }
	break;
	case 15:
#line 402 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_USER;
    }
	break;
	case 16:
#line 405 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_GROUP;
    }
	break;
	case 17:
#line 408 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_ORDER;
    }
	break;
	case 18:
#line 411 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_TRADE;
    }
	break;
	case 19:
#line 414 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_POSN;
    }
	break;
	case 20:
#line 417 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_MARKET;
    }
	break;
	case 21:
#line 432 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if ((rest->fields & DBR_RESRC_MASK) != 0) {
            dbr_err_set(DBR_EINVAL, "resrc already specified");
            cs = rurl_error;
            {p++; goto _out; }
        }
    }
	break;
#line 1145 "/Users/marayl/repo/doobry/src/elm/rest.c"
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	if ( p == eof )
	{
	const char *__acts = _rurl_actions + _rurl_eof_actions[cs];
	unsigned int __nacts = (unsigned int) *__acts++;
	while ( __nacts-- > 0 ) {
		switch ( *__acts++ ) {
	case 4:
#line 341 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (str.count < str.max)
            str.buf[str.count] = '\0';
    }
	break;
	case 6:
#line 354 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_ID;
        rest->id = inum;
    }
	break;
	case 8:
#line 370 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_ACCNT;
    }
	break;
	case 10:
#line 384 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_CONTR;
    }
	break;
	case 11:
#line 390 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_LOGON;
    }
	break;
	case 12:
#line 393 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_LOGOFF;
    }
	break;
	case 13:
#line 396 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_ACCNT;
    }
	break;
	case 14:
#line 399 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_CONTR;
    }
	break;
	case 15:
#line 402 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_USER;
    }
	break;
	case 16:
#line 405 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_GROUP;
    }
	break;
	case 17:
#line 408 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_ORDER;
    }
	break;
	case 18:
#line 411 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_TRADE;
    }
	break;
	case 19:
#line 414 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_POSN;
    }
	break;
	case 20:
#line 417 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_MARKET;
    }
	break;
#line 1247 "/Users/marayl/repo/doobry/src/elm/rest.c"
		}
	}
	}

	_out: {}
	}

#line 462 "/Users/marayl/repo/doobry/src/elm/rest.rl"

	if (cs == rurl_error)
		return -1;

	if (cs < rurl_first_final)
		return 0;

	return 1;
}

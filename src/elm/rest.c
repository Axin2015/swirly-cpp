
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


#line 332 "/Users/marayl/repo/doobry/src/elm/rest.rl"



#line 46 "/Users/marayl/repo/doobry/src/elm/rest.c"
static const char _json_actions[] = {
	0, 1, 2, 1, 4, 1, 6, 1, 
	7, 1, 8, 1, 9, 1, 10, 1, 
	11, 1, 12, 1, 13, 1, 14, 1, 
	15, 1, 16, 1, 17, 1, 18, 1, 
	19, 1, 20, 1, 23, 1, 24, 1, 
	26, 1, 34, 1, 37, 1, 40, 1, 
	42, 1, 44, 2, 5, 28, 2, 5, 
	30, 2, 5, 32, 2, 5, 36, 2, 
	21, 38, 2, 22, 38, 2, 25, 0, 
	2, 27, 3, 2, 29, 3, 2, 31, 
	3, 2, 33, 0, 2, 35, 3, 2, 
	39, 0, 2, 41, 0, 2, 43, 0, 
	3, 25, 0, 1, 3, 25, 0, 2, 
	3, 33, 0, 1, 3, 33, 0, 2, 
	3, 39, 0, 1, 3, 39, 0, 2, 
	3, 41, 0, 1, 3, 41, 0, 2, 
	3, 43, 0, 1, 3, 43, 0, 2
	
};

static const short _json_key_offsets[] = {
	0, 0, 4, 9, 27, 29, 33, 35, 
	37, 38, 42, 46, 48, 53, 58, 62, 
	62, 64, 66, 68, 69, 73, 77, 81, 
	83, 85, 86, 91, 93, 95, 97, 98, 
	103, 105, 107, 109, 111, 112, 116, 120, 
	122, 127, 127, 129, 131, 133, 135, 137, 
	138, 142, 146, 148, 153, 153, 155, 156, 
	160, 167, 169, 176, 178, 180, 182, 183, 
	187, 194, 196, 203, 207, 209, 211, 213, 
	215, 216, 220, 224, 232, 234, 236, 238, 
	240, 242, 243, 248, 250, 252, 253, 258, 
	260, 262, 264, 265, 270, 274, 276, 278, 
	279, 284, 286, 287, 292, 294, 295, 297, 
	299, 301, 303, 304, 308, 315, 317, 324, 
	326, 330, 331, 335, 339, 341, 346, 346, 
	348, 350, 351, 355, 359, 375, 377, 379, 
	381, 383, 384, 389, 391, 393, 395, 397, 
	398, 403, 405, 407, 409, 411, 413, 414, 
	419, 421, 423, 425, 429, 431, 432, 437, 
	438, 443, 445, 447, 449, 451, 452, 457, 
	459, 461, 463, 464, 469, 471, 473, 475, 
	477, 480, 485, 486, 491, 493, 495, 497, 
	498, 503, 505, 507, 509, 511, 512, 514, 
	516, 518, 520, 521, 525, 532, 534, 541, 
	543, 545, 547, 549, 550, 554, 561, 563, 
	570
};

static const char _json_trans_keys[] = {
	32, 123, 9, 13, 32, 34, 125, 9, 
	13, 65, 67, 71, 73, 76, 77, 82, 
	83, 84, 97, 99, 103, 105, 108, 109, 
	114, 115, 116, 67, 99, 67, 84, 99, 
	116, 78, 110, 84, 116, 34, 32, 58, 
	9, 13, 32, 34, 9, 13, 34, 92, 
	32, 44, 125, 9, 13, 32, 44, 125, 
	9, 13, 32, 34, 9, 13, 73, 105, 
	79, 111, 78, 110, 34, 32, 58, 9, 
	13, 32, 34, 9, 13, 66, 83, 98, 
	115, 85, 117, 89, 121, 34, 32, 44, 
	125, 9, 13, 69, 101, 76, 108, 76, 
	108, 34, 32, 44, 125, 9, 13, 79, 
	111, 78, 110, 84, 116, 82, 114, 34, 
	32, 58, 9, 13, 32, 34, 9, 13, 
	34, 92, 32, 44, 125, 9, 13, 73, 
	105, 86, 118, 69, 101, 85, 117, 80, 
	112, 34, 32, 58, 9, 13, 32, 34, 
	9, 13, 34, 92, 32, 44, 125, 9, 
	13, 68, 100, 34, 32, 58, 9, 13, 
	32, 43, 45, 9, 13, 48, 57, 48, 
	57, 32, 44, 125, 9, 13, 48, 57, 
	79, 111, 84, 116, 83, 115, 34, 32, 
	58, 9, 13, 32, 43, 45, 9, 13, 
	48, 57, 48, 57, 32, 44, 125, 9, 
	13, 48, 57, 69, 73, 101, 105, 84, 
	116, 72, 104, 79, 111, 68, 100, 34, 
	32, 58, 9, 13, 32, 34, 9, 13, 
	68, 71, 72, 80, 100, 103, 104, 112, 
	69, 101, 76, 108, 69, 101, 84, 116, 
	69, 101, 34, 32, 44, 125, 9, 13, 
	69, 101, 84, 116, 34, 32, 44, 125, 
	9, 13, 69, 101, 65, 97, 68, 100, 
	34, 32, 44, 125, 9, 13, 79, 85, 
	111, 117, 83, 115, 84, 116, 34, 32, 
	44, 125, 9, 13, 84, 116, 34, 32, 
	44, 125, 9, 13, 78, 110, 95, 76, 
	108, 79, 111, 84, 116, 83, 115, 34, 
	32, 58, 9, 13, 32, 43, 45, 9, 
	13, 48, 57, 48, 57, 32, 44, 125, 
	9, 13, 48, 57, 69, 101, 70, 83, 
	102, 115, 34, 32, 58, 9, 13, 32, 
	34, 9, 13, 34, 92, 32, 44, 125, 
	9, 13, 82, 114, 67, 99, 34, 32, 
	58, 9, 13, 32, 34, 9, 13, 65, 
	67, 71, 76, 79, 80, 84, 86, 97, 
	99, 103, 108, 111, 112, 116, 118, 67, 
	99, 67, 99, 78, 110, 84, 116, 34, 
	32, 44, 125, 9, 13, 79, 111, 78, 
	110, 84, 116, 82, 114, 34, 32, 44, 
	125, 9, 13, 73, 105, 86, 118, 69, 
	101, 85, 117, 80, 112, 34, 32, 44, 
	125, 9, 13, 79, 111, 71, 103, 79, 
	111, 70, 78, 102, 110, 70, 102, 34, 
	32, 44, 125, 9, 13, 34, 32, 44, 
	125, 9, 13, 82, 114, 68, 100, 69, 
	101, 82, 114, 34, 32, 44, 125, 9, 
	13, 79, 111, 83, 115, 78, 110, 34, 
	32, 44, 125, 9, 13, 82, 114, 65, 
	97, 68, 100, 69, 101, 34, 82, 114, 
	32, 44, 125, 9, 13, 34, 32, 44, 
	125, 9, 13, 73, 105, 69, 101, 87, 
	119, 34, 32, 44, 125, 9, 13, 69, 
	101, 84, 116, 84, 116, 76, 108, 95, 
	68, 100, 65, 97, 84, 116, 69, 101, 
	34, 32, 58, 9, 13, 32, 43, 45, 
	9, 13, 48, 57, 48, 57, 32, 44, 
	125, 9, 13, 48, 57, 73, 105, 67, 
	99, 75, 107, 83, 115, 34, 32, 58, 
	9, 13, 32, 43, 45, 9, 13, 48, 
	57, 48, 57, 32, 44, 125, 9, 13, 
	48, 57, 32, 9, 13, 0
};

static const char _json_single_lengths[] = {
	0, 2, 3, 18, 2, 4, 2, 2, 
	1, 2, 2, 2, 3, 3, 2, 0, 
	2, 2, 2, 1, 2, 2, 4, 2, 
	2, 1, 3, 2, 2, 2, 1, 3, 
	2, 2, 2, 2, 1, 2, 2, 2, 
	3, 0, 2, 2, 2, 2, 2, 1, 
	2, 2, 2, 3, 0, 2, 1, 2, 
	3, 0, 3, 2, 2, 2, 1, 2, 
	3, 0, 3, 4, 2, 2, 2, 2, 
	1, 2, 2, 8, 2, 2, 2, 2, 
	2, 1, 3, 2, 2, 1, 3, 2, 
	2, 2, 1, 3, 4, 2, 2, 1, 
	3, 2, 1, 3, 2, 1, 2, 2, 
	2, 2, 1, 2, 3, 0, 3, 2, 
	4, 1, 2, 2, 2, 3, 0, 2, 
	2, 1, 2, 2, 16, 2, 2, 2, 
	2, 1, 3, 2, 2, 2, 2, 1, 
	3, 2, 2, 2, 2, 2, 1, 3, 
	2, 2, 2, 4, 2, 1, 3, 1, 
	3, 2, 2, 2, 2, 1, 3, 2, 
	2, 2, 1, 3, 2, 2, 2, 2, 
	3, 3, 1, 3, 2, 2, 2, 1, 
	3, 2, 2, 2, 2, 1, 2, 2, 
	2, 2, 1, 2, 3, 0, 3, 2, 
	2, 2, 2, 1, 2, 3, 0, 3, 
	1
};

static const char _json_range_lengths[] = {
	0, 1, 1, 0, 0, 0, 0, 0, 
	0, 1, 1, 0, 1, 1, 1, 0, 
	0, 0, 0, 0, 1, 1, 0, 0, 
	0, 0, 1, 0, 0, 0, 0, 1, 
	0, 0, 0, 0, 0, 1, 1, 0, 
	1, 0, 0, 0, 0, 0, 0, 0, 
	1, 1, 0, 1, 0, 0, 0, 1, 
	2, 1, 2, 0, 0, 0, 0, 1, 
	2, 1, 2, 0, 0, 0, 0, 0, 
	0, 1, 1, 0, 0, 0, 0, 0, 
	0, 0, 1, 0, 0, 0, 1, 0, 
	0, 0, 0, 1, 0, 0, 0, 0, 
	1, 0, 0, 1, 0, 0, 0, 0, 
	0, 0, 0, 1, 2, 1, 2, 0, 
	0, 0, 1, 1, 0, 1, 0, 0, 
	0, 0, 1, 1, 0, 0, 0, 0, 
	0, 0, 1, 0, 0, 0, 0, 0, 
	1, 0, 0, 0, 0, 0, 0, 1, 
	0, 0, 0, 0, 0, 0, 1, 0, 
	1, 0, 0, 0, 0, 0, 1, 0, 
	0, 0, 0, 1, 0, 0, 0, 0, 
	0, 1, 0, 1, 0, 0, 0, 0, 
	1, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 1, 2, 1, 2, 0, 
	0, 0, 0, 0, 1, 2, 1, 2, 
	1
};

static const short _json_index_offsets[] = {
	0, 0, 4, 9, 28, 31, 36, 39, 
	42, 44, 48, 52, 55, 60, 65, 69, 
	70, 73, 76, 79, 81, 85, 89, 94, 
	97, 100, 102, 107, 110, 113, 116, 118, 
	123, 126, 129, 132, 135, 137, 141, 145, 
	148, 153, 154, 157, 160, 163, 166, 169, 
	171, 175, 179, 182, 187, 188, 191, 193, 
	197, 203, 205, 211, 214, 217, 220, 222, 
	226, 232, 234, 240, 245, 248, 251, 254, 
	257, 259, 263, 267, 276, 279, 282, 285, 
	288, 291, 293, 298, 301, 304, 306, 311, 
	314, 317, 320, 322, 327, 332, 335, 338, 
	340, 345, 348, 350, 355, 358, 360, 363, 
	366, 369, 372, 374, 378, 384, 386, 392, 
	395, 400, 402, 406, 410, 413, 418, 419, 
	422, 425, 427, 431, 435, 452, 455, 458, 
	461, 464, 466, 471, 474, 477, 480, 483, 
	485, 490, 493, 496, 499, 502, 505, 507, 
	512, 515, 518, 521, 526, 529, 531, 536, 
	538, 543, 546, 549, 552, 555, 557, 562, 
	565, 568, 571, 573, 578, 581, 584, 587, 
	590, 594, 599, 601, 606, 609, 612, 615, 
	617, 622, 625, 628, 631, 634, 636, 639, 
	642, 645, 648, 650, 654, 660, 662, 668, 
	671, 674, 677, 680, 682, 686, 692, 694, 
	700
};

static const unsigned char _json_trans_targs[] = {
	1, 2, 1, 0, 2, 3, 200, 2, 
	0, 4, 32, 42, 53, 59, 67, 111, 
	177, 191, 4, 32, 42, 53, 59, 67, 
	111, 177, 191, 0, 5, 5, 0, 6, 
	16, 6, 16, 0, 7, 7, 0, 8, 
	8, 0, 9, 0, 9, 10, 9, 0, 
	10, 11, 10, 0, 12, 15, 11, 13, 
	14, 200, 13, 0, 13, 14, 200, 13, 
	0, 14, 3, 14, 0, 11, 17, 17, 
	0, 18, 18, 0, 19, 19, 0, 20, 
	0, 20, 21, 20, 0, 21, 22, 21, 
	0, 23, 27, 23, 27, 0, 24, 24, 
	0, 25, 25, 0, 26, 0, 13, 14, 
	200, 13, 0, 28, 28, 0, 29, 29, 
	0, 30, 30, 0, 31, 0, 13, 14, 
	200, 13, 0, 33, 33, 0, 34, 34, 
	0, 35, 35, 0, 36, 36, 0, 37, 
	0, 37, 38, 37, 0, 38, 39, 38, 
	0, 40, 41, 39, 13, 14, 200, 13, 
	0, 39, 43, 43, 0, 44, 44, 0, 
	45, 45, 0, 46, 46, 0, 47, 47, 
	0, 48, 0, 48, 49, 48, 0, 49, 
	50, 49, 0, 51, 52, 50, 13, 14, 
	200, 13, 0, 50, 54, 54, 0, 55, 
	0, 55, 56, 55, 0, 56, 57, 57, 
	56, 58, 0, 58, 0, 13, 14, 200, 
	13, 58, 0, 60, 60, 0, 61, 61, 
	0, 62, 62, 0, 63, 0, 63, 64, 
	63, 0, 64, 65, 65, 64, 66, 0, 
	66, 0, 13, 14, 200, 13, 66, 0, 
	68, 100, 68, 100, 0, 69, 69, 0, 
	70, 70, 0, 71, 71, 0, 72, 72, 
	0, 73, 0, 73, 74, 73, 0, 74, 
	75, 74, 0, 76, 83, 87, 92, 76, 
	83, 87, 92, 0, 77, 77, 0, 78, 
	78, 0, 79, 79, 0, 80, 80, 0, 
	81, 81, 0, 82, 0, 13, 14, 200, 
	13, 0, 84, 84, 0, 85, 85, 0, 
	86, 0, 13, 14, 200, 13, 0, 88, 
	88, 0, 89, 89, 0, 90, 90, 0, 
	91, 0, 13, 14, 200, 13, 0, 93, 
	97, 93, 97, 0, 94, 94, 0, 95, 
	95, 0, 96, 0, 13, 14, 200, 13, 
	0, 98, 98, 0, 99, 0, 13, 14, 
	200, 13, 0, 101, 101, 0, 102, 0, 
	103, 103, 0, 104, 104, 0, 105, 105, 
	0, 106, 106, 0, 107, 0, 107, 108, 
	107, 0, 108, 109, 109, 108, 110, 0, 
	110, 0, 13, 14, 200, 13, 110, 0, 
	112, 112, 0, 113, 119, 113, 119, 0, 
	114, 0, 114, 115, 114, 0, 115, 116, 
	115, 0, 117, 118, 116, 13, 14, 200, 
	13, 0, 116, 120, 120, 0, 121, 121, 
	0, 122, 0, 122, 123, 122, 0, 123, 
	124, 123, 0, 125, 131, 137, 144, 153, 
	159, 164, 172, 125, 131, 137, 144, 153, 
	159, 164, 172, 0, 126, 126, 0, 127, 
	127, 0, 128, 128, 0, 129, 129, 0, 
	130, 0, 13, 14, 200, 13, 0, 132, 
	132, 0, 133, 133, 0, 134, 134, 0, 
	135, 135, 0, 136, 0, 13, 14, 200, 
	13, 0, 138, 138, 0, 139, 139, 0, 
	140, 140, 0, 141, 141, 0, 142, 142, 
	0, 143, 0, 13, 14, 200, 13, 0, 
	145, 145, 0, 146, 146, 0, 147, 147, 
	0, 148, 151, 148, 151, 0, 149, 149, 
	0, 150, 0, 13, 14, 200, 13, 0, 
	152, 0, 13, 14, 200, 13, 0, 154, 
	154, 0, 155, 155, 0, 156, 156, 0, 
	157, 157, 0, 158, 0, 13, 14, 200, 
	13, 0, 160, 160, 0, 161, 161, 0, 
	162, 162, 0, 163, 0, 13, 14, 200, 
	13, 0, 165, 165, 0, 166, 166, 0, 
	167, 167, 0, 168, 168, 0, 169, 170, 
	170, 0, 13, 14, 200, 13, 0, 171, 
	0, 13, 14, 200, 13, 0, 173, 173, 
	0, 174, 174, 0, 175, 175, 0, 176, 
	0, 13, 14, 200, 13, 0, 178, 178, 
	0, 179, 179, 0, 180, 180, 0, 181, 
	181, 0, 182, 0, 183, 183, 0, 184, 
	184, 0, 185, 185, 0, 186, 186, 0, 
	187, 0, 187, 188, 187, 0, 188, 189, 
	189, 188, 190, 0, 190, 0, 13, 14, 
	200, 13, 190, 0, 192, 192, 0, 193, 
	193, 0, 194, 194, 0, 195, 195, 0, 
	196, 0, 196, 197, 196, 0, 197, 198, 
	198, 197, 199, 0, 199, 0, 13, 14, 
	200, 13, 199, 0, 200, 200, 0, 0
};

static const unsigned char _json_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 72, 0, 0, 0, 0, 3, 51, 
	51, 51, 51, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 3, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 43, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 63, 63, 
	63, 63, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 66, 66, 
	66, 66, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 78, 0, 
	0, 0, 0, 3, 57, 57, 57, 57, 
	0, 3, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	75, 0, 0, 0, 0, 3, 54, 54, 
	54, 54, 0, 3, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 69, 96, 
	0, 100, 0, 1, 0, 39, 39, 39, 
	39, 1, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 90, 120, 0, 124, 0, 
	1, 0, 47, 47, 47, 47, 1, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	35, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 5, 5, 5, 
	5, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 7, 7, 7, 7, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 9, 9, 9, 9, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 11, 11, 11, 11, 
	0, 0, 0, 0, 0, 0, 13, 13, 
	13, 13, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 93, 128, 0, 132, 0, 
	1, 0, 49, 49, 49, 49, 1, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 84, 
	0, 0, 0, 0, 3, 60, 60, 60, 
	60, 0, 3, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	37, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 19, 19, 19, 19, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 21, 21, 21, 
	21, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 25, 25, 25, 25, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 17, 17, 17, 17, 0, 
	0, 0, 15, 15, 15, 15, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 27, 27, 27, 
	27, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 31, 31, 31, 
	31, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 29, 29, 29, 29, 0, 0, 
	0, 23, 23, 23, 23, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 33, 33, 33, 33, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 81, 
	104, 0, 108, 0, 1, 0, 41, 41, 
	41, 41, 1, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 87, 
	112, 0, 116, 0, 1, 0, 45, 45, 
	45, 45, 1, 0, 0, 0, 0, 0
};

static const int json_start = 1;
static const int json_first_final = 200;
static const int json_error = 0;

static const int json_en_main = 1;


#line 335 "/Users/marayl/repo/doobry/src/elm/rest.rl"

DBR_API void
dbr_rest_init(struct DbrRest* rest)
{
    int cs;
    
#line 455 "/Users/marayl/repo/doobry/src/elm/rest.c"
	{
	cs = json_start;
	}

#line 341 "/Users/marayl/repo/doobry/src/elm/rest.rl"
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
	
#line 474 "/Users/marayl/repo/doobry/src/elm/rest.c"
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
#line 41 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->num.sign = 1;
        rest->num.idigits = 0;
        rest->num.fdigits = 0;
        rest->num.fcount = 0;
    }
	break;
	case 1:
#line 47 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->num.sign = -1;
    }
	break;
	case 2:
#line 50 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->num.idigits = rest->num.idigits * 10 + ((*p) - '0');
    }
	break;
	case 3:
#line 98 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->str.count = 0;
    }
	break;
	case 4:
#line 101 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->str.count < rest->str.max)
            rest->str.buf[rest->str.count++] = (*p);
        else {
            cs = json_error; msg = "max string length exceeded";
            {p++; goto _out; }
        }
    }
	break;
	case 5:
#line 109 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->str.count < rest->str.max)
            rest->str.buf[rest->str.count] = '\0';
    }
	break;
	case 6:
#line 124 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_method(rest, DBR_METHOD_DELETE);
    }
	break;
	case 7:
#line 127 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_method(rest, DBR_METHOD_GET);
    }
	break;
	case 8:
#line 130 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_method(rest, DBR_METHOD_HEAD);
    }
	break;
	case 9:
#line 133 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_method(rest, DBR_METHOD_POST);
    }
	break;
	case 10:
#line 136 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_method(rest, DBR_METHOD_PUT);
    }
	break;
	case 11:
#line 145 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_LOGON);
    }
	break;
	case 12:
#line 148 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_LOGOFF);
    }
	break;
	case 13:
#line 151 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_ACCNT);
    }
	break;
	case 14:
#line 154 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_CONTR);
    }
	break;
	case 15:
#line 157 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_TRADER);
    }
	break;
	case 16:
#line 160 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_GIVEUP);
    }
	break;
	case 17:
#line 163 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_ORDER);
    }
	break;
	case 18:
#line 166 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_TRADE);
    }
	break;
	case 19:
#line 169 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_POSN);
    }
	break;
	case 20:
#line 172 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_VIEW);
    }
	break;
	case 21:
#line 186 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->action = DBR_ACTION_BUY;
    }
	break;
	case 22:
#line 189 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->action = DBR_ACTION_SELL;
    }
	break;
	case 23:
#line 198 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if ((rest->fields & DBR_METHOD_MASK) != 0) {
            cs = json_error; msg = "method already specified";
            {p++; goto _out; }
        }
    }
	break;
	case 24:
#line 204 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if ((rest->fields & DBR_RESRC_MASK) != 0) {
            cs = json_error; msg = "resrc already specified";
            {p++; goto _out; }
        }
    }
	break;
	case 25:
#line 210 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_ID) {
            cs = json_error; msg = "id already specified";
            {p++; goto _out; }
        }
    }
	break;
	case 26:
#line 216 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_param(rest, DBR_PARAM_ID);
        rest->id = inum(rest);
    }
	break;
	case 27:
#line 220 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_ACCNT) {
            cs = json_error; msg = "accnt already specified";
            {p++; goto _out; }
        }
        rest->str.buf = rest->accnt;
        rest->str.max = DBR_MNEM_MAX;
    }
	break;
	case 28:
#line 228 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_param(rest, DBR_PARAM_ACCNT);
    }
	break;
	case 29:
#line 231 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_GIVEUP) {
            cs = json_error; msg = "giveup already specified";
            {p++; goto _out; }
        }
        rest->str.buf = rest->giveup;
        rest->str.max = DBR_MNEM_MAX;
    }
	break;
	case 30:
#line 239 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_param(rest, DBR_PARAM_GIVEUP);
    }
	break;
	case 31:
#line 242 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_CONTR) {
            cs = json_error; msg = "contr already specified";
            {p++; goto _out; }
        }
        rest->str.buf = rest->contr;
        rest->str.max = DBR_MNEM_MAX;
    }
	break;
	case 32:
#line 250 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_param(rest, DBR_PARAM_CONTR);
    }
	break;
	case 33:
#line 253 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_SETTL_DATE) {
            cs = json_error; msg = "settl-date already specified";
            {p++; goto _out; }
        }
    }
	break;
	case 34:
#line 259 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_param(rest, DBR_PARAM_SETTL_DATE);
        rest->settl_date = inum(rest);
    }
	break;
	case 35:
#line 263 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_REF) {
            cs = json_error; msg = "ref already specified";
            {p++; goto _out; }
        }
        rest->str.buf = rest->ref;
        rest->str.max = DBR_REF_MAX;
    }
	break;
	case 36:
#line 271 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_param(rest, DBR_PARAM_REF);
    }
	break;
	case 37:
#line 274 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_ACTION) {
            cs = json_error; msg = "action already specified";
            {p++; goto _out; }
        }
    }
	break;
	case 38:
#line 280 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_param(rest, DBR_PARAM_ACTION);
    }
	break;
	case 39:
#line 283 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_TICKS) {
            cs = json_error; msg = "ticks already specified";
            {p++; goto _out; }
        }
    }
	break;
	case 40:
#line 289 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_param(rest, DBR_PARAM_TICKS);
        rest->ticks = inum(rest);
    }
	break;
	case 41:
#line 293 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_LOTS) {
            cs = json_error; msg = "lots already specified";
            {p++; goto _out; }
        }
    }
	break;
	case 42:
#line 299 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_param(rest, DBR_PARAM_LOTS);
        rest->lots = inum(rest);
    }
	break;
	case 43:
#line 303 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_MIN_LOTS) {
            cs = json_error; msg = "min-lots already specified";
            {p++; goto _out; }
        }
    }
	break;
	case 44:
#line 309 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_param(rest, DBR_PARAM_MIN_LOTS);
        rest->min_lots = inum(rest);
    }
	break;
#line 875 "/Users/marayl/repo/doobry/src/elm/rest.c"
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

#line 354 "/Users/marayl/repo/doobry/src/elm/rest.rl"
    rest->cs = cs;

	if (cs == json_error) {
        dbr_err_set(DBR_EINVAL, msg);
		return -1;
    }

	if (cs < json_first_final)
		return 0;

	return 1;
}


#line 487 "/Users/marayl/repo/doobry/src/elm/rest.rl"



#line 907 "/Users/marayl/repo/doobry/src/elm/rest.c"
static const char _rurl_actions[] = {
	0, 1, 1, 1, 3, 1, 7, 1, 
	11, 1, 12, 1, 13, 1, 14, 1, 
	20, 1, 21, 2, 0, 1, 2, 2, 
	3, 2, 6, 17, 2, 6, 18, 2, 
	7, 13, 2, 9, 14, 2, 9, 20, 
	3, 4, 8, 13, 3, 4, 8, 15, 
	3, 4, 8, 16, 3, 4, 8, 17, 
	3, 4, 8, 18, 3, 4, 8, 19, 
	3, 4, 10, 14, 3, 4, 10, 20, 
	4, 4, 8, 5, 17, 4, 4, 8, 
	5, 18
};

static const char _rurl_key_offsets[] = {
	0, 0, 1, 9, 10, 11, 12, 13, 
	14, 15, 16, 17, 18, 19, 20, 21, 
	22, 23, 24, 25, 26, 27, 29, 30, 
	31, 32, 33, 34, 35, 36, 37, 38, 
	39, 40, 41, 42, 43, 44, 45, 47, 
	48, 49, 50, 51, 52, 53, 54, 55, 
	56, 56, 57, 58, 59, 60, 61, 62, 
	63, 65, 68, 69, 70, 72, 75, 76, 
	77, 78
};

static const char _rurl_trans_keys[] = {
	47, 97, 99, 103, 108, 111, 112, 116, 
	118, 99, 99, 110, 116, 111, 110, 116, 
	114, 105, 118, 101, 117, 112, 47, 47, 
	111, 103, 111, 102, 110, 102, 114, 100, 
	101, 114, 47, 47, 111, 115, 110, 47, 
	47, 114, 97, 100, 101, 47, 114, 47, 
	47, 47, 105, 101, 119, 47, 47, 47, 
	47, 47, 47, 47, 47, 47, 47, 48, 
	57, 47, 48, 57, 47, 47, 48, 57, 
	47, 48, 57, 47, 47, 47, 47, 0
};

static const char _rurl_single_lengths[] = {
	0, 1, 8, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 2, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 2, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	0, 1, 1, 1, 1, 1, 1, 1, 
	0, 1, 1, 1, 0, 1, 1, 1, 
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
	1, 1, 0, 0, 1, 1, 0, 0, 
	0, 0
};

static const unsigned char _rurl_index_offsets[] = {
	0, 0, 2, 11, 13, 15, 17, 19, 
	21, 23, 25, 27, 29, 31, 33, 35, 
	37, 39, 41, 43, 45, 47, 50, 52, 
	54, 56, 58, 60, 62, 64, 66, 68, 
	70, 72, 74, 76, 78, 80, 82, 85, 
	87, 89, 91, 93, 95, 97, 99, 101, 
	103, 104, 106, 108, 110, 112, 114, 116, 
	118, 120, 123, 125, 127, 129, 132, 134, 
	136, 138
};

static const char _rurl_trans_targs[] = {
	2, 0, 3, 7, 11, 18, 23, 29, 
	34, 42, 0, 4, 0, 5, 0, 6, 
	0, 45, 0, 8, 0, 9, 0, 10, 
	0, 49, 0, 12, 0, 13, 0, 14, 
	0, 15, 0, 16, 0, 17, 0, 0, 
	52, 19, 0, 20, 0, 21, 0, 22, 
	54, 0, 53, 0, 24, 0, 25, 0, 
	26, 0, 27, 0, 28, 0, 0, 55, 
	30, 0, 31, 0, 32, 0, 33, 0, 
	0, 58, 35, 0, 36, 0, 37, 0, 
	38, 0, 39, 40, 0, 0, 59, 41, 
	0, 0, 62, 43, 0, 44, 0, 63, 
	0, 46, 0, 0, 47, 48, 47, 0, 
	50, 0, 0, 51, 48, 51, 48, 52, 
	48, 0, 48, 0, 56, 55, 57, 0, 
	48, 57, 0, 48, 58, 60, 59, 61, 
	0, 48, 61, 0, 48, 62, 64, 0, 
	0, 65, 48, 65, 0
};

static const char _rurl_trans_actions[] = {
	17, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 5, 0, 0, 
	22, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 5, 0, 0, 22, 
	0, 0, 0, 0, 0, 0, 5, 0, 
	0, 22, 0, 0, 0, 0, 0, 0, 
	0, 0, 5, 0, 0, 0, 22, 5, 
	0, 0, 22, 0, 0, 0, 0, 0, 
	0, 31, 0, 0, 22, 40, 3, 0, 
	34, 0, 0, 22, 64, 3, 48, 3, 
	9, 0, 7, 0, 72, 3, 19, 0, 
	25, 1, 0, 60, 3, 77, 3, 19, 
	0, 28, 1, 0, 44, 3, 37, 0, 
	0, 22, 68, 3, 0
};

static const char _rurl_eof_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 11, 0, 40, 
	0, 13, 0, 64, 48, 9, 7, 52, 
	0, 25, 60, 56, 0, 28, 44, 15, 
	0, 68
};

static const int rurl_start = 1;
static const int rurl_first_final = 45;
static const int rurl_error = 0;

static const int rurl_en_main = 1;


#line 490 "/Users/marayl/repo/doobry/src/elm/rest.rl"

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

    
#line 1063 "/Users/marayl/repo/doobry/src/elm/rest.c"
	{
	cs = rurl_start;
	}

#line 508 "/Users/marayl/repo/doobry/src/elm/rest.rl"
    
#line 1070 "/Users/marayl/repo/doobry/src/elm/rest.c"
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
#line 370 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        inum = 0;
    }
	break;
	case 1:
#line 373 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        inum = inum * 10 + ((*p) - '0');
    }
	break;
	case 2:
#line 378 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        str.count = 0;
    }
	break;
	case 3:
#line 381 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (str.count < str.max)
            str.buf[str.count++] = (*p);
        else {
            cs = rurl_error; msg = "max string length exceeded";
            {p++; goto _out; }
        }
    }
	break;
	case 4:
#line 389 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (str.count < str.max)
            str.buf[str.count] = '\0';
    }
	break;
	case 5:
#line 395 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_ID) {
            cs = rurl_error; msg = "id already specified";
            {p++; goto _out; }
        }
    }
	break;
	case 6:
#line 401 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_param(rest, DBR_PARAM_ID);
        rest->id = inum;
    }
	break;
	case 7:
#line 408 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_ACCNT) {
            cs = rurl_error; msg = "accnt already specified";
            {p++; goto _out; }
        }
        str.buf = rest->accnt;
        str.max = DBR_MNEM_MAX;
    }
	break;
	case 8:
#line 416 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_param(rest, DBR_PARAM_ACCNT);
    }
	break;
	case 9:
#line 423 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_CONTR) {
            cs = rurl_error; msg = "contr already specified";
            {p++; goto _out; }
        }
        str.buf = rest->contr;
        str.max = DBR_MNEM_MAX;
    }
	break;
	case 10:
#line 431 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_param(rest, DBR_PARAM_CONTR);
    }
	break;
	case 11:
#line 437 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_LOGON);
    }
	break;
	case 12:
#line 440 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_LOGOFF);
    }
	break;
	case 13:
#line 443 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_ACCNT);
    }
	break;
	case 14:
#line 446 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_CONTR);
    }
	break;
	case 15:
#line 449 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_TRADER);
    }
	break;
	case 16:
#line 452 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_GIVEUP);
    }
	break;
	case 17:
#line 455 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_ORDER);
    }
	break;
	case 18:
#line 458 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_TRADE);
    }
	break;
	case 19:
#line 461 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_POSN);
    }
	break;
	case 20:
#line 464 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_VIEW);
    }
	break;
	case 21:
#line 479 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if ((rest->fields & DBR_RESRC_MASK) != 0) {
            cs = rurl_error; msg = "resrc already specified";
            {p++; goto _out; }
        }
    }
	break;
#line 1298 "/Users/marayl/repo/doobry/src/elm/rest.c"
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
#line 389 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (str.count < str.max)
            str.buf[str.count] = '\0';
    }
	break;
	case 6:
#line 401 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_param(rest, DBR_PARAM_ID);
        rest->id = inum;
    }
	break;
	case 8:
#line 416 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_param(rest, DBR_PARAM_ACCNT);
    }
	break;
	case 10:
#line 431 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_param(rest, DBR_PARAM_CONTR);
    }
	break;
	case 11:
#line 437 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_LOGON);
    }
	break;
	case 12:
#line 440 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_LOGOFF);
    }
	break;
	case 13:
#line 443 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_ACCNT);
    }
	break;
	case 14:
#line 446 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_CONTR);
    }
	break;
	case 15:
#line 449 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_TRADER);
    }
	break;
	case 16:
#line 452 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_GIVEUP);
    }
	break;
	case 17:
#line 455 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_ORDER);
    }
	break;
	case 18:
#line 458 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_TRADE);
    }
	break;
	case 19:
#line 461 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_POSN);
    }
	break;
	case 20:
#line 464 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        dbr_rest_set_resrc(rest, DBR_RESRC_VIEW);
    }
	break;
#line 1400 "/Users/marayl/repo/doobry/src/elm/rest.c"
		}
	}
	}

	_out: {}
	}

#line 509 "/Users/marayl/repo/doobry/src/elm/rest.rl"

	if (cs == rurl_error) {
        dbr_err_set(DBR_EINVAL, msg);
		return -1;
    }

	if (cs < rurl_first_final)
		return 0;

	return 1;
}

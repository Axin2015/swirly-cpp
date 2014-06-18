
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

#pragma GCC diagnostic ignored "-Wunused-const-variable"

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


#line 335 "/Users/marayl/repo/doobry/src/elm/rest.rl"



#line 48 "/Users/marayl/repo/doobry/src/elm/rest.c"
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
	122, 127, 127, 129, 131, 133, 135, 136, 
	140, 144, 146, 151, 151, 153, 154, 158, 
	165, 167, 174, 176, 178, 180, 181, 185, 
	192, 194, 201, 205, 207, 209, 211, 213, 
	214, 218, 222, 230, 232, 234, 236, 238, 
	240, 241, 246, 248, 250, 251, 256, 258, 
	260, 262, 263, 268, 272, 274, 276, 277, 
	282, 284, 285, 290, 292, 293, 295, 297, 
	299, 301, 302, 306, 313, 315, 322, 324, 
	328, 329, 333, 337, 339, 344, 344, 346, 
	348, 349, 353, 357, 375, 377, 379, 381, 
	383, 384, 389, 391, 393, 395, 397, 398, 
	403, 405, 407, 409, 411, 412, 417, 419, 
	421, 423, 427, 429, 430, 435, 436, 441, 
	443, 445, 447, 449, 451, 452, 457, 459, 
	461, 463, 465, 466, 471, 473, 475, 477, 
	478, 483, 485, 487, 489, 491, 492, 497, 
	499, 501, 503, 504, 509, 511, 513, 515, 
	517, 518, 520, 522, 524, 526, 527, 531, 
	538, 540, 547, 549, 551, 553, 555, 556, 
	560, 567, 569, 576
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
	34, 92, 32, 44, 125, 9, 13, 82, 
	114, 79, 111, 85, 117, 80, 112, 34, 
	32, 58, 9, 13, 32, 34, 9, 13, 
	34, 92, 32, 44, 125, 9, 13, 68, 
	100, 34, 32, 58, 9, 13, 32, 43, 
	45, 9, 13, 48, 57, 48, 57, 32, 
	44, 125, 9, 13, 48, 57, 79, 111, 
	84, 116, 83, 115, 34, 32, 58, 9, 
	13, 32, 43, 45, 9, 13, 48, 57, 
	48, 57, 32, 44, 125, 9, 13, 48, 
	57, 69, 73, 101, 105, 84, 116, 72, 
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
	9, 13, 78, 110, 95, 76, 108, 79, 
	111, 84, 116, 83, 115, 34, 32, 58, 
	9, 13, 32, 43, 45, 9, 13, 48, 
	57, 48, 57, 32, 44, 125, 9, 13, 
	48, 57, 69, 101, 70, 83, 102, 115, 
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
	32, 44, 125, 9, 13, 69, 101, 84, 
	116, 84, 116, 76, 108, 95, 68, 100, 
	65, 97, 84, 116, 69, 101, 34, 32, 
	58, 9, 13, 32, 43, 45, 9, 13, 
	48, 57, 48, 57, 32, 44, 125, 9, 
	13, 48, 57, 73, 105, 67, 99, 75, 
	107, 83, 115, 34, 32, 58, 9, 13, 
	32, 43, 45, 9, 13, 48, 57, 48, 
	57, 32, 44, 125, 9, 13, 48, 57, 
	32, 9, 13, 0
};

static const char _json_single_lengths[] = {
	0, 2, 3, 18, 2, 4, 2, 2, 
	1, 2, 2, 2, 3, 3, 2, 0, 
	2, 2, 2, 1, 2, 2, 4, 2, 
	2, 1, 3, 2, 2, 2, 1, 3, 
	2, 2, 2, 2, 1, 2, 2, 2, 
	3, 0, 2, 2, 2, 2, 1, 2, 
	2, 2, 3, 0, 2, 1, 2, 3, 
	0, 3, 2, 2, 2, 1, 2, 3, 
	0, 3, 4, 2, 2, 2, 2, 1, 
	2, 2, 8, 2, 2, 2, 2, 2, 
	1, 3, 2, 2, 1, 3, 2, 2, 
	2, 1, 3, 4, 2, 2, 1, 3, 
	2, 1, 3, 2, 1, 2, 2, 2, 
	2, 1, 2, 3, 0, 3, 2, 4, 
	1, 2, 2, 2, 3, 0, 2, 2, 
	1, 2, 2, 18, 2, 2, 2, 2, 
	1, 3, 2, 2, 2, 2, 1, 3, 
	2, 2, 2, 2, 1, 3, 2, 2, 
	2, 4, 2, 1, 3, 1, 3, 2, 
	2, 2, 2, 2, 1, 3, 2, 2, 
	2, 2, 1, 3, 2, 2, 2, 1, 
	3, 2, 2, 2, 2, 1, 3, 2, 
	2, 2, 1, 3, 2, 2, 2, 2, 
	1, 2, 2, 2, 2, 1, 2, 3, 
	0, 3, 2, 2, 2, 2, 1, 2, 
	3, 0, 3, 1
};

static const char _json_range_lengths[] = {
	0, 1, 1, 0, 0, 0, 0, 0, 
	0, 1, 1, 0, 1, 1, 1, 0, 
	0, 0, 0, 0, 1, 1, 0, 0, 
	0, 0, 1, 0, 0, 0, 0, 1, 
	0, 0, 0, 0, 0, 1, 1, 0, 
	1, 0, 0, 0, 0, 0, 0, 1, 
	1, 0, 1, 0, 0, 0, 1, 2, 
	1, 2, 0, 0, 0, 0, 1, 2, 
	1, 2, 0, 0, 0, 0, 0, 0, 
	1, 1, 0, 0, 0, 0, 0, 0, 
	0, 1, 0, 0, 0, 1, 0, 0, 
	0, 0, 1, 0, 0, 0, 0, 1, 
	0, 0, 1, 0, 0, 0, 0, 0, 
	0, 0, 1, 2, 1, 2, 0, 0, 
	0, 1, 1, 0, 1, 0, 0, 0, 
	0, 1, 1, 0, 0, 0, 0, 0, 
	0, 1, 0, 0, 0, 0, 0, 1, 
	0, 0, 0, 0, 0, 1, 0, 0, 
	0, 0, 0, 0, 1, 0, 1, 0, 
	0, 0, 0, 0, 0, 1, 0, 0, 
	0, 0, 0, 1, 0, 0, 0, 0, 
	1, 0, 0, 0, 0, 0, 1, 0, 
	0, 0, 0, 1, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 1, 2, 
	1, 2, 0, 0, 0, 0, 0, 1, 
	2, 1, 2, 1
};

static const short _json_index_offsets[] = {
	0, 0, 4, 9, 28, 31, 36, 39, 
	42, 44, 48, 52, 55, 60, 65, 69, 
	70, 73, 76, 79, 81, 85, 89, 94, 
	97, 100, 102, 107, 110, 113, 116, 118, 
	123, 126, 129, 132, 135, 137, 141, 145, 
	148, 153, 154, 157, 160, 163, 166, 168, 
	172, 176, 179, 184, 185, 188, 190, 194, 
	200, 202, 208, 211, 214, 217, 219, 223, 
	229, 231, 237, 242, 245, 248, 251, 254, 
	256, 260, 264, 273, 276, 279, 282, 285, 
	288, 290, 295, 298, 301, 303, 308, 311, 
	314, 317, 319, 324, 329, 332, 335, 337, 
	342, 345, 347, 352, 355, 357, 360, 363, 
	366, 369, 371, 375, 381, 383, 389, 392, 
	397, 399, 403, 407, 410, 415, 416, 419, 
	422, 424, 428, 432, 451, 454, 457, 460, 
	463, 465, 470, 473, 476, 479, 482, 484, 
	489, 492, 495, 498, 501, 503, 508, 511, 
	514, 517, 522, 525, 527, 532, 534, 539, 
	542, 545, 548, 551, 554, 556, 561, 564, 
	567, 570, 573, 575, 580, 583, 586, 589, 
	591, 596, 599, 602, 605, 608, 610, 615, 
	618, 621, 624, 626, 631, 634, 637, 640, 
	643, 645, 648, 651, 654, 657, 659, 663, 
	669, 671, 677, 680, 683, 686, 689, 691, 
	695, 701, 703, 709
};

static const unsigned char _json_trans_targs[] = {
	1, 2, 1, 0, 2, 3, 203, 2, 
	0, 4, 32, 42, 52, 58, 66, 110, 
	180, 194, 4, 32, 42, 52, 58, 66, 
	110, 180, 194, 0, 5, 5, 0, 6, 
	16, 6, 16, 0, 7, 7, 0, 8, 
	8, 0, 9, 0, 9, 10, 9, 0, 
	10, 11, 10, 0, 12, 15, 11, 13, 
	14, 203, 13, 0, 13, 14, 203, 13, 
	0, 14, 3, 14, 0, 11, 17, 17, 
	0, 18, 18, 0, 19, 19, 0, 20, 
	0, 20, 21, 20, 0, 21, 22, 21, 
	0, 23, 27, 23, 27, 0, 24, 24, 
	0, 25, 25, 0, 26, 0, 13, 14, 
	203, 13, 0, 28, 28, 0, 29, 29, 
	0, 30, 30, 0, 31, 0, 13, 14, 
	203, 13, 0, 33, 33, 0, 34, 34, 
	0, 35, 35, 0, 36, 36, 0, 37, 
	0, 37, 38, 37, 0, 38, 39, 38, 
	0, 40, 41, 39, 13, 14, 203, 13, 
	0, 39, 43, 43, 0, 44, 44, 0, 
	45, 45, 0, 46, 46, 0, 47, 0, 
	47, 48, 47, 0, 48, 49, 48, 0, 
	50, 51, 49, 13, 14, 203, 13, 0, 
	49, 53, 53, 0, 54, 0, 54, 55, 
	54, 0, 55, 56, 56, 55, 57, 0, 
	57, 0, 13, 14, 203, 13, 57, 0, 
	59, 59, 0, 60, 60, 0, 61, 61, 
	0, 62, 0, 62, 63, 62, 0, 63, 
	64, 64, 63, 65, 0, 65, 0, 13, 
	14, 203, 13, 65, 0, 67, 99, 67, 
	99, 0, 68, 68, 0, 69, 69, 0, 
	70, 70, 0, 71, 71, 0, 72, 0, 
	72, 73, 72, 0, 73, 74, 73, 0, 
	75, 82, 86, 91, 75, 82, 86, 91, 
	0, 76, 76, 0, 77, 77, 0, 78, 
	78, 0, 79, 79, 0, 80, 80, 0, 
	81, 0, 13, 14, 203, 13, 0, 83, 
	83, 0, 84, 84, 0, 85, 0, 13, 
	14, 203, 13, 0, 87, 87, 0, 88, 
	88, 0, 89, 89, 0, 90, 0, 13, 
	14, 203, 13, 0, 92, 96, 92, 96, 
	0, 93, 93, 0, 94, 94, 0, 95, 
	0, 13, 14, 203, 13, 0, 97, 97, 
	0, 98, 0, 13, 14, 203, 13, 0, 
	100, 100, 0, 101, 0, 102, 102, 0, 
	103, 103, 0, 104, 104, 0, 105, 105, 
	0, 106, 0, 106, 107, 106, 0, 107, 
	108, 108, 107, 109, 0, 109, 0, 13, 
	14, 203, 13, 109, 0, 111, 111, 0, 
	112, 118, 112, 118, 0, 113, 0, 113, 
	114, 113, 0, 114, 115, 114, 0, 116, 
	117, 115, 13, 14, 203, 13, 0, 115, 
	119, 119, 0, 120, 120, 0, 121, 0, 
	121, 122, 121, 0, 122, 123, 122, 0, 
	124, 130, 136, 142, 151, 158, 164, 169, 
	175, 124, 130, 136, 142, 151, 158, 164, 
	169, 175, 0, 125, 125, 0, 126, 126, 
	0, 127, 127, 0, 128, 128, 0, 129, 
	0, 13, 14, 203, 13, 0, 131, 131, 
	0, 132, 132, 0, 133, 133, 0, 134, 
	134, 0, 135, 0, 13, 14, 203, 13, 
	0, 137, 137, 0, 138, 138, 0, 139, 
	139, 0, 140, 140, 0, 141, 0, 13, 
	14, 203, 13, 0, 143, 143, 0, 144, 
	144, 0, 145, 145, 0, 146, 149, 146, 
	149, 0, 147, 147, 0, 148, 0, 13, 
	14, 203, 13, 0, 150, 0, 13, 14, 
	203, 13, 0, 152, 152, 0, 153, 153, 
	0, 154, 154, 0, 155, 155, 0, 156, 
	156, 0, 157, 0, 13, 14, 203, 13, 
	0, 159, 159, 0, 160, 160, 0, 161, 
	161, 0, 162, 162, 0, 163, 0, 13, 
	14, 203, 13, 0, 165, 165, 0, 166, 
	166, 0, 167, 167, 0, 168, 0, 13, 
	14, 203, 13, 0, 170, 170, 0, 171, 
	171, 0, 172, 172, 0, 173, 173, 0, 
	174, 0, 13, 14, 203, 13, 0, 176, 
	176, 0, 177, 177, 0, 178, 178, 0, 
	179, 0, 13, 14, 203, 13, 0, 181, 
	181, 0, 182, 182, 0, 183, 183, 0, 
	184, 184, 0, 185, 0, 186, 186, 0, 
	187, 187, 0, 188, 188, 0, 189, 189, 
	0, 190, 0, 190, 191, 190, 0, 191, 
	192, 192, 191, 193, 0, 193, 0, 13, 
	14, 203, 13, 193, 0, 195, 195, 0, 
	196, 196, 0, 197, 197, 0, 198, 198, 
	0, 199, 0, 199, 200, 199, 0, 200, 
	201, 201, 200, 202, 0, 202, 0, 13, 
	14, 203, 13, 202, 0, 203, 203, 0, 
	0
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
	0, 0, 0, 0, 0, 75, 0, 0, 
	0, 0, 3, 54, 54, 54, 54, 0, 
	3, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 69, 96, 0, 100, 0, 
	1, 0, 39, 39, 39, 39, 1, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	90, 120, 0, 124, 0, 1, 0, 47, 
	47, 47, 47, 1, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 35, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 5, 5, 5, 5, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 7, 
	7, 7, 7, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 9, 
	9, 9, 9, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 11, 11, 11, 11, 0, 0, 0, 
	0, 0, 0, 13, 13, 13, 13, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	93, 128, 0, 132, 0, 1, 0, 49, 
	49, 49, 49, 1, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 84, 0, 0, 0, 
	0, 3, 60, 60, 60, 60, 0, 3, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 37, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 19, 19, 19, 19, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 21, 21, 21, 21, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 25, 
	25, 25, 25, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 17, 
	17, 17, 17, 0, 0, 0, 15, 15, 
	15, 15, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 33, 33, 33, 33, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 27, 
	27, 27, 27, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 31, 
	31, 31, 31, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 29, 29, 29, 29, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 23, 23, 23, 23, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	81, 104, 0, 108, 0, 1, 0, 41, 
	41, 41, 41, 1, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	87, 112, 0, 116, 0, 1, 0, 45, 
	45, 45, 45, 1, 0, 0, 0, 0, 
	0
};

static const int json_start = 1;
static const int json_first_final = 203;
static const int json_error = 0;

static const int json_en_main = 1;


#line 338 "/Users/marayl/repo/doobry/src/elm/rest.rl"

DBR_API void
dbr_rest_init(struct DbrRest* rest)
{
    int cs;
    
#line 462 "/Users/marayl/repo/doobry/src/elm/rest.c"
	{
	cs = json_start;
	}

#line 344 "/Users/marayl/repo/doobry/src/elm/rest.rl"
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
	
#line 481 "/Users/marayl/repo/doobry/src/elm/rest.c"
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
#line 43 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->num.sign = 1;
        rest->num.idigits = 0;
        rest->num.fdigits = 0;
        rest->num.fcount = 0;
    }
	break;
	case 1:
#line 49 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->num.sign = -1;
    }
	break;
	case 2:
#line 52 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->num.idigits = rest->num.idigits * 10 + ((*p) - '0');
    }
	break;
	case 3:
#line 100 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->str.count = 0;
    }
	break;
	case 4:
#line 103 "/Users/marayl/repo/doobry/src/elm/rest.rl"
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
#line 111 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->str.count < rest->str.max)
            rest->str.buf[rest->str.count] = '\0';
    }
	break;
	case 6:
#line 126 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_METHOD_MASK) | DBR_METHOD_DELETE;
    }
	break;
	case 7:
#line 129 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_METHOD_MASK) | DBR_METHOD_GET;
    }
	break;
	case 8:
#line 132 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_METHOD_MASK) | DBR_METHOD_HEAD;
    }
	break;
	case 9:
#line 135 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_METHOD_MASK) | DBR_METHOD_POST;
    }
	break;
	case 10:
#line 138 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_METHOD_MASK) | DBR_METHOD_PUT;
    }
	break;
	case 11:
#line 147 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_LOGON;
    }
	break;
	case 12:
#line 150 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_LOGOFF;
    }
	break;
	case 13:
#line 153 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_ACCNT;
    }
	break;
	case 14:
#line 156 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_CONTR;
    }
	break;
	case 15:
#line 159 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_USER;
    }
	break;
	case 16:
#line 162 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_GROUP;
    }
	break;
	case 17:
#line 165 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_ORDER;
    }
	break;
	case 18:
#line 168 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_TRADE;
    }
	break;
	case 19:
#line 171 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_POSN;
    }
	break;
	case 20:
#line 174 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_MARKET;
    }
	break;
	case 21:
#line 188 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->action = DBR_ACTION_BUY;
    }
	break;
	case 22:
#line 191 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->action = DBR_ACTION_SELL;
    }
	break;
	case 23:
#line 200 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if ((rest->fields & DBR_METHOD_MASK) != 0) {
            cs = json_error; msg = "method already specified";
            {p++; goto _out; }
        }
    }
	break;
	case 24:
#line 206 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if ((rest->fields & DBR_RESRC_MASK) != 0) {
            cs = json_error; msg = "resrc already specified";
            {p++; goto _out; }
        }
    }
	break;
	case 25:
#line 212 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_ID) {
            cs = json_error; msg = "id already specified";
            {p++; goto _out; }
        }
    }
	break;
	case 26:
#line 218 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_ID;
        rest->id = inum(rest);
    }
	break;
	case 27:
#line 222 "/Users/marayl/repo/doobry/src/elm/rest.rl"
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
#line 230 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_ACCNT;
    }
	break;
	case 29:
#line 233 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_GROUP) {
            cs = json_error; msg = "group already specified";
            {p++; goto _out; }
        }
        rest->str.buf = rest->group;
        rest->str.max = DBR_MNEM_MAX;
    }
	break;
	case 30:
#line 241 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_GROUP;
    }
	break;
	case 31:
#line 244 "/Users/marayl/repo/doobry/src/elm/rest.rl"
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
#line 252 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_CONTR;
    }
	break;
	case 33:
#line 255 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_SETTL_DATE) {
            cs = json_error; msg = "settl-date already specified";
            {p++; goto _out; }
        }
    }
	break;
	case 34:
#line 261 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_SETTL_DATE;
        rest->settl_date = inum(rest);
    }
	break;
	case 35:
#line 265 "/Users/marayl/repo/doobry/src/elm/rest.rl"
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
#line 273 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_REF;
    }
	break;
	case 37:
#line 276 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_ACTION) {
            cs = json_error; msg = "action already specified";
            {p++; goto _out; }
        }
    }
	break;
	case 38:
#line 282 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_ACTION;
        rest->action = inum(rest);
    }
	break;
	case 39:
#line 286 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_TICKS) {
            cs = json_error; msg = "ticks already specified";
            {p++; goto _out; }
        }
    }
	break;
	case 40:
#line 292 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_TICKS;
        rest->ticks = inum(rest);
    }
	break;
	case 41:
#line 296 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_LOTS) {
            cs = json_error; msg = "lots already specified";
            {p++; goto _out; }
        }
    }
	break;
	case 42:
#line 302 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_LOTS;
        rest->lots = inum(rest);
    }
	break;
	case 43:
#line 306 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_MIN_LOTS) {
            cs = json_error; msg = "min-lots already specified";
            {p++; goto _out; }
        }
    }
	break;
	case 44:
#line 312 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_MIN_LOTS;
        rest->min_lots = inum(rest);
    }
	break;
#line 883 "/Users/marayl/repo/doobry/src/elm/rest.c"
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

#line 357 "/Users/marayl/repo/doobry/src/elm/rest.rl"
    rest->cs = cs;

	if (cs == json_error) {
        dbr_err_set(DBR_EINVAL, msg);
		return -1;
    }

	if (cs < json_first_final)
		return 0;

	return 1;
}


#line 503 "/Users/marayl/repo/doobry/src/elm/rest.rl"



#line 915 "/Users/marayl/repo/doobry/src/elm/rest.c"
static const char _rurl_actions[] = {
	0, 1, 1, 1, 3, 1, 7, 1, 
	9, 1, 13, 1, 14, 1, 15, 1, 
	16, 1, 22, 1, 23, 2, 0, 1, 
	2, 2, 3, 2, 6, 19, 2, 6, 
	20, 2, 7, 15, 2, 11, 16, 2, 
	11, 22, 3, 4, 8, 15, 3, 4, 
	8, 17, 3, 4, 8, 18, 3, 4, 
	8, 19, 3, 4, 8, 20, 3, 4, 
	10, 21, 3, 4, 12, 16, 3, 4, 
	12, 22, 4, 4, 8, 5, 19, 4, 
	4, 8, 5, 20
};

static const char _rurl_key_offsets[] = {
	0, 0, 1, 10, 11, 12, 13, 14, 
	15, 16, 17, 18, 19, 20, 21, 22, 
	23, 24, 25, 26, 27, 29, 30, 31, 
	32, 33, 34, 35, 36, 37, 38, 39, 
	40, 41, 42, 43, 44, 45, 46, 47, 
	48, 49, 50, 51, 52, 53, 54, 55, 
	56, 57, 58, 59, 60, 60, 61, 62, 
	63, 64, 65, 66, 67, 68, 69, 70, 
	72, 75, 76, 77, 79, 82
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
	47, 47, 47, 47, 47, 47, 48, 57, 
	47, 48, 57, 47, 47, 48, 57, 47, 
	48, 57, 47, 0
};

static const char _rurl_single_lengths[] = {
	0, 1, 9, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 2, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 0, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 0, 
	1, 1, 1, 0, 1, 1
};

static const char _rurl_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 1, 
	1, 0, 0, 1, 1, 0
};

static const unsigned char _rurl_index_offsets[] = {
	0, 0, 2, 12, 14, 16, 18, 20, 
	22, 24, 26, 28, 30, 32, 34, 36, 
	38, 40, 42, 44, 46, 49, 51, 53, 
	55, 57, 59, 61, 63, 65, 67, 69, 
	71, 73, 75, 77, 79, 81, 83, 85, 
	87, 89, 91, 93, 95, 97, 99, 101, 
	103, 105, 107, 109, 111, 112, 114, 116, 
	118, 120, 122, 124, 126, 128, 130, 132, 
	134, 137, 139, 141, 143, 146
};

static const char _rurl_trans_targs[] = {
	2, 0, 3, 7, 11, 17, 22, 27, 
	33, 38, 44, 0, 4, 0, 5, 0, 
	6, 0, 49, 0, 8, 0, 9, 0, 
	10, 0, 53, 0, 12, 0, 13, 0, 
	14, 0, 15, 0, 16, 0, 0, 56, 
	18, 0, 19, 0, 20, 0, 21, 58, 
	0, 57, 0, 23, 0, 24, 0, 25, 
	0, 26, 0, 59, 0, 28, 0, 29, 
	0, 30, 0, 31, 0, 32, 0, 0, 
	62, 34, 0, 35, 0, 36, 0, 37, 
	0, 0, 65, 39, 0, 40, 0, 41, 
	0, 42, 0, 43, 0, 0, 66, 45, 
	0, 46, 0, 47, 0, 48, 0, 0, 
	69, 50, 0, 0, 51, 52, 51, 0, 
	54, 0, 0, 55, 52, 55, 52, 56, 
	52, 0, 52, 0, 60, 0, 0, 61, 
	52, 61, 63, 62, 64, 0, 52, 64, 
	0, 52, 65, 67, 66, 68, 0, 52, 
	68, 0, 52, 69, 0
};

static const char _rurl_trans_actions[] = {
	19, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 5, 0, 0, 24, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 5, 0, 0, 
	24, 0, 0, 0, 0, 0, 0, 7, 
	0, 0, 24, 0, 0, 0, 0, 0, 
	0, 0, 0, 5, 0, 0, 24, 0, 
	0, 0, 0, 0, 0, 5, 0, 0, 
	24, 33, 0, 0, 24, 42, 3, 0, 
	36, 0, 0, 24, 66, 3, 50, 3, 
	11, 0, 9, 0, 39, 0, 0, 24, 
	70, 3, 74, 3, 21, 0, 27, 1, 
	0, 62, 3, 79, 3, 21, 0, 30, 
	1, 0, 46, 3, 0
};

static const char _rurl_eof_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 13, 0, 42, 0, 15, 0, 66, 
	50, 11, 9, 17, 0, 70, 54, 0, 
	27, 62, 58, 0, 30, 46
};

static const int rurl_start = 1;
static const int rurl_first_final = 49;
static const int rurl_error = 0;

static const int rurl_en_main = 1;


#line 506 "/Users/marayl/repo/doobry/src/elm/rest.rl"

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

    
#line 1074 "/Users/marayl/repo/doobry/src/elm/rest.c"
	{
	cs = rurl_start;
	}

#line 524 "/Users/marayl/repo/doobry/src/elm/rest.rl"
    
#line 1081 "/Users/marayl/repo/doobry/src/elm/rest.c"
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
#line 373 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        inum = 0;
    }
	break;
	case 1:
#line 376 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        inum = inum * 10 + ((*p) - '0');
    }
	break;
	case 2:
#line 381 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        str.count = 0;
    }
	break;
	case 3:
#line 384 "/Users/marayl/repo/doobry/src/elm/rest.rl"
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
#line 392 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (str.count < str.max)
            str.buf[str.count] = '\0';
    }
	break;
	case 5:
#line 398 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_ID) {
            cs = rurl_error; msg = "id already specified";
            {p++; goto _out; }
        }
    }
	break;
	case 6:
#line 404 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_ID;
        rest->id = inum;
    }
	break;
	case 7:
#line 411 "/Users/marayl/repo/doobry/src/elm/rest.rl"
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
#line 419 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_ACCNT;
    }
	break;
	case 9:
#line 426 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_GROUP) {
            cs = rurl_error; msg = "group already specified";
            {p++; goto _out; }
        }
        str.buf = rest->group;
        str.max = DBR_MNEM_MAX;
    }
	break;
	case 10:
#line 434 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_GROUP;
    }
	break;
	case 11:
#line 439 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (rest->fields & DBR_PARAM_CONTR) {
            cs = rurl_error; msg = "contr already specified";
            {p++; goto _out; }
        }
        str.buf = rest->contr;
        str.max = DBR_MNEM_MAX;
    }
	break;
	case 12:
#line 447 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_CONTR;
    }
	break;
	case 13:
#line 453 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_LOGON;
    }
	break;
	case 14:
#line 456 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_LOGOFF;
    }
	break;
	case 15:
#line 459 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_ACCNT;
    }
	break;
	case 16:
#line 462 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_CONTR;
    }
	break;
	case 17:
#line 465 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_USER;
    }
	break;
	case 18:
#line 468 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_GROUP;
    }
	break;
	case 19:
#line 471 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_ORDER;
    }
	break;
	case 20:
#line 474 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_TRADE;
    }
	break;
	case 21:
#line 477 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_POSN;
    }
	break;
	case 22:
#line 480 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_MARKET;
    }
	break;
	case 23:
#line 495 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if ((rest->fields & DBR_RESRC_MASK) != 0) {
            cs = rurl_error; msg = "resrc already specified";
            {p++; goto _out; }
        }
    }
	break;
#line 1326 "/Users/marayl/repo/doobry/src/elm/rest.c"
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
#line 392 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        if (str.count < str.max)
            str.buf[str.count] = '\0';
    }
	break;
	case 6:
#line 404 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_ID;
        rest->id = inum;
    }
	break;
	case 8:
#line 419 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_ACCNT;
    }
	break;
	case 10:
#line 434 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_GROUP;
    }
	break;
	case 12:
#line 447 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields |= DBR_PARAM_CONTR;
    }
	break;
	case 13:
#line 453 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_LOGON;
    }
	break;
	case 14:
#line 456 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_LOGOFF;
    }
	break;
	case 15:
#line 459 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_ACCNT;
    }
	break;
	case 16:
#line 462 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_CONTR;
    }
	break;
	case 17:
#line 465 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_USER;
    }
	break;
	case 18:
#line 468 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_GROUP;
    }
	break;
	case 19:
#line 471 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_ORDER;
    }
	break;
	case 20:
#line 474 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_TRADE;
    }
	break;
	case 21:
#line 477 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_POSN;
    }
	break;
	case 22:
#line 480 "/Users/marayl/repo/doobry/src/elm/rest.rl"
	{
        rest->fields = (rest->fields & ~DBR_RESRC_MASK) | DBR_RESRC_MARKET;
    }
	break;
#line 1434 "/Users/marayl/repo/doobry/src/elm/rest.c"
		}
	}
	}

	_out: {}
	}

#line 525 "/Users/marayl/repo/doobry/src/elm/rest.rl"

	if (cs == rurl_error) {
        dbr_err_set(DBR_EINVAL, msg);
		return -1;
    }

	if (cs < rurl_first_final)
		return 0;

	return 1;
}

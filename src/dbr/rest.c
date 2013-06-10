
#line 1 "/home/marayl/proj/doobry/src/dbr/rest.rl"
// -*- c -*-
/*
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


#line 322 "/home/marayl/proj/doobry/src/dbr/rest.rl"



#line 46 "/home/marayl/proj/doobry/src/dbr/rest.c"
static const char _json_actions[] = {
	0, 1, 2, 1, 4, 1, 6, 1, 
	7, 1, 8, 1, 9, 1, 10, 1, 
	11, 1, 12, 1, 13, 1, 14, 1, 
	15, 1, 16, 1, 17, 1, 20, 1, 
	21, 1, 23, 1, 34, 1, 37, 1, 
	39, 2, 5, 25, 2, 5, 27, 2, 
	5, 29, 2, 5, 31, 2, 5, 33, 
	2, 18, 35, 2, 19, 35, 2, 22, 
	0, 2, 24, 3, 2, 26, 3, 2, 
	28, 3, 2, 30, 3, 2, 32, 3, 
	2, 36, 0, 2, 38, 0, 3, 22, 
	0, 1, 3, 22, 0, 2, 3, 36, 
	0, 1, 3, 36, 0, 2, 3, 38, 
	0, 1, 3, 38, 0, 2
};

static const short _json_key_offsets[] = {
	0, 0, 4, 9, 23, 25, 29, 31, 
	33, 34, 38, 42, 44, 49, 54, 58, 
	58, 60, 62, 64, 65, 69, 73, 77, 
	79, 81, 82, 87, 89, 91, 93, 94, 
	99, 101, 103, 105, 107, 109, 110, 114, 
	118, 130, 132, 134, 136, 138, 139, 144, 
	146, 148, 150, 152, 153, 158, 160, 162, 
	164, 166, 168, 169, 174, 176, 178, 180, 
	182, 183, 188, 190, 192, 194, 195, 200, 
	202, 204, 206, 208, 211, 216, 217, 222, 
	224, 225, 229, 236, 238, 245, 247, 249, 
	251, 252, 256, 263, 265, 272, 278, 280, 
	282, 284, 286, 287, 291, 295, 297, 302, 
	302, 304, 306, 308, 310, 311, 315, 319, 
	327, 329, 331, 333, 335, 337, 338, 343, 
	345, 347, 348, 353, 355, 357, 359, 360, 
	365, 369, 371, 373, 374, 379, 381, 382, 
	387, 389, 391, 392, 396, 400, 402, 407, 
	407, 409, 411, 412, 416, 420, 422, 427, 
	427, 431, 433, 435, 437, 438, 442, 449, 
	451, 458, 460, 462, 464, 466, 467, 471, 
	475, 477, 482, 482
};

static const char _json_trans_keys[] = {
	32, 123, 9, 13, 32, 34, 125, 9, 
	13, 65, 69, 73, 76, 77, 82, 84, 
	97, 101, 105, 108, 109, 114, 116, 67, 
	99, 67, 84, 99, 116, 78, 110, 84, 
	116, 34, 32, 58, 9, 13, 32, 34, 
	9, 13, 34, 92, 32, 44, 125, 9, 
	13, 32, 44, 125, 9, 13, 32, 34, 
	9, 13, 73, 105, 79, 111, 78, 110, 
	34, 32, 58, 9, 13, 32, 34, 9, 
	13, 66, 83, 98, 115, 85, 117, 89, 
	121, 34, 32, 44, 125, 9, 13, 69, 
	101, 76, 108, 76, 108, 34, 32, 44, 
	125, 9, 13, 78, 110, 84, 116, 73, 
	105, 84, 116, 89, 121, 34, 32, 58, 
	9, 13, 32, 34, 9, 13, 65, 73, 
	77, 79, 80, 84, 97, 105, 109, 111, 
	112, 116, 67, 99, 67, 99, 78, 110, 
	84, 116, 34, 32, 44, 125, 9, 13, 
	78, 110, 83, 115, 84, 116, 82, 114, 
	34, 32, 44, 125, 9, 13, 65, 97, 
	82, 114, 75, 107, 69, 101, 84, 116, 
	34, 32, 44, 125, 9, 13, 82, 114, 
	68, 100, 69, 101, 82, 114, 34, 32, 
	44, 125, 9, 13, 79, 111, 83, 115, 
	78, 110, 34, 32, 44, 125, 9, 13, 
	82, 114, 65, 97, 68, 100, 69, 101, 
	34, 82, 114, 32, 44, 125, 9, 13, 
	34, 32, 44, 125, 9, 13, 68, 100, 
	34, 32, 58, 9, 13, 32, 43, 45, 
	9, 13, 48, 57, 48, 57, 32, 44, 
	125, 9, 13, 48, 57, 79, 111, 84, 
	116, 83, 115, 34, 32, 58, 9, 13, 
	32, 43, 45, 9, 13, 48, 57, 48, 
	57, 32, 44, 125, 9, 13, 48, 57, 
	65, 69, 78, 97, 101, 110, 82, 114, 
	75, 107, 69, 101, 84, 116, 34, 32, 
	58, 9, 13, 32, 34, 9, 13, 34, 
	92, 32, 44, 125, 9, 13, 84, 116, 
	72, 104, 79, 111, 68, 100, 34, 32, 
	58, 9, 13, 32, 34, 9, 13, 68, 
	71, 72, 80, 100, 103, 104, 112, 69, 
	101, 76, 108, 69, 101, 84, 116, 69, 
	101, 34, 32, 44, 125, 9, 13, 69, 
	101, 84, 116, 34, 32, 44, 125, 9, 
	13, 69, 101, 65, 97, 68, 100, 34, 
	32, 44, 125, 9, 13, 79, 85, 111, 
	117, 83, 115, 84, 116, 34, 32, 44, 
	125, 9, 13, 84, 116, 34, 32, 44, 
	125, 9, 13, 69, 101, 77, 109, 34, 
	32, 58, 9, 13, 32, 34, 9, 13, 
	34, 92, 32, 44, 125, 9, 13, 69, 
	101, 70, 102, 34, 32, 58, 9, 13, 
	32, 34, 9, 13, 34, 92, 32, 44, 
	125, 9, 13, 73, 82, 105, 114, 67, 
	99, 75, 107, 83, 115, 34, 32, 58, 
	9, 13, 32, 43, 45, 9, 13, 48, 
	57, 48, 57, 32, 44, 125, 9, 13, 
	48, 57, 65, 97, 68, 100, 69, 101, 
	82, 114, 34, 32, 58, 9, 13, 32, 
	34, 9, 13, 34, 92, 32, 44, 125, 
	9, 13, 32, 9, 13, 0
};

static const char _json_single_lengths[] = {
	0, 2, 3, 14, 2, 4, 2, 2, 
	1, 2, 2, 2, 3, 3, 2, 0, 
	2, 2, 2, 1, 2, 2, 4, 2, 
	2, 1, 3, 2, 2, 2, 1, 3, 
	2, 2, 2, 2, 2, 1, 2, 2, 
	12, 2, 2, 2, 2, 1, 3, 2, 
	2, 2, 2, 1, 3, 2, 2, 2, 
	2, 2, 1, 3, 2, 2, 2, 2, 
	1, 3, 2, 2, 2, 1, 3, 2, 
	2, 2, 2, 3, 3, 1, 3, 2, 
	1, 2, 3, 0, 3, 2, 2, 2, 
	1, 2, 3, 0, 3, 6, 2, 2, 
	2, 2, 1, 2, 2, 2, 3, 0, 
	2, 2, 2, 2, 1, 2, 2, 8, 
	2, 2, 2, 2, 2, 1, 3, 2, 
	2, 1, 3, 2, 2, 2, 1, 3, 
	4, 2, 2, 1, 3, 2, 1, 3, 
	2, 2, 1, 2, 2, 2, 3, 0, 
	2, 2, 1, 2, 2, 2, 3, 0, 
	4, 2, 2, 2, 1, 2, 3, 0, 
	3, 2, 2, 2, 2, 1, 2, 2, 
	2, 3, 0, 1
};

static const char _json_range_lengths[] = {
	0, 1, 1, 0, 0, 0, 0, 0, 
	0, 1, 1, 0, 1, 1, 1, 0, 
	0, 0, 0, 0, 1, 1, 0, 0, 
	0, 0, 1, 0, 0, 0, 0, 1, 
	0, 0, 0, 0, 0, 0, 1, 1, 
	0, 0, 0, 0, 0, 0, 1, 0, 
	0, 0, 0, 0, 1, 0, 0, 0, 
	0, 0, 0, 1, 0, 0, 0, 0, 
	0, 1, 0, 0, 0, 0, 1, 0, 
	0, 0, 0, 0, 1, 0, 1, 0, 
	0, 1, 2, 1, 2, 0, 0, 0, 
	0, 1, 2, 1, 2, 0, 0, 0, 
	0, 0, 0, 1, 1, 0, 1, 0, 
	0, 0, 0, 0, 0, 1, 1, 0, 
	0, 0, 0, 0, 0, 0, 1, 0, 
	0, 0, 1, 0, 0, 0, 0, 1, 
	0, 0, 0, 0, 1, 0, 0, 1, 
	0, 0, 0, 1, 1, 0, 1, 0, 
	0, 0, 0, 1, 1, 0, 1, 0, 
	0, 0, 0, 0, 0, 1, 2, 1, 
	2, 0, 0, 0, 0, 0, 1, 1, 
	0, 1, 0, 1
};

static const short _json_index_offsets[] = {
	0, 0, 4, 9, 24, 27, 32, 35, 
	38, 40, 44, 48, 51, 56, 61, 65, 
	66, 69, 72, 75, 77, 81, 85, 90, 
	93, 96, 98, 103, 106, 109, 112, 114, 
	119, 122, 125, 128, 131, 134, 136, 140, 
	144, 157, 160, 163, 166, 169, 171, 176, 
	179, 182, 185, 188, 190, 195, 198, 201, 
	204, 207, 210, 212, 217, 220, 223, 226, 
	229, 231, 236, 239, 242, 245, 247, 252, 
	255, 258, 261, 264, 268, 273, 275, 280, 
	283, 285, 289, 295, 297, 303, 306, 309, 
	312, 314, 318, 324, 326, 332, 339, 342, 
	345, 348, 351, 353, 357, 361, 364, 369, 
	370, 373, 376, 379, 382, 384, 388, 392, 
	401, 404, 407, 410, 413, 416, 418, 423, 
	426, 429, 431, 436, 439, 442, 445, 447, 
	452, 457, 460, 463, 465, 470, 473, 475, 
	480, 483, 486, 488, 492, 496, 499, 504, 
	505, 508, 511, 513, 517, 521, 524, 529, 
	530, 535, 538, 541, 544, 546, 550, 556, 
	558, 564, 567, 570, 573, 576, 578, 582, 
	586, 589, 594, 595
};

static const unsigned char _json_indicies[] = {
	0, 2, 0, 1, 2, 3, 4, 2, 
	1, 5, 6, 7, 8, 9, 10, 11, 
	5, 6, 7, 8, 9, 10, 11, 1, 
	12, 12, 1, 13, 14, 13, 14, 1, 
	15, 15, 1, 16, 16, 1, 17, 1, 
	17, 18, 17, 1, 18, 19, 18, 1, 
	21, 22, 20, 23, 24, 25, 23, 1, 
	26, 27, 4, 26, 1, 27, 3, 27, 
	1, 20, 28, 28, 1, 29, 29, 1, 
	30, 30, 1, 31, 1, 31, 32, 31, 
	1, 32, 33, 32, 1, 34, 35, 34, 
	35, 1, 36, 36, 1, 37, 37, 1, 
	38, 1, 39, 40, 41, 39, 1, 42, 
	42, 1, 43, 43, 1, 44, 44, 1, 
	45, 1, 46, 47, 48, 46, 1, 49, 
	49, 1, 50, 50, 1, 51, 51, 1, 
	52, 52, 1, 53, 53, 1, 54, 1, 
	54, 55, 54, 1, 55, 56, 55, 1, 
	57, 58, 59, 60, 61, 62, 57, 58, 
	59, 60, 61, 62, 1, 63, 63, 1, 
	64, 64, 1, 65, 65, 1, 66, 66, 
	1, 67, 1, 68, 69, 70, 68, 1, 
	71, 71, 1, 72, 72, 1, 73, 73, 
	1, 74, 74, 1, 75, 1, 76, 77, 
	78, 76, 1, 79, 79, 1, 80, 80, 
	1, 81, 81, 1, 82, 82, 1, 83, 
	83, 1, 84, 1, 85, 86, 87, 85, 
	1, 88, 88, 1, 89, 89, 1, 90, 
	90, 1, 91, 91, 1, 92, 1, 93, 
	94, 95, 93, 1, 96, 96, 1, 97, 
	97, 1, 98, 98, 1, 99, 1, 100, 
	101, 102, 100, 1, 103, 103, 1, 104, 
	104, 1, 105, 105, 1, 106, 106, 1, 
	107, 108, 108, 1, 109, 110, 111, 109, 
	1, 112, 1, 113, 114, 115, 113, 1, 
	116, 116, 1, 117, 1, 117, 118, 117, 
	1, 118, 119, 120, 118, 121, 1, 122, 
	1, 123, 124, 125, 123, 122, 1, 126, 
	126, 1, 127, 127, 1, 128, 128, 1, 
	129, 1, 129, 130, 129, 1, 130, 131, 
	132, 130, 133, 1, 134, 1, 135, 136, 
	137, 135, 134, 1, 138, 139, 140, 138, 
	139, 140, 1, 141, 141, 1, 142, 142, 
	1, 143, 143, 1, 144, 144, 1, 145, 
	1, 145, 146, 145, 1, 146, 147, 146, 
	1, 149, 150, 148, 151, 152, 153, 151, 
	1, 148, 154, 154, 1, 155, 155, 1, 
	156, 156, 1, 157, 157, 1, 158, 1, 
	158, 159, 158, 1, 159, 160, 159, 1, 
	161, 162, 163, 164, 161, 162, 163, 164, 
	1, 165, 165, 1, 166, 166, 1, 167, 
	167, 1, 168, 168, 1, 169, 169, 1, 
	170, 1, 171, 172, 173, 171, 1, 174, 
	174, 1, 175, 175, 1, 176, 1, 177, 
	178, 179, 177, 1, 180, 180, 1, 181, 
	181, 1, 182, 182, 1, 183, 1, 184, 
	185, 186, 184, 1, 187, 188, 187, 188, 
	1, 189, 189, 1, 190, 190, 1, 191, 
	1, 192, 193, 194, 192, 1, 195, 195, 
	1, 196, 1, 197, 198, 199, 197, 1, 
	200, 200, 1, 201, 201, 1, 202, 1, 
	202, 203, 202, 1, 203, 204, 203, 1, 
	206, 207, 205, 208, 209, 210, 208, 1, 
	205, 211, 211, 1, 212, 212, 1, 213, 
	1, 213, 214, 213, 1, 214, 215, 214, 
	1, 217, 218, 216, 219, 220, 221, 219, 
	1, 216, 222, 223, 222, 223, 1, 224, 
	224, 1, 225, 225, 1, 226, 226, 1, 
	227, 1, 227, 228, 227, 1, 228, 229, 
	230, 228, 231, 1, 232, 1, 233, 234, 
	235, 233, 232, 1, 236, 236, 1, 237, 
	237, 1, 238, 238, 1, 239, 239, 1, 
	240, 1, 240, 241, 240, 1, 241, 242, 
	241, 1, 244, 245, 243, 246, 247, 248, 
	246, 1, 243, 4, 4, 1, 0
};

static const unsigned char _json_trans_targs[] = {
	1, 0, 2, 3, 171, 4, 32, 79, 
	85, 93, 144, 152, 5, 6, 16, 7, 
	8, 9, 10, 11, 11, 12, 15, 13, 
	14, 171, 13, 14, 17, 18, 19, 20, 
	21, 22, 23, 27, 24, 25, 26, 13, 
	14, 171, 28, 29, 30, 31, 13, 14, 
	171, 33, 34, 35, 36, 37, 38, 39, 
	40, 41, 47, 53, 60, 66, 71, 42, 
	43, 44, 45, 46, 13, 14, 171, 48, 
	49, 50, 51, 52, 13, 14, 171, 54, 
	55, 56, 57, 58, 59, 13, 14, 171, 
	61, 62, 63, 64, 65, 13, 14, 171, 
	67, 68, 69, 70, 13, 14, 171, 72, 
	73, 74, 75, 76, 77, 13, 14, 171, 
	78, 13, 14, 171, 80, 81, 82, 83, 
	83, 84, 84, 13, 14, 171, 86, 87, 
	88, 89, 90, 91, 91, 92, 92, 13, 
	14, 171, 94, 104, 136, 95, 96, 97, 
	98, 99, 100, 101, 101, 102, 103, 13, 
	14, 171, 105, 106, 107, 108, 109, 110, 
	111, 112, 119, 123, 128, 113, 114, 115, 
	116, 117, 118, 13, 14, 171, 120, 121, 
	122, 13, 14, 171, 124, 125, 126, 127, 
	13, 14, 171, 129, 133, 130, 131, 132, 
	13, 14, 171, 134, 135, 13, 14, 171, 
	137, 138, 139, 140, 141, 141, 142, 143, 
	13, 14, 171, 145, 146, 147, 148, 149, 
	149, 150, 151, 13, 14, 171, 153, 161, 
	154, 155, 156, 157, 158, 159, 159, 160, 
	160, 13, 14, 171, 162, 163, 164, 165, 
	166, 167, 168, 168, 169, 170, 13, 14, 
	171
};

static const char _json_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 74, 3, 0, 0, 50, 
	50, 50, 0, 0, 0, 0, 0, 0, 
	0, 35, 0, 0, 0, 0, 0, 56, 
	56, 56, 0, 0, 0, 0, 59, 59, 
	59, 0, 0, 0, 0, 0, 0, 0, 
	31, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 21, 21, 21, 0, 
	0, 0, 0, 0, 15, 15, 15, 0, 
	0, 0, 0, 0, 0, 17, 17, 17, 
	0, 0, 0, 0, 0, 23, 23, 23, 
	0, 0, 0, 0, 27, 27, 27, 0, 
	0, 0, 0, 0, 0, 25, 25, 25, 
	0, 19, 19, 19, 0, 0, 0, 62, 
	86, 90, 1, 33, 33, 33, 0, 0, 
	0, 0, 0, 83, 102, 106, 1, 39, 
	39, 39, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 68, 3, 0, 0, 44, 
	44, 44, 0, 0, 0, 0, 0, 0, 
	29, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 5, 5, 5, 0, 0, 
	0, 7, 7, 7, 0, 0, 0, 0, 
	9, 9, 9, 0, 0, 0, 0, 0, 
	11, 11, 11, 0, 0, 13, 13, 13, 
	0, 0, 0, 0, 65, 3, 0, 0, 
	41, 41, 41, 0, 0, 0, 0, 77, 
	3, 0, 0, 53, 53, 53, 0, 0, 
	0, 0, 0, 0, 0, 80, 94, 98, 
	1, 37, 37, 37, 0, 0, 0, 0, 
	0, 0, 71, 3, 0, 0, 47, 47, 
	47
};

static const int json_start = 1;
static const int json_first_final = 171;
static const int json_error = 0;

static const int json_en_main = 1;


#line 325 "/home/marayl/proj/doobry/src/dbr/rest.rl"

DBR_API void
dbr_rest_init(struct DbrRest* rest)
{
    int cs;
    
#line 390 "/home/marayl/proj/doobry/src/dbr/rest.c"
	{
	cs = json_start;
	}

#line 331 "/home/marayl/proj/doobry/src/dbr/rest.rl"
    rest->cs = cs;
    rest->func = 0;
}

DBR_API int
dbr_rest_json(struct DbrRest* rest, const char* buf, size_t size)
{
	const char* p = buf;
	const char* pe = p + size;

    int cs = rest->cs;
	
#line 408 "/home/marayl/proj/doobry/src/dbr/rest.c"
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
#line 41 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->num.sign = 1;
        rest->num.idigits = 0;
        rest->num.fdigits = 0;
        rest->num.fcount = 0;
    }
	break;
	case 1:
#line 47 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->num.sign = -1;
    }
	break;
	case 2:
#line 50 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->num.idigits = rest->num.idigits * 10 + ((*p) - '0');
    }
	break;
	case 3:
#line 98 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->str.count = 0;
    }
	break;
	case 4:
#line 101 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if (rest->str.count < rest->str.max)
            rest->str.buf[rest->str.count++] = (*p);
        else {
            DBR_DEBUG1F("exceed max string length of %ld", (long)rest->str.max);
            cs = json_error;
            {p++; goto _out; }
        }
    }
	break;
	case 5:
#line 110 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if (rest->str.count < rest->str.max)
            rest->str.buf[rest->str.count] = '\0';
    }
	break;
	case 6:
#line 125 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_METHOD_MASK) | DBR_DELETE;
    }
	break;
	case 7:
#line 128 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_METHOD_MASK) | DBR_GET;
    }
	break;
	case 8:
#line 131 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_METHOD_MASK) | DBR_HEAD;
    }
	break;
	case 9:
#line 134 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_METHOD_MASK) | DBR_POST;
    }
	break;
	case 10:
#line 137 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_METHOD_MASK) | DBR_PUT;
    }
	break;
	case 11:
#line 146 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_INSTR;
    }
	break;
	case 12:
#line 149 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_MARKET;
    }
	break;
	case 13:
#line 152 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_TRADER;
    }
	break;
	case 14:
#line 155 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_ACCNT;
    }
	break;
	case 15:
#line 158 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_ORDER;
    }
	break;
	case 16:
#line 161 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_TRADE;
    }
	break;
	case 17:
#line 164 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_POSN;
    }
	break;
	case 18:
#line 175 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->action = DBR_BUY;
    }
	break;
	case 19:
#line 178 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->action = DBR_SELL;
    }
	break;
	case 20:
#line 187 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if ((rest->func & DBR_METHOD_MASK) != 0) {
            DBR_DEBUG1("method already specified");
            cs = json_error;
            {p++; goto _out; }
        }
    }
	break;
	case 21:
#line 194 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if ((rest->func & DBR_ENTITY_MASK) != 0) {
            DBR_DEBUG1("entity already specified");
            cs = json_error;
            {p++; goto _out; }
        }
    }
	break;
	case 22:
#line 201 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if (rest->func & DBR_ID_PARAM) {
            DBR_DEBUG1("id already specified");
            cs = json_error;
            {p++; goto _out; }
        }
    }
	break;
	case 23:
#line 208 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func |= DBR_ID_PARAM;
        rest->id = inum(rest);
    }
	break;
	case 24:
#line 212 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if (rest->func & DBR_MNEM_PARAM) {
            DBR_DEBUG1("mnem already specified");
            cs = json_error;
            {p++; goto _out; }
        }
        rest->str.buf = rest->mnem;
        rest->str.max = DBR_MNEM_MAX;
    }
	break;
	case 25:
#line 221 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func |= DBR_MNEM_PARAM;
    }
	break;
	case 26:
#line 224 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if (rest->func & DBR_MARKET_PARAM) {
            DBR_DEBUG1("market already specified");
            cs = json_error;
            {p++; goto _out; }
        }
        rest->str.buf = rest->market;
        rest->str.max = DBR_MNEM_MAX;
    }
	break;
	case 27:
#line 233 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func |= DBR_MARKET_PARAM;
    }
	break;
	case 28:
#line 236 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if (rest->func & DBR_TRADER_PARAM) {
            DBR_DEBUG1("trader already specified");
            cs = json_error;
            {p++; goto _out; }
        }
        rest->str.buf = rest->trader;
        rest->str.max = DBR_MNEM_MAX;
    }
	break;
	case 29:
#line 245 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func |= DBR_TRADER_PARAM;
    }
	break;
	case 30:
#line 248 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if (rest->func & DBR_ACCNT_PARAM) {
            DBR_DEBUG1("accnt already specified");
            cs = json_error;
            {p++; goto _out; }
        }
        rest->str.buf = rest->accnt;
        rest->str.max = DBR_MNEM_MAX;
    }
	break;
	case 31:
#line 257 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func |= DBR_ACCNT_PARAM;
    }
	break;
	case 32:
#line 260 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if (rest->func & DBR_REF_PARAM) {
            DBR_DEBUG1("ref already specified");
            cs = json_error;
            {p++; goto _out; }
        }
        rest->str.buf = rest->ref;
        rest->str.max = DBR_REF_MAX;
    }
	break;
	case 33:
#line 269 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func |= DBR_REF_PARAM;
    }
	break;
	case 34:
#line 272 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if (rest->func & DBR_ACTION_PARAM) {
            DBR_DEBUG1("action already specified");
            cs = json_error;
            {p++; goto _out; }
        }
    }
	break;
	case 35:
#line 279 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func |= DBR_ACTION_PARAM;
    }
	break;
	case 36:
#line 282 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if (rest->func & DBR_TICKS_PARAM) {
            DBR_DEBUG1("ticks already specified");
            cs = json_error;
            {p++; goto _out; }
        }
    }
	break;
	case 37:
#line 289 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func |= DBR_TICKS_PARAM;
        rest->ticks = inum(rest);
    }
	break;
	case 38:
#line 293 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if (rest->func & DBR_LOTS_PARAM) {
            DBR_DEBUG1("lots already specified");
            cs = json_error;
            {p++; goto _out; }
        }
    }
	break;
	case 39:
#line 300 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func |= DBR_LOTS_PARAM;
        rest->lots = inum(rest);
    }
	break;
#line 789 "/home/marayl/proj/doobry/src/dbr/rest.c"
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

#line 343 "/home/marayl/proj/doobry/src/dbr/rest.rl"
    rest->cs = cs;

	if (cs == json_error)
		return -1;

	if (cs < json_first_final)
		return 0;

	return 1;
}


#line 502 "/home/marayl/proj/doobry/src/dbr/rest.rl"



#line 819 "/home/marayl/proj/doobry/src/dbr/rest.c"
static const char _rurl_actions[] = {
	0, 1, 1, 1, 3, 1, 7, 1, 
	9, 1, 13, 1, 14, 1, 15, 1, 
	16, 1, 17, 1, 21, 2, 0, 1, 
	2, 2, 3, 2, 5, 13, 2, 5, 
	14, 2, 5, 15, 2, 5, 16, 2, 
	5, 17, 2, 12, 18, 2, 12, 19, 
	3, 4, 6, 13, 3, 4, 6, 14, 
	3, 4, 6, 15, 3, 4, 6, 16, 
	3, 4, 6, 17, 3, 4, 8, 18, 
	3, 4, 10, 19, 3, 4, 10, 20, 
	4, 4, 8, 11, 18, 4, 4, 10, 
	11, 19
};

static const char _rurl_key_offsets[] = {
	0, 0, 1, 8, 9, 10, 11, 12, 
	13, 14, 15, 16, 17, 18, 19, 20, 
	21, 22, 23, 24, 25, 26, 27, 28, 
	29, 30, 31, 32, 33, 34, 35, 36, 
	37, 38, 39, 40, 42, 43, 44, 45, 
	46, 46, 47, 48, 49, 50, 51, 52, 
	53, 54, 55, 56, 58, 61, 62, 63, 
	65, 68, 69, 70
};

static const char _rurl_trans_keys[] = {
	47, 97, 100, 105, 109, 111, 112, 116, 
	99, 99, 110, 116, 101, 112, 116, 104, 
	110, 115, 116, 114, 97, 114, 107, 101, 
	116, 114, 100, 101, 114, 47, 47, 111, 
	115, 110, 47, 47, 114, 97, 100, 101, 
	47, 114, 47, 47, 47, 47, 47, 47, 
	47, 47, 47, 47, 47, 47, 47, 47, 
	48, 57, 47, 48, 57, 47, 47, 48, 
	57, 47, 48, 57, 47, 47, 47, 0
};

static const char _rurl_single_lengths[] = {
	0, 1, 7, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 2, 1, 1, 1, 1, 
	0, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 0, 1, 1, 1, 0, 
	1, 1, 1, 1
};

static const char _rurl_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 1, 1, 0, 0, 1, 
	1, 0, 0, 0
};

static const unsigned char _rurl_index_offsets[] = {
	0, 0, 2, 10, 12, 14, 16, 18, 
	20, 22, 24, 26, 28, 30, 32, 34, 
	36, 38, 40, 42, 44, 46, 48, 50, 
	52, 54, 56, 58, 60, 62, 64, 66, 
	68, 70, 72, 74, 77, 79, 81, 83, 
	85, 86, 88, 90, 92, 94, 96, 98, 
	100, 102, 104, 106, 108, 111, 113, 115, 
	117, 120, 122, 124
};

static const char _rurl_trans_targs[] = {
	2, 0, 3, 7, 11, 15, 20, 26, 
	31, 0, 4, 0, 5, 0, 6, 0, 
	37, 0, 8, 0, 9, 0, 10, 0, 
	41, 0, 12, 0, 13, 0, 14, 0, 
	44, 0, 16, 0, 17, 0, 18, 0, 
	19, 0, 47, 0, 21, 0, 22, 0, 
	23, 0, 24, 0, 25, 0, 0, 50, 
	27, 0, 28, 0, 29, 0, 30, 0, 
	0, 53, 32, 0, 33, 0, 34, 0, 
	35, 0, 36, 57, 0, 0, 54, 38, 
	0, 0, 39, 40, 39, 0, 42, 0, 
	0, 43, 40, 43, 45, 0, 0, 46, 
	40, 46, 48, 0, 0, 49, 40, 49, 
	51, 50, 52, 0, 40, 52, 0, 40, 
	53, 55, 54, 56, 0, 40, 56, 0, 
	58, 0, 0, 59, 40, 59, 0
};

static const char _rurl_trans_actions[] = {
	19, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 5, 0, 0, 24, 
	0, 0, 0, 0, 0, 0, 7, 0, 
	0, 24, 0, 0, 0, 0, 0, 0, 
	0, 0, 7, 0, 0, 0, 24, 36, 
	0, 0, 24, 60, 3, 0, 39, 0, 
	0, 24, 64, 3, 27, 0, 0, 24, 
	48, 3, 30, 0, 0, 24, 52, 3, 
	80, 3, 21, 0, 42, 1, 0, 76, 
	3, 85, 3, 21, 0, 45, 1, 0, 
	33, 0, 0, 24, 56, 3, 0
};

static const char _rurl_eof_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 15, 0, 60, 
	0, 17, 0, 64, 9, 0, 48, 11, 
	0, 52, 68, 0, 42, 76, 72, 0, 
	45, 13, 0, 56
};

static const int rurl_start = 1;
static const int rurl_first_final = 37;
static const int rurl_error = 0;

static const int rurl_en_main = 1;


#line 505 "/home/marayl/proj/doobry/src/dbr/rest.rl"

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

    
#line 965 "/home/marayl/proj/doobry/src/dbr/rest.c"
	{
	cs = rurl_start;
	}

#line 522 "/home/marayl/proj/doobry/src/dbr/rest.rl"
    
#line 972 "/home/marayl/proj/doobry/src/dbr/rest.c"
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
#line 357 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        inum = 0;
    }
	break;
	case 1:
#line 360 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        inum = inum * 10 + ((*p) - '0');
    }
	break;
	case 2:
#line 365 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        str.count = 0;
    }
	break;
	case 3:
#line 368 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if (str.count < str.max)
            str.buf[str.count++] = (*p);
        else {
            DBR_DEBUG1F("exceed max string length of %ld", (long)rest->str.max);
            cs = rurl_error;
            {p++; goto _out; }
        }
    }
	break;
	case 4:
#line 377 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if (str.count < str.max)
            str.buf[str.count] = '\0';
    }
	break;
	case 5:
#line 383 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if (rest->func & DBR_MNEM_PARAM) {
            DBR_DEBUG1("mnem already specified");
            cs = rurl_error;
            {p++; goto _out; }
        }
        str.buf = rest->mnem;
        str.max = DBR_MNEM_MAX;
    }
	break;
	case 6:
#line 392 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func |= DBR_MNEM_PARAM;
    }
	break;
	case 7:
#line 413 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if (rest->func & DBR_TRADER_PARAM) {
            DBR_DEBUG1("trader already specified");
            cs = rurl_error;
            {p++; goto _out; }
        }
        str.buf = rest->trader;
        str.max = DBR_MNEM_MAX;
    }
	break;
	case 8:
#line 422 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func |= DBR_TRADER_PARAM;
    }
	break;
	case 9:
#line 429 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if (rest->func & DBR_ACCNT_PARAM) {
            DBR_DEBUG1("accnt already specified");
            cs = rurl_error;
            {p++; goto _out; }
        }
        str.buf = rest->accnt;
        str.max = DBR_MNEM_MAX;
    }
	break;
	case 10:
#line 438 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func |= DBR_ACCNT_PARAM;
    }
	break;
	case 11:
#line 445 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if (rest->func & DBR_ID_PARAM) {
            DBR_DEBUG1("id already specified");
            cs = rurl_error;
            {p++; goto _out; }
        }
    }
	break;
	case 12:
#line 452 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func |= DBR_ID_PARAM;
        rest->id = inum;
    }
	break;
	case 13:
#line 459 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_INSTR;
    }
	break;
	case 14:
#line 462 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_MARKET;
    }
	break;
	case 15:
#line 465 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_TRADER;
    }
	break;
	case 16:
#line 468 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_ACCNT;
    }
	break;
	case 17:
#line 471 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_DEPTH;
    }
	break;
	case 18:
#line 474 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_ORDER;
    }
	break;
	case 19:
#line 477 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_TRADE;
    }
	break;
	case 20:
#line 480 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_POSN;
    }
	break;
	case 21:
#line 493 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if ((rest->func & DBR_ENTITY_MASK) != 0) {
            DBR_DEBUG1("entity already specified");
            cs = rurl_error;
            {p++; goto _out; }
        }
    }
	break;
#line 1211 "/home/marayl/proj/doobry/src/dbr/rest.c"
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
#line 377 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        if (str.count < str.max)
            str.buf[str.count] = '\0';
    }
	break;
	case 6:
#line 392 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func |= DBR_MNEM_PARAM;
    }
	break;
	case 8:
#line 422 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func |= DBR_TRADER_PARAM;
    }
	break;
	case 10:
#line 438 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func |= DBR_ACCNT_PARAM;
    }
	break;
	case 12:
#line 452 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func |= DBR_ID_PARAM;
        rest->id = inum;
    }
	break;
	case 13:
#line 459 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_INSTR;
    }
	break;
	case 14:
#line 462 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_MARKET;
    }
	break;
	case 15:
#line 465 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_TRADER;
    }
	break;
	case 16:
#line 468 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_ACCNT;
    }
	break;
	case 17:
#line 471 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_DEPTH;
    }
	break;
	case 18:
#line 474 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_ORDER;
    }
	break;
	case 19:
#line 477 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_TRADE;
    }
	break;
	case 20:
#line 480 "/home/marayl/proj/doobry/src/dbr/rest.rl"
	{
        rest->func = (rest->func & ~DBR_ENTITY_MASK) | DBR_POSN;
    }
	break;
#line 1307 "/home/marayl/proj/doobry/src/dbr/rest.c"
		}
	}
	}

	_out: {}
	}

#line 523 "/home/marayl/proj/doobry/src/dbr/rest.rl"

	if (cs == rurl_error)
		return -1;

	if (cs < rurl_first_final)
		return 0;

	return 1;
}


#line 1 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
// -*- C++ -*-
/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#include <swirly/fir/RestRequest.hpp>

#include <swirly/elm/Exception.hpp>

using namespace std;

namespace swirly {

namespace {


#line 263 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"



#line 36 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.cpp"
static const char _json_actions[] = {
	0, 1, 0, 1, 2, 1, 4, 1, 
	5, 1, 7, 1, 8, 1, 10, 1, 
	11, 1, 12, 1, 13, 1, 15, 1, 
	16, 1, 18, 1, 19, 1, 20, 1, 
	21, 1, 22, 1, 23, 1, 24, 1, 
	25, 1, 26, 1, 27, 1, 28, 1, 
	29, 1, 30, 1, 31, 1, 32, 1, 
	34, 2, 0, 1, 2, 0, 2, 2, 
	6, 3, 2, 9, 3, 2, 14, 3, 
	2, 17, 3, 2, 33, 3
};

static const short _json_key_offsets[] = {
	0, 0, 4, 9, 23, 25, 27, 29, 
	31, 32, 36, 41, 43, 48, 53, 57, 
	57, 58, 59, 60, 65, 69, 71, 73, 
	75, 76, 80, 85, 87, 92, 92, 93, 
	94, 95, 100, 102, 104, 105, 109, 114, 
	116, 121, 121, 122, 123, 124, 129, 133, 
	135, 137, 139, 141, 142, 146, 151, 155, 
	157, 159, 161, 163, 164, 169, 171, 173, 
	175, 177, 178, 183, 184, 185, 186, 191, 
	193, 195, 196, 200, 208, 210, 217, 218, 
	219, 220, 225, 229, 231, 233, 235, 237, 
	239, 240, 244, 252, 254, 261, 262, 263, 
	264, 269, 271, 273, 274, 278, 283, 285, 
	290, 290, 291, 292, 293, 298, 300, 302, 
	303, 307, 312, 314, 319, 319, 320, 321, 
	322, 327, 333, 335, 337, 339, 341, 343, 
	345, 347, 348, 352, 360, 362, 369, 370, 
	371, 372, 377, 379, 381, 382, 386, 390, 
	394, 396, 398, 399, 404, 406, 408, 410, 
	411, 416, 418, 420, 422, 423, 427, 435, 
	437, 444, 445, 446, 447, 452, 454, 456, 
	458, 460, 461, 465, 473, 475, 482, 483, 
	484, 485, 490
};

static const char _json_trans_keys[] = {
	32, 123, 9, 13, 32, 34, 125, 9, 
	13, 65, 67, 76, 77, 82, 83, 84, 
	97, 99, 108, 109, 114, 115, 116, 67, 
	99, 67, 99, 78, 110, 84, 116, 34, 
	32, 58, 9, 13, 32, 34, 110, 9, 
	13, 34, 92, 32, 44, 125, 9, 13, 
	32, 44, 125, 9, 13, 32, 34, 9, 
	13, 117, 108, 108, 32, 44, 125, 9, 
	13, 79, 80, 111, 112, 78, 110, 84, 
	116, 82, 114, 34, 32, 58, 9, 13, 
	32, 34, 110, 9, 13, 34, 92, 32, 
	44, 125, 9, 13, 117, 108, 108, 32, 
	44, 125, 9, 13, 84, 116, 89, 121, 
	34, 32, 58, 9, 13, 32, 34, 110, 
	9, 13, 34, 92, 32, 44, 125, 9, 
	13, 117, 108, 108, 32, 44, 125, 9, 
	13, 73, 79, 105, 111, 81, 113, 73, 
	105, 78, 110, 68, 100, 34, 32, 58, 
	9, 13, 32, 34, 110, 9, 13, 77, 
	84, 109, 116, 65, 97, 75, 107, 69, 
	101, 82, 114, 34, 32, 44, 125, 9, 
	13, 65, 97, 75, 107, 69, 101, 82, 
	114, 34, 32, 44, 125, 9, 13, 117, 
	108, 108, 32, 44, 125, 9, 13, 84, 
	116, 83, 115, 34, 32, 58, 9, 13, 
	32, 43, 45, 110, 9, 13, 48, 57, 
	48, 57, 32, 44, 125, 9, 13, 48, 
	57, 117, 108, 108, 32, 44, 125, 9, 
	13, 73, 78, 105, 110, 78, 110, 76, 
	108, 79, 111, 84, 116, 83, 115, 34, 
	32, 58, 9, 13, 32, 43, 45, 110, 
	9, 13, 48, 57, 48, 57, 32, 44, 
	125, 9, 13, 48, 57, 117, 108, 108, 
	32, 44, 125, 9, 13, 69, 101, 77, 
	109, 34, 32, 58, 9, 13, 32, 34, 
	110, 9, 13, 34, 92, 32, 44, 125, 
	9, 13, 117, 108, 108, 32, 44, 125, 
	9, 13, 69, 101, 70, 102, 34, 32, 
	58, 9, 13, 32, 34, 110, 9, 13, 
	34, 92, 32, 44, 125, 9, 13, 117, 
	108, 108, 32, 44, 125, 9, 13, 69, 
	73, 84, 101, 105, 116, 84, 116, 84, 
	116, 76, 108, 68, 100, 65, 97, 84, 
	116, 69, 101, 34, 32, 58, 9, 13, 
	32, 43, 45, 110, 9, 13, 48, 57, 
	48, 57, 32, 44, 125, 9, 13, 48, 
	57, 117, 108, 108, 32, 44, 125, 9, 
	13, 68, 100, 69, 101, 34, 32, 58, 
	9, 13, 32, 34, 9, 13, 66, 83, 
	98, 115, 85, 117, 89, 121, 34, 32, 
	44, 125, 9, 13, 69, 101, 76, 108, 
	76, 108, 34, 32, 44, 125, 9, 13, 
	65, 97, 84, 116, 69, 101, 34, 32, 
	58, 9, 13, 32, 43, 45, 110, 9, 
	13, 48, 57, 48, 57, 32, 44, 125, 
	9, 13, 48, 57, 117, 108, 108, 32, 
	44, 125, 9, 13, 73, 105, 67, 99, 
	75, 107, 83, 115, 34, 32, 58, 9, 
	13, 32, 43, 45, 110, 9, 13, 48, 
	57, 48, 57, 32, 44, 125, 9, 13, 
	48, 57, 117, 108, 108, 32, 44, 125, 
	9, 13, 32, 9, 13, 0
};

static const char _json_single_lengths[] = {
	0, 2, 3, 14, 2, 2, 2, 2, 
	1, 2, 3, 2, 3, 3, 2, 0, 
	1, 1, 1, 3, 4, 2, 2, 2, 
	1, 2, 3, 2, 3, 0, 1, 1, 
	1, 3, 2, 2, 1, 2, 3, 2, 
	3, 0, 1, 1, 1, 3, 4, 2, 
	2, 2, 2, 1, 2, 3, 4, 2, 
	2, 2, 2, 1, 3, 2, 2, 2, 
	2, 1, 3, 1, 1, 1, 3, 2, 
	2, 1, 2, 4, 0, 3, 1, 1, 
	1, 3, 4, 2, 2, 2, 2, 2, 
	1, 2, 4, 0, 3, 1, 1, 1, 
	3, 2, 2, 1, 2, 3, 2, 3, 
	0, 1, 1, 1, 3, 2, 2, 1, 
	2, 3, 2, 3, 0, 1, 1, 1, 
	3, 6, 2, 2, 2, 2, 2, 2, 
	2, 1, 2, 4, 0, 3, 1, 1, 
	1, 3, 2, 2, 1, 2, 2, 4, 
	2, 2, 1, 3, 2, 2, 2, 1, 
	3, 2, 2, 2, 1, 2, 4, 0, 
	3, 1, 1, 1, 3, 2, 2, 2, 
	2, 1, 2, 4, 0, 3, 1, 1, 
	1, 3, 1
};

static const char _json_range_lengths[] = {
	0, 1, 1, 0, 0, 0, 0, 0, 
	0, 1, 1, 0, 1, 1, 1, 0, 
	0, 0, 0, 1, 0, 0, 0, 0, 
	0, 1, 1, 0, 1, 0, 0, 0, 
	0, 1, 0, 0, 0, 1, 1, 0, 
	1, 0, 0, 0, 0, 1, 0, 0, 
	0, 0, 0, 0, 1, 1, 0, 0, 
	0, 0, 0, 0, 1, 0, 0, 0, 
	0, 0, 1, 0, 0, 0, 1, 0, 
	0, 0, 1, 2, 1, 2, 0, 0, 
	0, 1, 0, 0, 0, 0, 0, 0, 
	0, 1, 2, 1, 2, 0, 0, 0, 
	1, 0, 0, 0, 1, 1, 0, 1, 
	0, 0, 0, 0, 1, 0, 0, 0, 
	1, 1, 0, 1, 0, 0, 0, 0, 
	1, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 2, 1, 2, 0, 0, 
	0, 1, 0, 0, 0, 1, 1, 0, 
	0, 0, 0, 1, 0, 0, 0, 0, 
	1, 0, 0, 0, 0, 1, 2, 1, 
	2, 0, 0, 0, 1, 0, 0, 0, 
	0, 0, 1, 2, 1, 2, 0, 0, 
	0, 1, 1
};

static const short _json_index_offsets[] = {
	0, 0, 4, 9, 24, 27, 30, 33, 
	36, 38, 42, 47, 50, 55, 60, 64, 
	65, 67, 69, 71, 76, 81, 84, 87, 
	90, 92, 96, 101, 104, 109, 110, 112, 
	114, 116, 121, 124, 127, 129, 133, 138, 
	141, 146, 147, 149, 151, 153, 158, 163, 
	166, 169, 172, 175, 177, 181, 186, 191, 
	194, 197, 200, 203, 205, 210, 213, 216, 
	219, 222, 224, 229, 231, 233, 235, 240, 
	243, 246, 248, 252, 259, 261, 267, 269, 
	271, 273, 278, 283, 286, 289, 292, 295, 
	298, 300, 304, 311, 313, 319, 321, 323, 
	325, 330, 333, 336, 338, 342, 347, 350, 
	355, 356, 358, 360, 362, 367, 370, 373, 
	375, 379, 384, 387, 392, 393, 395, 397, 
	399, 404, 411, 414, 417, 420, 423, 426, 
	429, 432, 434, 438, 445, 447, 453, 455, 
	457, 459, 464, 467, 470, 472, 476, 480, 
	485, 488, 491, 493, 498, 501, 504, 507, 
	509, 514, 517, 520, 523, 525, 529, 536, 
	538, 544, 546, 548, 550, 555, 558, 561, 
	564, 567, 569, 573, 580, 582, 588, 590, 
	592, 594, 599
};

static const unsigned char _json_trans_targs[] = {
	1, 2, 1, 0, 2, 3, 178, 2, 
	0, 4, 20, 46, 82, 109, 121, 165, 
	4, 20, 46, 82, 109, 121, 165, 0, 
	5, 5, 0, 6, 6, 0, 7, 7, 
	0, 8, 8, 0, 9, 0, 9, 10, 
	9, 0, 10, 11, 16, 10, 0, 12, 
	15, 11, 13, 14, 178, 13, 0, 13, 
	14, 178, 13, 0, 14, 3, 14, 0, 
	11, 17, 0, 18, 0, 19, 0, 13, 
	14, 178, 13, 0, 21, 34, 21, 34, 
	0, 22, 22, 0, 23, 23, 0, 24, 
	24, 0, 25, 0, 25, 26, 25, 0, 
	26, 27, 30, 26, 0, 28, 29, 27, 
	13, 14, 178, 13, 0, 27, 31, 0, 
	32, 0, 33, 0, 13, 14, 178, 13, 
	0, 35, 35, 0, 36, 36, 0, 37, 
	0, 37, 38, 37, 0, 38, 39, 42, 
	38, 0, 40, 41, 39, 13, 14, 178, 
	13, 0, 39, 43, 0, 44, 0, 45, 
	0, 13, 14, 178, 13, 0, 47, 71, 
	47, 71, 0, 48, 48, 0, 49, 49, 
	0, 50, 50, 0, 51, 51, 0, 52, 
	0, 52, 53, 52, 0, 53, 54, 67, 
	53, 0, 55, 61, 55, 61, 0, 56, 
	56, 0, 57, 57, 0, 58, 58, 0, 
	59, 59, 0, 60, 0, 13, 14, 178, 
	13, 0, 62, 62, 0, 63, 63, 0, 
	64, 64, 0, 65, 65, 0, 66, 0, 
	13, 14, 178, 13, 0, 68, 0, 69, 
	0, 70, 0, 13, 14, 178, 13, 0, 
	72, 72, 0, 73, 73, 0, 74, 0, 
	74, 75, 74, 0, 75, 76, 76, 78, 
	75, 77, 0, 77, 0, 13, 14, 178, 
	13, 77, 0, 79, 0, 80, 0, 81, 
	0, 13, 14, 178, 13, 0, 83, 97, 
	83, 97, 0, 84, 84, 0, 85, 85, 
	0, 86, 86, 0, 87, 87, 0, 88, 
	88, 0, 89, 0, 89, 90, 89, 0, 
	90, 91, 91, 93, 90, 92, 0, 92, 
	0, 13, 14, 178, 13, 92, 0, 94, 
	0, 95, 0, 96, 0, 13, 14, 178, 
	13, 0, 98, 98, 0, 99, 99, 0, 
	100, 0, 100, 101, 100, 0, 101, 102, 
	105, 101, 0, 103, 104, 102, 13, 14, 
	178, 13, 0, 102, 106, 0, 107, 0, 
	108, 0, 13, 14, 178, 13, 0, 110, 
	110, 0, 111, 111, 0, 112, 0, 112, 
	113, 112, 0, 113, 114, 117, 113, 0, 
	115, 116, 114, 13, 14, 178, 13, 0, 
	114, 118, 0, 119, 0, 120, 0, 13, 
	14, 178, 13, 0, 122, 138, 153, 122, 
	138, 153, 0, 123, 123, 0, 124, 124, 
	0, 125, 125, 0, 126, 126, 0, 127, 
	127, 0, 128, 128, 0, 129, 129, 0, 
	130, 0, 130, 131, 130, 0, 131, 132, 
	132, 134, 131, 133, 0, 133, 0, 13, 
	14, 178, 13, 133, 0, 135, 0, 136, 
	0, 137, 0, 13, 14, 178, 13, 0, 
	139, 139, 0, 140, 140, 0, 141, 0, 
	141, 142, 141, 0, 142, 143, 142, 0, 
	144, 148, 144, 148, 0, 145, 145, 0, 
	146, 146, 0, 147, 0, 13, 14, 178, 
	13, 0, 149, 149, 0, 150, 150, 0, 
	151, 151, 0, 152, 0, 13, 14, 178, 
	13, 0, 154, 154, 0, 155, 155, 0, 
	156, 156, 0, 157, 0, 157, 158, 157, 
	0, 158, 159, 159, 161, 158, 160, 0, 
	160, 0, 13, 14, 178, 13, 160, 0, 
	162, 0, 163, 0, 164, 0, 13, 14, 
	178, 13, 0, 166, 166, 0, 167, 167, 
	0, 168, 168, 0, 169, 169, 0, 170, 
	0, 170, 171, 170, 0, 171, 172, 172, 
	174, 171, 173, 0, 173, 0, 13, 14, 
	178, 13, 173, 0, 175, 0, 176, 0, 
	177, 0, 13, 14, 178, 13, 0, 178, 
	178, 0, 0
};

static const char _json_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 69, 0, 0, 0, 0, 
	0, 5, 21, 21, 21, 21, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	5, 0, 0, 0, 0, 0, 0, 19, 
	19, 19, 19, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 66, 0, 0, 0, 0, 0, 5, 
	13, 13, 13, 13, 0, 5, 0, 0, 
	0, 0, 0, 0, 11, 11, 11, 11, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 75, 0, 
	0, 0, 0, 0, 5, 55, 55, 55, 
	55, 0, 5, 0, 0, 0, 0, 0, 
	0, 53, 53, 53, 53, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 49, 49, 49, 
	49, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	51, 51, 51, 51, 0, 0, 0, 0, 
	0, 0, 0, 47, 47, 47, 47, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 1, 57, 0, 
	0, 60, 0, 3, 0, 41, 41, 41, 
	41, 3, 0, 0, 0, 0, 0, 0, 
	0, 39, 39, 39, 39, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 57, 0, 0, 60, 0, 3, 
	0, 45, 45, 45, 45, 3, 0, 0, 
	0, 0, 0, 0, 0, 43, 43, 43, 
	43, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 63, 
	0, 0, 0, 0, 0, 5, 9, 9, 
	9, 9, 0, 5, 0, 0, 0, 0, 
	0, 0, 7, 7, 7, 7, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 72, 0, 0, 0, 
	0, 0, 5, 25, 25, 25, 25, 0, 
	5, 0, 0, 0, 0, 0, 0, 23, 
	23, 23, 23, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 1, 
	57, 0, 0, 60, 0, 3, 0, 17, 
	17, 17, 17, 3, 0, 0, 0, 0, 
	0, 0, 0, 15, 15, 15, 15, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 35, 35, 35, 
	35, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 37, 37, 37, 
	37, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 57, 0, 0, 60, 0, 
	3, 0, 29, 29, 29, 29, 3, 0, 
	0, 0, 0, 0, 0, 0, 27, 27, 
	27, 27, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 1, 57, 
	0, 0, 60, 0, 3, 0, 33, 33, 
	33, 33, 3, 0, 0, 0, 0, 0, 
	0, 0, 31, 31, 31, 31, 0, 0, 
	0, 0, 0
};

static const int json_start = 1;
static const int json_first_final = 178;
static const int json_error = 0;

static const int json_en_main = 1;


#line 266 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"

} // anonymous

RestRequest::~RestRequest() noexcept = default;

void RestRequest::reset(bool clear) noexcept
{
  decltype(cs_) cs;
  
#line 394 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.cpp"
	{
	cs = json_start;
	}

#line 275 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
  cs_ = cs;

  if (!clear) {
    return;
  }
  fields_ = 0;

  mnem_.len = 0;
  contr_.len = 0;
  settlDate_ = 0_ymd;
  accnt_.len = 0;
  ref_.len = 0;
  state_ = 0;
  side_ = static_cast<swirly::Side>(0);
  lots_ = 0_lts;
  ticks_ = 0_tks;
  minLots_ = 0_lts;
  liqInd_ = swirly::LiqInd::None;
  cpty_.len = 0;
}

bool RestRequest::parse(string_view buf)
{
  const char* p{buf.data()};
  const char* pe{p + buf.size()};
  const char* msg{"parse error"};

  auto cs = cs_;
  
#line 429 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.cpp"
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
#line 31 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    num_.sign = 1;
    num_.digits = 0;
  }
	break;
	case 1:
#line 35 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    num_.sign = -1;
  }
	break;
	case 2:
#line 38 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    num_.digits = num_.digits * 10 + ((*p) - '0');
  }
	break;
	case 3:
#line 57 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    *str_.len = 0;
  }
	break;
	case 4:
#line 60 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    auto& len = *str_.len;
    if (len < str_.max) {
      str_.buf[len++] = (*p);
    } else {
      cs = json_error; msg = "max length exceeded";
      {p++; goto _out; }
    }
  }
	break;
	case 5:
#line 80 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Mnem;
    mnem_.len = 0;
  }
	break;
	case 6:
#line 84 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    str_.len = &mnem_.len;
    str_.buf = mnem_.buf;
    str_.max = MaxMnem;
  }
	break;
	case 7:
#line 89 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Mnem;
  }
	break;
	case 8:
#line 95 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Contr;
    contr_.len = 0;
  }
	break;
	case 9:
#line 99 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    str_.len = &contr_.len;
    str_.buf = contr_.buf;
    str_.max = MaxMnem;
  }
	break;
	case 10:
#line 104 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Contr;
  }
	break;
	case 11:
#line 110 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~SettlDate;
    settlDate_ = 0_ymd;
  }
	break;
	case 12:
#line 114 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= SettlDate;
    settlDate_ = IsoDate{num()};
  }
	break;
	case 13:
#line 121 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Accnt;
    accnt_.len = 0;
  }
	break;
	case 14:
#line 125 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    str_.len = &accnt_.len;
    str_.buf = accnt_.buf;
    str_.max = MaxMnem;
  }
	break;
	case 15:
#line 130 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Accnt;
  }
	break;
	case 16:
#line 136 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Ref;
    ref_.len = 0;
  }
	break;
	case 17:
#line 140 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    str_.len = &ref_.len;
    str_.buf = ref_.buf;
    str_.max = MaxRef;
  }
	break;
	case 18:
#line 145 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Ref;
  }
	break;
	case 19:
#line 151 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~State;
    state_ = 0;
  }
	break;
	case 20:
#line 155 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    if (num_.sign >= 0) {
      fields_ |= State;
      state_ = num();
    } else {
      cs = json_error; msg = "negative state field";
    }
  }
	break;
	case 21:
#line 166 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Ticks;
    ticks_ = 0_tks;
  }
	break;
	case 22:
#line 170 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Ticks;
    ticks_ = swirly::Ticks{num()};
  }
	break;
	case 23:
#line 177 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Side;
    side_ = swirly::Side::Buy;
  }
	break;
	case 24:
#line 181 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Side;
    side_ = swirly::Side::Sell;
  }
	break;
	case 25:
#line 188 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Lots;
    lots_ = 0_lts;
  }
	break;
	case 26:
#line 192 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Lots;
    lots_ = swirly::Lots{num()};
  }
	break;
	case 27:
#line 199 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~MinLots;
    minLots_ = 0_lts;
  }
	break;
	case 28:
#line 203 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= MinLots;
    minLots_ = swirly::Lots{num()};
  }
	break;
	case 29:
#line 210 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~LiqInd;
    liqInd_ = swirly::LiqInd::None;
  }
	break;
	case 30:
#line 214 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= LiqInd;
    liqInd_ = swirly::LiqInd::Maker;
  }
	break;
	case 31:
#line 218 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= LiqInd;
    liqInd_ = swirly::LiqInd::Taker;
  }
	break;
	case 32:
#line 226 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Cpty;
    cpty_.len = 0;
  }
	break;
	case 33:
#line 230 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    str_.len = &cpty_.len;
    str_.buf = cpty_.buf;
    str_.max = MaxMnem;
  }
	break;
	case 34:
#line 235 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Cpty;
  }
	break;
#line 753 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.cpp"
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

#line 304 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
  cs_ = cs;

  if (cs == json_error) {
    throw BadRequestException{msg};
  }
  if (cs < json_first_final) {
    return false;
  }
  return true;
}

} // swirly

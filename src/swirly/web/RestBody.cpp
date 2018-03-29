
#line 1 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
// -*- C++ -*-
/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
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
#include <swirly/web/RestBody.hpp>

#include <swirly/fin/Exception.hpp>

namespace swirly {
inline namespace web {
using namespace std;
namespace {


#line 262 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"



#line 35 "/home/marayl/repo/swirly/src/swirly/web/RestBody.cpp"
static const char _json_actions[] = {
	0, 1, 0, 1, 2, 1, 4, 1, 
	5, 1, 7, 1, 8, 1, 10, 1, 
	11, 1, 13, 1, 14, 1, 15, 1, 
	16, 1, 18, 1, 19, 1, 20, 1, 
	21, 1, 22, 1, 23, 1, 24, 1, 
	25, 1, 26, 1, 27, 1, 28, 1, 
	29, 1, 30, 1, 31, 1, 32, 1, 
	34, 2, 0, 1, 2, 0, 2, 2, 
	6, 3, 2, 9, 3, 2, 12, 3, 
	2, 17, 3, 2, 33, 3
};

static const short _json_key_offsets[] = {
	0, 0, 4, 9, 25, 27, 29, 31, 
	33, 34, 38, 43, 45, 50, 55, 59, 
	59, 60, 61, 62, 67, 69, 71, 73, 
	74, 78, 83, 85, 90, 90, 91, 92, 
	93, 98, 100, 102, 104, 106, 107, 111, 
	116, 118, 123, 123, 124, 125, 126, 131, 
	135, 137, 139, 141, 143, 144, 148, 153, 
	157, 159, 161, 163, 165, 166, 171, 173, 
	175, 177, 179, 180, 185, 186, 187, 188, 
	193, 195, 197, 198, 202, 210, 212, 219, 
	220, 221, 222, 227, 229, 231, 233, 235, 
	237, 239, 240, 244, 252, 254, 261, 262, 
	263, 264, 269, 271, 273, 274, 278, 283, 
	285, 290, 290, 291, 292, 293, 298, 306, 
	308, 310, 312, 314, 316, 318, 320, 321, 
	325, 333, 335, 342, 343, 344, 345, 350, 
	352, 354, 355, 359, 363, 367, 369, 371, 
	372, 377, 379, 381, 383, 384, 389, 391, 
	393, 395, 396, 400, 408, 410, 417, 418, 
	419, 420, 425, 427, 429, 431, 433, 434, 
	438, 443, 445, 450, 450, 451, 452, 453, 
	458, 460, 462, 464, 466, 467, 471, 479, 
	481, 488, 489, 490, 491, 496
};

static const char _json_trans_keys[] = {
	32, 123, 9, 13, 32, 34, 125, 9, 
	13, 65, 67, 73, 76, 77, 82, 83, 
	84, 97, 99, 105, 108, 109, 114, 115, 
	116, 67, 99, 67, 99, 78, 110, 84, 
	116, 34, 32, 58, 9, 13, 32, 34, 
	110, 9, 13, 34, 92, 32, 44, 125, 
	9, 13, 32, 44, 125, 9, 13, 32, 
	34, 9, 13, 117, 108, 108, 32, 44, 
	125, 9, 13, 80, 112, 84, 116, 89, 
	121, 34, 32, 58, 9, 13, 32, 34, 
	110, 9, 13, 34, 92, 32, 44, 125, 
	9, 13, 117, 108, 108, 32, 44, 125, 
	9, 13, 78, 110, 83, 115, 84, 116, 
	82, 114, 34, 32, 58, 9, 13, 32, 
	34, 110, 9, 13, 34, 92, 32, 44, 
	125, 9, 13, 117, 108, 108, 32, 44, 
	125, 9, 13, 73, 79, 105, 111, 81, 
	113, 73, 105, 78, 110, 68, 100, 34, 
	32, 58, 9, 13, 32, 34, 110, 9, 
	13, 77, 84, 109, 116, 65, 97, 75, 
	107, 69, 101, 82, 114, 34, 32, 44, 
	125, 9, 13, 65, 97, 75, 107, 69, 
	101, 82, 114, 34, 32, 44, 125, 9, 
	13, 117, 108, 108, 32, 44, 125, 9, 
	13, 84, 116, 83, 115, 34, 32, 58, 
	9, 13, 32, 43, 45, 110, 9, 13, 
	48, 57, 48, 57, 32, 44, 125, 9, 
	13, 48, 57, 117, 108, 108, 32, 44, 
	125, 9, 13, 73, 105, 78, 110, 76, 
	108, 79, 111, 84, 116, 83, 115, 34, 
	32, 58, 9, 13, 32, 43, 45, 110, 
	9, 13, 48, 57, 48, 57, 32, 44, 
	125, 9, 13, 48, 57, 117, 108, 108, 
	32, 44, 125, 9, 13, 69, 101, 70, 
	102, 34, 32, 58, 9, 13, 32, 34, 
	110, 9, 13, 34, 92, 32, 44, 125, 
	9, 13, 117, 108, 108, 32, 44, 125, 
	9, 13, 69, 73, 84, 89, 101, 105, 
	116, 121, 84, 116, 84, 116, 76, 108, 
	68, 100, 65, 97, 84, 116, 69, 101, 
	34, 32, 58, 9, 13, 32, 43, 45, 
	110, 9, 13, 48, 57, 48, 57, 32, 
	44, 125, 9, 13, 48, 57, 117, 108, 
	108, 32, 44, 125, 9, 13, 68, 100, 
	69, 101, 34, 32, 58, 9, 13, 32, 
	34, 9, 13, 66, 83, 98, 115, 85, 
	117, 89, 121, 34, 32, 44, 125, 9, 
	13, 69, 101, 76, 108, 76, 108, 34, 
	32, 44, 125, 9, 13, 65, 97, 84, 
	116, 69, 101, 34, 32, 58, 9, 13, 
	32, 43, 45, 110, 9, 13, 48, 57, 
	48, 57, 32, 44, 125, 9, 13, 48, 
	57, 117, 108, 108, 32, 44, 125, 9, 
	13, 77, 109, 66, 98, 79, 111, 76, 
	108, 34, 32, 58, 9, 13, 32, 34, 
	110, 9, 13, 34, 92, 32, 44, 125, 
	9, 13, 117, 108, 108, 32, 44, 125, 
	9, 13, 73, 105, 67, 99, 75, 107, 
	83, 115, 34, 32, 58, 9, 13, 32, 
	43, 45, 110, 9, 13, 48, 57, 48, 
	57, 32, 44, 125, 9, 13, 48, 57, 
	117, 108, 108, 32, 44, 125, 9, 13, 
	32, 9, 13, 0
};

static const char _json_single_lengths[] = {
	0, 2, 3, 16, 2, 2, 2, 2, 
	1, 2, 3, 2, 3, 3, 2, 0, 
	1, 1, 1, 3, 2, 2, 2, 1, 
	2, 3, 2, 3, 0, 1, 1, 1, 
	3, 2, 2, 2, 2, 1, 2, 3, 
	2, 3, 0, 1, 1, 1, 3, 4, 
	2, 2, 2, 2, 1, 2, 3, 4, 
	2, 2, 2, 2, 1, 3, 2, 2, 
	2, 2, 1, 3, 1, 1, 1, 3, 
	2, 2, 1, 2, 4, 0, 3, 1, 
	1, 1, 3, 2, 2, 2, 2, 2, 
	2, 1, 2, 4, 0, 3, 1, 1, 
	1, 3, 2, 2, 1, 2, 3, 2, 
	3, 0, 1, 1, 1, 3, 8, 2, 
	2, 2, 2, 2, 2, 2, 1, 2, 
	4, 0, 3, 1, 1, 1, 3, 2, 
	2, 1, 2, 2, 4, 2, 2, 1, 
	3, 2, 2, 2, 1, 3, 2, 2, 
	2, 1, 2, 4, 0, 3, 1, 1, 
	1, 3, 2, 2, 2, 2, 1, 2, 
	3, 2, 3, 0, 1, 1, 1, 3, 
	2, 2, 2, 2, 1, 2, 4, 0, 
	3, 1, 1, 1, 3, 1
};

static const char _json_range_lengths[] = {
	0, 1, 1, 0, 0, 0, 0, 0, 
	0, 1, 1, 0, 1, 1, 1, 0, 
	0, 0, 0, 1, 0, 0, 0, 0, 
	1, 1, 0, 1, 0, 0, 0, 0, 
	1, 0, 0, 0, 0, 0, 1, 1, 
	0, 1, 0, 0, 0, 0, 1, 0, 
	0, 0, 0, 0, 0, 1, 1, 0, 
	0, 0, 0, 0, 0, 1, 0, 0, 
	0, 0, 0, 1, 0, 0, 0, 1, 
	0, 0, 0, 1, 2, 1, 2, 0, 
	0, 0, 1, 0, 0, 0, 0, 0, 
	0, 0, 1, 2, 1, 2, 0, 0, 
	0, 1, 0, 0, 0, 1, 1, 0, 
	1, 0, 0, 0, 0, 1, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 1, 
	2, 1, 2, 0, 0, 0, 1, 0, 
	0, 0, 1, 1, 0, 0, 0, 0, 
	1, 0, 0, 0, 0, 1, 0, 0, 
	0, 0, 1, 2, 1, 2, 0, 0, 
	0, 1, 0, 0, 0, 0, 0, 1, 
	1, 0, 1, 0, 0, 0, 0, 1, 
	0, 0, 0, 0, 0, 1, 2, 1, 
	2, 0, 0, 0, 1, 1
};

static const short _json_index_offsets[] = {
	0, 0, 4, 9, 26, 29, 32, 35, 
	38, 40, 44, 49, 52, 57, 62, 66, 
	67, 69, 71, 73, 78, 81, 84, 87, 
	89, 93, 98, 101, 106, 107, 109, 111, 
	113, 118, 121, 124, 127, 130, 132, 136, 
	141, 144, 149, 150, 152, 154, 156, 161, 
	166, 169, 172, 175, 178, 180, 184, 189, 
	194, 197, 200, 203, 206, 208, 213, 216, 
	219, 222, 225, 227, 232, 234, 236, 238, 
	243, 246, 249, 251, 255, 262, 264, 270, 
	272, 274, 276, 281, 284, 287, 290, 293, 
	296, 299, 301, 305, 312, 314, 320, 322, 
	324, 326, 331, 334, 337, 339, 343, 348, 
	351, 356, 357, 359, 361, 363, 368, 377, 
	380, 383, 386, 389, 392, 395, 398, 400, 
	404, 411, 413, 419, 421, 423, 425, 430, 
	433, 436, 438, 442, 446, 451, 454, 457, 
	459, 464, 467, 470, 473, 475, 480, 483, 
	486, 489, 491, 495, 502, 504, 510, 512, 
	514, 516, 521, 524, 527, 530, 533, 535, 
	539, 544, 547, 552, 553, 555, 557, 559, 
	564, 567, 570, 573, 576, 578, 582, 589, 
	591, 597, 599, 601, 603, 608
};

static const unsigned char _json_trans_targs[] = {
	1, 2, 1, 0, 2, 3, 181, 2, 
	0, 4, 20, 33, 47, 83, 98, 110, 
	168, 4, 20, 33, 47, 83, 98, 110, 
	168, 0, 5, 5, 0, 6, 6, 0, 
	7, 7, 0, 8, 8, 0, 9, 0, 
	9, 10, 9, 0, 10, 11, 16, 10, 
	0, 12, 15, 11, 13, 14, 181, 13, 
	0, 13, 14, 181, 13, 0, 14, 3, 
	14, 0, 11, 17, 0, 18, 0, 19, 
	0, 13, 14, 181, 13, 0, 21, 21, 
	0, 22, 22, 0, 23, 23, 0, 24, 
	0, 24, 25, 24, 0, 25, 26, 29, 
	25, 0, 27, 28, 26, 13, 14, 181, 
	13, 0, 26, 30, 0, 31, 0, 32, 
	0, 13, 14, 181, 13, 0, 34, 34, 
	0, 35, 35, 0, 36, 36, 0, 37, 
	37, 0, 38, 0, 38, 39, 38, 0, 
	39, 40, 43, 39, 0, 41, 42, 40, 
	13, 14, 181, 13, 0, 40, 44, 0, 
	45, 0, 46, 0, 13, 14, 181, 13, 
	0, 48, 72, 48, 72, 0, 49, 49, 
	0, 50, 50, 0, 51, 51, 0, 52, 
	52, 0, 53, 0, 53, 54, 53, 0, 
	54, 55, 68, 54, 0, 56, 62, 56, 
	62, 0, 57, 57, 0, 58, 58, 0, 
	59, 59, 0, 60, 60, 0, 61, 0, 
	13, 14, 181, 13, 0, 63, 63, 0, 
	64, 64, 0, 65, 65, 0, 66, 66, 
	0, 67, 0, 13, 14, 181, 13, 0, 
	69, 0, 70, 0, 71, 0, 13, 14, 
	181, 13, 0, 73, 73, 0, 74, 74, 
	0, 75, 0, 75, 76, 75, 0, 76, 
	77, 77, 79, 76, 78, 0, 78, 0, 
	13, 14, 181, 13, 78, 0, 80, 0, 
	81, 0, 82, 0, 13, 14, 181, 13, 
	0, 84, 84, 0, 85, 85, 0, 86, 
	86, 0, 87, 87, 0, 88, 88, 0, 
	89, 89, 0, 90, 0, 90, 91, 90, 
	0, 91, 92, 92, 94, 91, 93, 0, 
	93, 0, 13, 14, 181, 13, 93, 0, 
	95, 0, 96, 0, 97, 0, 13, 14, 
	181, 13, 0, 99, 99, 0, 100, 100, 
	0, 101, 0, 101, 102, 101, 0, 102, 
	103, 106, 102, 0, 104, 105, 103, 13, 
	14, 181, 13, 0, 103, 107, 0, 108, 
	0, 109, 0, 13, 14, 181, 13, 0, 
	111, 127, 142, 154, 111, 127, 142, 154, 
	0, 112, 112, 0, 113, 113, 0, 114, 
	114, 0, 115, 115, 0, 116, 116, 0, 
	117, 117, 0, 118, 118, 0, 119, 0, 
	119, 120, 119, 0, 120, 121, 121, 123, 
	120, 122, 0, 122, 0, 13, 14, 181, 
	13, 122, 0, 124, 0, 125, 0, 126, 
	0, 13, 14, 181, 13, 0, 128, 128, 
	0, 129, 129, 0, 130, 0, 130, 131, 
	130, 0, 131, 132, 131, 0, 133, 137, 
	133, 137, 0, 134, 134, 0, 135, 135, 
	0, 136, 0, 13, 14, 181, 13, 0, 
	138, 138, 0, 139, 139, 0, 140, 140, 
	0, 141, 0, 13, 14, 181, 13, 0, 
	143, 143, 0, 144, 144, 0, 145, 145, 
	0, 146, 0, 146, 147, 146, 0, 147, 
	148, 148, 150, 147, 149, 0, 149, 0, 
	13, 14, 181, 13, 149, 0, 151, 0, 
	152, 0, 153, 0, 13, 14, 181, 13, 
	0, 155, 155, 0, 156, 156, 0, 157, 
	157, 0, 158, 158, 0, 159, 0, 159, 
	160, 159, 0, 160, 161, 164, 160, 0, 
	162, 163, 161, 13, 14, 181, 13, 0, 
	161, 165, 0, 166, 0, 167, 0, 13, 
	14, 181, 13, 0, 169, 169, 0, 170, 
	170, 0, 171, 171, 0, 172, 172, 0, 
	173, 0, 173, 174, 173, 0, 174, 175, 
	175, 177, 174, 176, 0, 176, 0, 13, 
	14, 181, 13, 176, 0, 178, 0, 179, 
	0, 180, 0, 13, 14, 181, 13, 0, 
	181, 181, 0, 0
};

static const char _json_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 66, 0, 0, 
	0, 0, 0, 5, 13, 13, 13, 13, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 5, 0, 0, 0, 0, 0, 
	0, 11, 11, 11, 11, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 75, 0, 
	0, 0, 0, 0, 5, 55, 55, 55, 
	55, 0, 5, 0, 0, 0, 0, 0, 
	0, 53, 53, 53, 53, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 69, 0, 0, 0, 0, 0, 5, 
	17, 17, 17, 17, 0, 5, 0, 0, 
	0, 0, 0, 0, 15, 15, 15, 15, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	49, 49, 49, 49, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 51, 51, 51, 51, 0, 
	0, 0, 0, 0, 0, 0, 47, 47, 
	47, 47, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	1, 57, 0, 0, 60, 0, 3, 0, 
	41, 41, 41, 41, 3, 0, 0, 0, 
	0, 0, 0, 0, 39, 39, 39, 39, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 57, 0, 0, 60, 0, 
	3, 0, 45, 45, 45, 45, 3, 0, 
	0, 0, 0, 0, 0, 0, 43, 43, 
	43, 43, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	72, 0, 0, 0, 0, 0, 5, 25, 
	25, 25, 25, 0, 5, 0, 0, 0, 
	0, 0, 0, 23, 23, 23, 23, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 1, 57, 0, 
	0, 60, 0, 3, 0, 21, 21, 21, 
	21, 3, 0, 0, 0, 0, 0, 0, 
	0, 19, 19, 19, 19, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 35, 35, 35, 35, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 37, 37, 37, 37, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	1, 57, 0, 0, 60, 0, 3, 0, 
	29, 29, 29, 29, 3, 0, 0, 0, 
	0, 0, 0, 0, 27, 27, 27, 27, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 63, 0, 0, 0, 
	0, 0, 5, 9, 9, 9, 9, 0, 
	5, 0, 0, 0, 0, 0, 0, 7, 
	7, 7, 7, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 1, 
	57, 0, 0, 60, 0, 3, 0, 33, 
	33, 33, 33, 3, 0, 0, 0, 0, 
	0, 0, 0, 31, 31, 31, 31, 0, 
	0, 0, 0, 0
};

static const int json_start = 1;
static const int json_first_final = 181;
static const int json_error = 0;

static const int json_en_main = 1;


#line 265 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"

} // anonymous

RestBody::~RestBody() = default;

void RestBody::reset(bool clear) noexcept
{
  decltype(cs_) cs;
  
#line 396 "/home/marayl/repo/swirly/src/swirly/web/RestBody.cpp"
	{
	cs = json_start;
	}

#line 274 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
  cs_ = cs;

  if (!clear) {
    return;
  }
  fields_ = 0;

  symbol_.len = 0;
  accnt_.len = 0;
  instr_.len = 0;
  settlDate_ = 0_ymd;
  ref_.len = 0;
  state_ = 0;
  side_ = static_cast<swirly::Side>(0);
  lots_ = 0_lts;
  ticks_ = 0_tks;
  minLots_ = 0_lts;
  liqInd_ = swirly::LiqInd::None;
  cpty_.len = 0;
}

bool RestBody::parse(string_view buf)
{
  const char* p{buf.data()};
  const char* pe{p + buf.size()};
  const char* msg{"parse error"};

  auto cs = cs_;
  
#line 431 "/home/marayl/repo/swirly/src/swirly/web/RestBody.cpp"
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
#line 30 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    num_.sign = 1;
    num_.digits = 0;
  }
	break;
	case 1:
#line 34 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    num_.sign = -1;
  }
	break;
	case 2:
#line 37 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    num_.digits = num_.digits * 10 + ((*p) - '0');
  }
	break;
	case 3:
#line 56 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    *str_.len = 0;
  }
	break;
	case 4:
#line 59 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
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
#line 79 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ &= ~Symbol;
    symbol_.len = 0;
  }
	break;
	case 6:
#line 83 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    str_.len = &symbol_.len;
    str_.buf = symbol_.buf;
    str_.max = MaxSymbol;
  }
	break;
	case 7:
#line 88 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ |= Symbol;
  }
	break;
	case 8:
#line 94 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ &= ~Accnt;
    accnt_.len = 0;
  }
	break;
	case 9:
#line 98 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    str_.len = &accnt_.len;
    str_.buf = accnt_.buf;
    str_.max = MaxSymbol;
  }
	break;
	case 10:
#line 103 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ |= Accnt;
  }
	break;
	case 11:
#line 109 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ &= ~Instr;
    instr_.len = 0;
  }
	break;
	case 12:
#line 113 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    str_.len = &instr_.len;
    str_.buf = instr_.buf;
    str_.max = MaxSymbol;
  }
	break;
	case 13:
#line 118 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ |= Instr;
  }
	break;
	case 14:
#line 124 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ &= ~SettlDate;
    settlDate_ = 0_ymd;
  }
	break;
	case 15:
#line 128 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ |= SettlDate;
    settlDate_ = IsoDate{num()};
  }
	break;
	case 16:
#line 135 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ &= ~Ref;
    ref_.len = 0;
  }
	break;
	case 17:
#line 139 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    str_.len = &ref_.len;
    str_.buf = ref_.buf;
    str_.max = MaxRef;
  }
	break;
	case 18:
#line 144 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ |= Ref;
  }
	break;
	case 19:
#line 150 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ &= ~State;
    state_ = 0;
  }
	break;
	case 20:
#line 154 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
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
#line 165 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ &= ~Ticks;
    ticks_ = 0_tks;
  }
	break;
	case 22:
#line 169 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ |= Ticks;
    ticks_ = swirly::Ticks{num()};
  }
	break;
	case 23:
#line 176 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ |= Side;
    side_ = swirly::Side::Buy;
  }
	break;
	case 24:
#line 180 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ |= Side;
    side_ = swirly::Side::Sell;
  }
	break;
	case 25:
#line 187 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ &= ~Lots;
    lots_ = 0_lts;
  }
	break;
	case 26:
#line 191 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ |= Lots;
    lots_ = swirly::Lots{num()};
  }
	break;
	case 27:
#line 198 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ &= ~MinLots;
    minLots_ = 0_lts;
  }
	break;
	case 28:
#line 202 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ |= MinLots;
    minLots_ = swirly::Lots{num()};
  }
	break;
	case 29:
#line 209 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ &= ~LiqInd;
    liqInd_ = swirly::LiqInd::None;
  }
	break;
	case 30:
#line 213 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ |= LiqInd;
    liqInd_ = swirly::LiqInd::Maker;
  }
	break;
	case 31:
#line 217 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ |= LiqInd;
    liqInd_ = swirly::LiqInd::Taker;
  }
	break;
	case 32:
#line 225 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ &= ~Cpty;
    cpty_.len = 0;
  }
	break;
	case 33:
#line 229 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    str_.len = &cpty_.len;
    str_.buf = cpty_.buf;
    str_.max = MaxSymbol;
  }
	break;
	case 34:
#line 234 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
	{
    fields_ |= Cpty;
  }
	break;
#line 755 "/home/marayl/repo/swirly/src/swirly/web/RestBody.cpp"
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

#line 303 "/home/marayl/repo/swirly/src/swirly/web/RestBody.rl"
  cs_ = cs;

  if (cs == json_error) {
    throw BadRequestException{msg};
  }
  if (cs < json_first_final) {
    return false;
  }
  return true;
}

} // namespace web
} // namespace swirly

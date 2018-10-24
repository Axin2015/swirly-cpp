
#line 1 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
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
#include "RestBody.hxx"

#include <swirly/app/Exception.hpp>

namespace swirly {
using namespace std;
namespace {


#line 257 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"



#line 34 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.cpp"
static const char _json_actions[] = {
	0, 1, 0, 1, 2, 1, 4, 1, 
	5, 1, 6, 1, 7, 1, 9, 1, 
	10, 1, 12, 1, 13, 1, 14, 1, 
	15, 1, 17, 1, 18, 1, 19, 1, 
	20, 1, 21, 1, 22, 1, 23, 1, 
	24, 1, 25, 1, 26, 1, 27, 1, 
	28, 1, 29, 1, 30, 1, 31, 1, 
	33, 2, 0, 1, 2, 0, 2, 2, 
	8, 3, 2, 11, 3, 2, 16, 3, 
	2, 32, 3
};

static const short _json_key_offsets[] = {
	0, 0, 4, 9, 25, 27, 29, 31, 
	33, 34, 38, 43, 45, 50, 55, 59, 
	59, 60, 61, 62, 67, 69, 71, 73, 
	74, 78, 83, 85, 90, 90, 91, 92, 
	93, 98, 102, 103, 107, 115, 117, 124, 
	125, 126, 127, 132, 134, 136, 138, 139, 
	143, 148, 150, 155, 155, 156, 157, 158, 
	163, 167, 169, 170, 172, 174, 176, 177, 
	181, 186, 190, 192, 194, 196, 198, 199, 
	204, 206, 208, 210, 212, 213, 218, 219, 
	220, 221, 226, 228, 230, 231, 235, 243, 
	245, 252, 253, 254, 255, 260, 262, 264, 
	265, 267, 269, 271, 273, 274, 278, 286, 
	288, 295, 296, 297, 298, 303, 305, 307, 
	308, 312, 317, 319, 324, 324, 325, 326, 
	327, 332, 338, 340, 342, 344, 345, 347, 
	349, 351, 353, 354, 358, 366, 368, 375, 
	376, 377, 378, 383, 385, 387, 388, 392, 
	396, 400, 402, 404, 405, 410, 412, 414, 
	416, 417, 422, 424, 426, 428, 429, 433, 
	441, 443, 450, 451, 452, 453, 458, 460, 
	462, 464, 466, 467, 471, 479, 481, 488, 
	489, 490, 491, 496
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
	9, 13, 68, 78, 100, 110, 34, 32, 
	58, 9, 13, 32, 43, 45, 110, 9, 
	13, 48, 57, 48, 57, 32, 44, 125, 
	9, 13, 48, 57, 117, 108, 108, 32, 
	44, 125, 9, 13, 83, 115, 84, 116, 
	82, 114, 34, 32, 58, 9, 13, 32, 
	34, 110, 9, 13, 34, 92, 32, 44, 
	125, 9, 13, 117, 108, 108, 32, 44, 
	125, 9, 13, 73, 79, 105, 111, 81, 
	113, 95, 73, 105, 78, 110, 68, 100, 
	34, 32, 58, 9, 13, 32, 34, 110, 
	9, 13, 77, 84, 109, 116, 65, 97, 
	75, 107, 69, 101, 82, 114, 34, 32, 
	44, 125, 9, 13, 65, 97, 75, 107, 
	69, 101, 82, 114, 34, 32, 44, 125, 
	9, 13, 117, 108, 108, 32, 44, 125, 
	9, 13, 84, 116, 83, 115, 34, 32, 
	58, 9, 13, 32, 43, 45, 110, 9, 
	13, 48, 57, 48, 57, 32, 44, 125, 
	9, 13, 48, 57, 117, 108, 108, 32, 
	44, 125, 9, 13, 73, 105, 78, 110, 
	95, 76, 108, 79, 111, 84, 116, 83, 
	115, 34, 32, 58, 9, 13, 32, 43, 
	45, 110, 9, 13, 48, 57, 48, 57, 
	32, 44, 125, 9, 13, 48, 57, 117, 
	108, 108, 32, 44, 125, 9, 13, 69, 
	101, 70, 102, 34, 32, 58, 9, 13, 
	32, 34, 110, 9, 13, 34, 92, 32, 
	44, 125, 9, 13, 117, 108, 108, 32, 
	44, 125, 9, 13, 69, 73, 84, 101, 
	105, 116, 84, 116, 84, 116, 76, 108, 
	95, 68, 100, 65, 97, 84, 116, 69, 
	101, 34, 32, 58, 9, 13, 32, 43, 
	45, 110, 9, 13, 48, 57, 48, 57, 
	32, 44, 125, 9, 13, 48, 57, 117, 
	108, 108, 32, 44, 125, 9, 13, 68, 
	100, 69, 101, 34, 32, 58, 9, 13, 
	32, 34, 9, 13, 66, 83, 98, 115, 
	85, 117, 89, 121, 34, 32, 44, 125, 
	9, 13, 69, 101, 76, 108, 76, 108, 
	34, 32, 44, 125, 9, 13, 65, 97, 
	84, 116, 69, 101, 34, 32, 58, 9, 
	13, 32, 43, 45, 110, 9, 13, 48, 
	57, 48, 57, 32, 44, 125, 9, 13, 
	48, 57, 117, 108, 108, 32, 44, 125, 
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
	3, 4, 1, 2, 4, 0, 3, 1, 
	1, 1, 3, 2, 2, 2, 1, 2, 
	3, 2, 3, 0, 1, 1, 1, 3, 
	4, 2, 1, 2, 2, 2, 1, 2, 
	3, 4, 2, 2, 2, 2, 1, 3, 
	2, 2, 2, 2, 1, 3, 1, 1, 
	1, 3, 2, 2, 1, 2, 4, 0, 
	3, 1, 1, 1, 3, 2, 2, 1, 
	2, 2, 2, 2, 1, 2, 4, 0, 
	3, 1, 1, 1, 3, 2, 2, 1, 
	2, 3, 2, 3, 0, 1, 1, 1, 
	3, 6, 2, 2, 2, 1, 2, 2, 
	2, 2, 1, 2, 4, 0, 3, 1, 
	1, 1, 3, 2, 2, 1, 2, 2, 
	4, 2, 2, 1, 3, 2, 2, 2, 
	1, 3, 2, 2, 2, 1, 2, 4, 
	0, 3, 1, 1, 1, 3, 2, 2, 
	2, 2, 1, 2, 4, 0, 3, 1, 
	1, 1, 3, 1
};

static const char _json_range_lengths[] = {
	0, 1, 1, 0, 0, 0, 0, 0, 
	0, 1, 1, 0, 1, 1, 1, 0, 
	0, 0, 0, 1, 0, 0, 0, 0, 
	1, 1, 0, 1, 0, 0, 0, 0, 
	1, 0, 0, 1, 2, 1, 2, 0, 
	0, 0, 1, 0, 0, 0, 0, 1, 
	1, 0, 1, 0, 0, 0, 0, 1, 
	0, 0, 0, 0, 0, 0, 0, 1, 
	1, 0, 0, 0, 0, 0, 0, 1, 
	0, 0, 0, 0, 0, 1, 0, 0, 
	0, 1, 0, 0, 0, 1, 2, 1, 
	2, 0, 0, 0, 1, 0, 0, 0, 
	0, 0, 0, 0, 0, 1, 2, 1, 
	2, 0, 0, 0, 1, 0, 0, 0, 
	1, 1, 0, 1, 0, 0, 0, 0, 
	1, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 1, 2, 1, 2, 0, 
	0, 0, 1, 0, 0, 0, 1, 1, 
	0, 0, 0, 0, 1, 0, 0, 0, 
	0, 1, 0, 0, 0, 0, 1, 2, 
	1, 2, 0, 0, 0, 1, 0, 0, 
	0, 0, 0, 1, 2, 1, 2, 0, 
	0, 0, 1, 1
};

static const short _json_index_offsets[] = {
	0, 0, 4, 9, 26, 29, 32, 35, 
	38, 40, 44, 49, 52, 57, 62, 66, 
	67, 69, 71, 73, 78, 81, 84, 87, 
	89, 93, 98, 101, 106, 107, 109, 111, 
	113, 118, 123, 125, 129, 136, 138, 144, 
	146, 148, 150, 155, 158, 161, 164, 166, 
	170, 175, 178, 183, 184, 186, 188, 190, 
	195, 200, 203, 205, 208, 211, 214, 216, 
	220, 225, 230, 233, 236, 239, 242, 244, 
	249, 252, 255, 258, 261, 263, 268, 270, 
	272, 274, 279, 282, 285, 287, 291, 298, 
	300, 306, 308, 310, 312, 317, 320, 323, 
	325, 328, 331, 334, 337, 339, 343, 350, 
	352, 358, 360, 362, 364, 369, 372, 375, 
	377, 381, 386, 389, 394, 395, 397, 399, 
	401, 406, 413, 416, 419, 422, 424, 427, 
	430, 433, 436, 438, 442, 449, 451, 457, 
	459, 461, 463, 468, 471, 474, 476, 480, 
	484, 489, 492, 495, 497, 502, 505, 508, 
	511, 513, 518, 521, 524, 527, 529, 533, 
	540, 542, 548, 550, 552, 554, 559, 562, 
	565, 568, 571, 573, 577, 584, 586, 592, 
	594, 596, 598, 603
};

static const unsigned char _json_trans_targs[] = {
	1, 2, 1, 0, 2, 3, 179, 2, 
	0, 4, 20, 33, 56, 93, 109, 121, 
	166, 4, 20, 33, 56, 93, 109, 121, 
	166, 0, 5, 5, 0, 6, 6, 0, 
	7, 7, 0, 8, 8, 0, 9, 0, 
	9, 10, 9, 0, 10, 11, 16, 10, 
	0, 12, 15, 11, 13, 14, 179, 13, 
	0, 13, 14, 179, 13, 0, 14, 3, 
	14, 0, 11, 17, 0, 18, 0, 19, 
	0, 13, 14, 179, 13, 0, 21, 21, 
	0, 22, 22, 0, 23, 23, 0, 24, 
	0, 24, 25, 24, 0, 25, 26, 29, 
	25, 0, 27, 28, 26, 13, 14, 179, 
	13, 0, 26, 30, 0, 31, 0, 32, 
	0, 13, 14, 179, 13, 0, 34, 43, 
	34, 43, 0, 35, 0, 35, 36, 35, 
	0, 36, 37, 37, 39, 36, 38, 0, 
	38, 0, 13, 14, 179, 13, 38, 0, 
	40, 0, 41, 0, 42, 0, 13, 14, 
	179, 13, 0, 44, 44, 0, 45, 45, 
	0, 46, 46, 0, 47, 0, 47, 48, 
	47, 0, 48, 49, 52, 48, 0, 50, 
	51, 49, 13, 14, 179, 13, 0, 49, 
	53, 0, 54, 0, 55, 0, 13, 14, 
	179, 13, 0, 57, 82, 57, 82, 0, 
	58, 58, 0, 59, 0, 60, 60, 0, 
	61, 61, 0, 62, 62, 0, 63, 0, 
	63, 64, 63, 0, 64, 65, 78, 64, 
	0, 66, 72, 66, 72, 0, 67, 67, 
	0, 68, 68, 0, 69, 69, 0, 70, 
	70, 0, 71, 0, 13, 14, 179, 13, 
	0, 73, 73, 0, 74, 74, 0, 75, 
	75, 0, 76, 76, 0, 77, 0, 13, 
	14, 179, 13, 0, 79, 0, 80, 0, 
	81, 0, 13, 14, 179, 13, 0, 83, 
	83, 0, 84, 84, 0, 85, 0, 85, 
	86, 85, 0, 86, 87, 87, 89, 86, 
	88, 0, 88, 0, 13, 14, 179, 13, 
	88, 0, 90, 0, 91, 0, 92, 0, 
	13, 14, 179, 13, 0, 94, 94, 0, 
	95, 95, 0, 96, 0, 97, 97, 0, 
	98, 98, 0, 99, 99, 0, 100, 100, 
	0, 101, 0, 101, 102, 101, 0, 102, 
	103, 103, 105, 102, 104, 0, 104, 0, 
	13, 14, 179, 13, 104, 0, 106, 0, 
	107, 0, 108, 0, 13, 14, 179, 13, 
	0, 110, 110, 0, 111, 111, 0, 112, 
	0, 112, 113, 112, 0, 113, 114, 117, 
	113, 0, 115, 116, 114, 13, 14, 179, 
	13, 0, 114, 118, 0, 119, 0, 120, 
	0, 13, 14, 179, 13, 0, 122, 139, 
	154, 122, 139, 154, 0, 123, 123, 0, 
	124, 124, 0, 125, 125, 0, 126, 0, 
	127, 127, 0, 128, 128, 0, 129, 129, 
	0, 130, 130, 0, 131, 0, 131, 132, 
	131, 0, 132, 133, 133, 135, 132, 134, 
	0, 134, 0, 13, 14, 179, 13, 134, 
	0, 136, 0, 137, 0, 138, 0, 13, 
	14, 179, 13, 0, 140, 140, 0, 141, 
	141, 0, 142, 0, 142, 143, 142, 0, 
	143, 144, 143, 0, 145, 149, 145, 149, 
	0, 146, 146, 0, 147, 147, 0, 148, 
	0, 13, 14, 179, 13, 0, 150, 150, 
	0, 151, 151, 0, 152, 152, 0, 153, 
	0, 13, 14, 179, 13, 0, 155, 155, 
	0, 156, 156, 0, 157, 157, 0, 158, 
	0, 158, 159, 158, 0, 159, 160, 160, 
	162, 159, 161, 0, 161, 0, 13, 14, 
	179, 13, 161, 0, 163, 0, 164, 0, 
	165, 0, 13, 14, 179, 13, 0, 167, 
	167, 0, 168, 168, 0, 169, 169, 0, 
	170, 170, 0, 171, 0, 171, 172, 171, 
	0, 172, 173, 173, 175, 172, 174, 0, 
	174, 0, 13, 14, 179, 13, 174, 0, 
	176, 0, 177, 0, 178, 0, 13, 14, 
	179, 13, 0, 179, 179, 0, 0
};

static const char _json_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 63, 0, 0, 
	0, 0, 0, 5, 13, 13, 13, 13, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 5, 0, 0, 0, 0, 0, 
	0, 11, 11, 11, 11, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 72, 0, 
	0, 0, 0, 0, 5, 55, 55, 55, 
	55, 0, 5, 0, 0, 0, 0, 0, 
	0, 53, 53, 53, 53, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 57, 0, 0, 60, 0, 
	3, 0, 9, 9, 9, 9, 3, 0, 
	0, 0, 0, 0, 0, 0, 7, 7, 
	7, 7, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 66, 0, 0, 0, 0, 
	0, 5, 17, 17, 17, 17, 0, 5, 
	0, 0, 0, 0, 0, 0, 15, 15, 
	15, 15, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 49, 49, 49, 49, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 51, 
	51, 51, 51, 0, 0, 0, 0, 0, 
	0, 0, 47, 47, 47, 47, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 1, 57, 0, 0, 
	60, 0, 3, 0, 41, 41, 41, 41, 
	3, 0, 0, 0, 0, 0, 0, 0, 
	39, 39, 39, 39, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	1, 57, 0, 0, 60, 0, 3, 0, 
	45, 45, 45, 45, 3, 0, 0, 0, 
	0, 0, 0, 0, 43, 43, 43, 43, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 69, 0, 
	0, 0, 0, 0, 5, 25, 25, 25, 
	25, 0, 5, 0, 0, 0, 0, 0, 
	0, 23, 23, 23, 23, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 1, 57, 0, 0, 60, 
	0, 3, 0, 21, 21, 21, 21, 3, 
	0, 0, 0, 0, 0, 0, 0, 19, 
	19, 19, 19, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 35, 35, 35, 35, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 37, 37, 37, 37, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 1, 57, 
	0, 0, 60, 0, 3, 0, 29, 29, 
	29, 29, 3, 0, 0, 0, 0, 0, 
	0, 0, 27, 27, 27, 27, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 57, 0, 0, 60, 0, 
	3, 0, 33, 33, 33, 33, 3, 0, 
	0, 0, 0, 0, 0, 0, 31, 31, 
	31, 31, 0, 0, 0, 0, 0
};

static const int json_start = 1;
static const int json_first_final = 179;
static const int json_error = 0;

static const int json_en_main = 1;


#line 260 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"

} // anonymous

RestBody::~RestBody() = default;

void RestBody::reset(bool clear) noexcept
{
  decltype(cs_) cs;
  
#line 393 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.cpp"
	{
	cs = json_start;
	}

#line 269 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
  cs_ = cs;

  if (!clear) {
    return;
  }
  fields_ = 0;

  id_ = 0_id64;
  accnt_.len = 0;
  instr_.len = 0;
  settl_date_ = 0_ymd;
  ref_.len = 0;
  state_ = 0;
  side_ = static_cast<swirly::Side>(0);
  lots_ = 0_lts;
  ticks_ = 0_tks;
  min_lots_ = 0_lts;
  liq_ind_ = swirly::LiqInd::None;
  cpty_.len = 0;
}

bool RestBody::parse(string_view buf)
{
  const char* p{buf.data()};
  const char* pe{p + buf.size()};
  const char* msg{"parse error"};

  auto cs = cs_;
  
#line 428 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.cpp"
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
#line 29 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    num_.sign = 1;
    num_.digits = 0;
  }
	break;
	case 1:
#line 33 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    num_.sign = -1;
  }
	break;
	case 2:
#line 36 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    num_.digits = num_.digits * 10 + ((*p) - '0');
  }
	break;
	case 3:
#line 55 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    *str_.len = 0;
  }
	break;
	case 4:
#line 58 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
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
#line 78 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~Id;
    id_ = 0_id64;
  }
	break;
	case 6:
#line 82 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= Id;
    id_ = swirly::Id64{num()};
  }
	break;
	case 7:
#line 89 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~Accnt;
    accnt_.len = 0;
  }
	break;
	case 8:
#line 93 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    str_.len = &accnt_.len;
    str_.buf = accnt_.buf;
    str_.max = MaxSymbol;
  }
	break;
	case 9:
#line 98 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= Accnt;
  }
	break;
	case 10:
#line 104 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~Instr;
    instr_.len = 0;
  }
	break;
	case 11:
#line 108 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    str_.len = &instr_.len;
    str_.buf = instr_.buf;
    str_.max = MaxSymbol;
  }
	break;
	case 12:
#line 113 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= Instr;
  }
	break;
	case 13:
#line 119 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~SettlDate;
    settl_date_ = 0_ymd;
  }
	break;
	case 14:
#line 123 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= SettlDate;
    settl_date_ = IsoDate{num()};
  }
	break;
	case 15:
#line 130 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~Ref;
    ref_.len = 0;
  }
	break;
	case 16:
#line 134 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    str_.len = &ref_.len;
    str_.buf = ref_.buf;
    str_.max = MaxRef;
  }
	break;
	case 17:
#line 139 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= Ref;
  }
	break;
	case 18:
#line 145 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~State;
    state_ = 0;
  }
	break;
	case 19:
#line 149 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    if (num_.sign >= 0) {
      fields_ |= State;
      state_ = num();
    } else {
      cs = json_error; msg = "negative state field";
    }
  }
	break;
	case 20:
#line 160 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~Ticks;
    ticks_ = 0_tks;
  }
	break;
	case 21:
#line 164 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= Ticks;
    ticks_ = swirly::Ticks{num()};
  }
	break;
	case 22:
#line 171 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= Side;
    side_ = swirly::Side::Buy;
  }
	break;
	case 23:
#line 175 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= Side;
    side_ = swirly::Side::Sell;
  }
	break;
	case 24:
#line 182 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~Lots;
    lots_ = 0_lts;
  }
	break;
	case 25:
#line 186 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= Lots;
    lots_ = swirly::Lots{num()};
  }
	break;
	case 26:
#line 193 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~MinLots;
    min_lots_ = 0_lts;
  }
	break;
	case 27:
#line 197 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= MinLots;
    min_lots_ = swirly::Lots{num()};
  }
	break;
	case 28:
#line 204 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~LiqInd;
    liq_ind_ = swirly::LiqInd::None;
  }
	break;
	case 29:
#line 208 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= LiqInd;
    liq_ind_ = swirly::LiqInd::Maker;
  }
	break;
	case 30:
#line 212 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= LiqInd;
    liq_ind_ = swirly::LiqInd::Taker;
  }
	break;
	case 31:
#line 220 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~Cpty;
    cpty_.len = 0;
  }
	break;
	case 32:
#line 224 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    str_.len = &cpty_.len;
    str_.buf = cpty_.buf;
    str_.max = MaxSymbol;
  }
	break;
	case 33:
#line 229 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= Cpty;
  }
	break;
#line 745 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.cpp"
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

#line 298 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
  cs_ = cs;

  if (cs == json_error) {
    throw BadRequestException{msg};
  }
  if (cs < json_first_final) {
    return false;
  }
  return true;
}

} // namespace swirly

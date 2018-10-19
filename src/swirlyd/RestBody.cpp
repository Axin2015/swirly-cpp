
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


#line 261 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"



#line 34 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.cpp"
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
	93, 98, 102, 104, 105, 107, 109, 111, 
	112, 116, 121, 125, 127, 129, 131, 133, 
	134, 139, 141, 143, 145, 147, 148, 153, 
	154, 155, 156, 161, 163, 165, 166, 170, 
	178, 180, 187, 188, 189, 190, 195, 197, 
	199, 200, 202, 204, 206, 208, 209, 213, 
	221, 223, 230, 231, 232, 233, 238, 240, 
	242, 244, 246, 248, 250, 251, 255, 260, 
	262, 267, 267, 268, 269, 270, 275, 277, 
	279, 280, 284, 289, 291, 296, 296, 297, 
	298, 299, 304, 312, 314, 316, 318, 319, 
	321, 323, 325, 327, 328, 332, 340, 342, 
	349, 350, 351, 352, 357, 359, 361, 362, 
	366, 370, 374, 376, 378, 379, 384, 386, 
	388, 390, 391, 396, 398, 400, 402, 403, 
	407, 415, 417, 424, 425, 426, 427, 432, 
	434, 436, 438, 440, 441, 445, 450, 452, 
	457, 457, 458, 459, 460, 465, 467, 469, 
	471, 473, 474, 478, 486, 488, 495, 496, 
	497, 498, 503
};

static const char _json_trans_keys[] = {
	32, 123, 9, 13, 32, 34, 125, 9, 
	13, 65, 67, 76, 77, 80, 82, 83, 
	84, 97, 99, 108, 109, 112, 114, 115, 
	116, 67, 99, 67, 99, 78, 110, 84, 
	116, 34, 32, 58, 9, 13, 32, 34, 
	110, 9, 13, 34, 92, 32, 44, 125, 
	9, 13, 32, 44, 125, 9, 13, 32, 
	34, 9, 13, 117, 108, 108, 32, 44, 
	125, 9, 13, 80, 112, 84, 116, 89, 
	121, 34, 32, 58, 9, 13, 32, 34, 
	110, 9, 13, 34, 92, 32, 44, 125, 
	9, 13, 117, 108, 108, 32, 44, 125, 
	9, 13, 73, 79, 105, 111, 81, 113, 
	95, 73, 105, 78, 110, 68, 100, 34, 
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
	125, 9, 13, 73, 105, 78, 110, 95, 
	76, 108, 79, 111, 84, 116, 83, 115, 
	34, 32, 58, 9, 13, 32, 43, 45, 
	110, 9, 13, 48, 57, 48, 57, 32, 
	44, 125, 9, 13, 48, 57, 117, 108, 
	108, 32, 44, 125, 9, 13, 82, 114, 
	79, 111, 68, 100, 85, 117, 67, 99, 
	84, 116, 34, 32, 58, 9, 13, 32, 
	34, 110, 9, 13, 34, 92, 32, 44, 
	125, 9, 13, 117, 108, 108, 32, 44, 
	125, 9, 13, 69, 101, 70, 102, 34, 
	32, 58, 9, 13, 32, 34, 110, 9, 
	13, 34, 92, 32, 44, 125, 9, 13, 
	117, 108, 108, 32, 44, 125, 9, 13, 
	69, 73, 84, 89, 101, 105, 116, 121, 
	84, 116, 84, 116, 76, 108, 95, 68, 
	100, 65, 97, 84, 116, 69, 101, 34, 
	32, 58, 9, 13, 32, 43, 45, 110, 
	9, 13, 48, 57, 48, 57, 32, 44, 
	125, 9, 13, 48, 57, 117, 108, 108, 
	32, 44, 125, 9, 13, 68, 100, 69, 
	101, 34, 32, 58, 9, 13, 32, 34, 
	9, 13, 66, 83, 98, 115, 85, 117, 
	89, 121, 34, 32, 44, 125, 9, 13, 
	69, 101, 76, 108, 76, 108, 34, 32, 
	44, 125, 9, 13, 65, 97, 84, 116, 
	69, 101, 34, 32, 58, 9, 13, 32, 
	43, 45, 110, 9, 13, 48, 57, 48, 
	57, 32, 44, 125, 9, 13, 48, 57, 
	117, 108, 108, 32, 44, 125, 9, 13, 
	77, 109, 66, 98, 79, 111, 76, 108, 
	34, 32, 58, 9, 13, 32, 34, 110, 
	9, 13, 34, 92, 32, 44, 125, 9, 
	13, 117, 108, 108, 32, 44, 125, 9, 
	13, 73, 105, 67, 99, 75, 107, 83, 
	115, 34, 32, 58, 9, 13, 32, 43, 
	45, 110, 9, 13, 48, 57, 48, 57, 
	32, 44, 125, 9, 13, 48, 57, 117, 
	108, 108, 32, 44, 125, 9, 13, 32, 
	9, 13, 0
};

static const char _json_single_lengths[] = {
	0, 2, 3, 16, 2, 2, 2, 2, 
	1, 2, 3, 2, 3, 3, 2, 0, 
	1, 1, 1, 3, 2, 2, 2, 1, 
	2, 3, 2, 3, 0, 1, 1, 1, 
	3, 4, 2, 1, 2, 2, 2, 1, 
	2, 3, 4, 2, 2, 2, 2, 1, 
	3, 2, 2, 2, 2, 1, 3, 1, 
	1, 1, 3, 2, 2, 1, 2, 4, 
	0, 3, 1, 1, 1, 3, 2, 2, 
	1, 2, 2, 2, 2, 1, 2, 4, 
	0, 3, 1, 1, 1, 3, 2, 2, 
	2, 2, 2, 2, 1, 2, 3, 2, 
	3, 0, 1, 1, 1, 3, 2, 2, 
	1, 2, 3, 2, 3, 0, 1, 1, 
	1, 3, 8, 2, 2, 2, 1, 2, 
	2, 2, 2, 1, 2, 4, 0, 3, 
	1, 1, 1, 3, 2, 2, 1, 2, 
	2, 4, 2, 2, 1, 3, 2, 2, 
	2, 1, 3, 2, 2, 2, 1, 2, 
	4, 0, 3, 1, 1, 1, 3, 2, 
	2, 2, 2, 1, 2, 3, 2, 3, 
	0, 1, 1, 1, 3, 2, 2, 2, 
	2, 1, 2, 4, 0, 3, 1, 1, 
	1, 3, 1
};

static const char _json_range_lengths[] = {
	0, 1, 1, 0, 0, 0, 0, 0, 
	0, 1, 1, 0, 1, 1, 1, 0, 
	0, 0, 0, 1, 0, 0, 0, 0, 
	1, 1, 0, 1, 0, 0, 0, 0, 
	1, 0, 0, 0, 0, 0, 0, 0, 
	1, 1, 0, 0, 0, 0, 0, 0, 
	1, 0, 0, 0, 0, 0, 1, 0, 
	0, 0, 1, 0, 0, 0, 1, 2, 
	1, 2, 0, 0, 0, 1, 0, 0, 
	0, 0, 0, 0, 0, 0, 1, 2, 
	1, 2, 0, 0, 0, 1, 0, 0, 
	0, 0, 0, 0, 0, 1, 1, 0, 
	1, 0, 0, 0, 0, 1, 0, 0, 
	0, 1, 1, 0, 1, 0, 0, 0, 
	0, 1, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 1, 2, 1, 2, 
	0, 0, 0, 1, 0, 0, 0, 1, 
	1, 0, 0, 0, 0, 1, 0, 0, 
	0, 0, 1, 0, 0, 0, 0, 1, 
	2, 1, 2, 0, 0, 0, 1, 0, 
	0, 0, 0, 0, 1, 1, 0, 1, 
	0, 0, 0, 0, 1, 0, 0, 0, 
	0, 0, 1, 2, 1, 2, 0, 0, 
	0, 1, 1
};

static const short _json_index_offsets[] = {
	0, 0, 4, 9, 26, 29, 32, 35, 
	38, 40, 44, 49, 52, 57, 62, 66, 
	67, 69, 71, 73, 78, 81, 84, 87, 
	89, 93, 98, 101, 106, 107, 109, 111, 
	113, 118, 123, 126, 128, 131, 134, 137, 
	139, 143, 148, 153, 156, 159, 162, 165, 
	167, 172, 175, 178, 181, 184, 186, 191, 
	193, 195, 197, 202, 205, 208, 210, 214, 
	221, 223, 229, 231, 233, 235, 240, 243, 
	246, 248, 251, 254, 257, 260, 262, 266, 
	273, 275, 281, 283, 285, 287, 292, 295, 
	298, 301, 304, 307, 310, 312, 316, 321, 
	324, 329, 330, 332, 334, 336, 341, 344, 
	347, 349, 353, 358, 361, 366, 367, 369, 
	371, 373, 378, 387, 390, 393, 396, 398, 
	401, 404, 407, 410, 412, 416, 423, 425, 
	431, 433, 435, 437, 442, 445, 448, 450, 
	454, 458, 463, 466, 469, 471, 476, 479, 
	482, 485, 487, 492, 495, 498, 501, 503, 
	507, 514, 516, 522, 524, 526, 528, 533, 
	536, 539, 542, 545, 547, 551, 556, 559, 
	564, 565, 567, 569, 571, 576, 579, 582, 
	585, 588, 590, 594, 601, 603, 609, 611, 
	613, 615, 620
};

static const unsigned char _json_trans_targs[] = {
	1, 2, 1, 0, 2, 3, 186, 2, 
	0, 4, 20, 33, 70, 86, 102, 114, 
	173, 4, 20, 33, 70, 86, 102, 114, 
	173, 0, 5, 5, 0, 6, 6, 0, 
	7, 7, 0, 8, 8, 0, 9, 0, 
	9, 10, 9, 0, 10, 11, 16, 10, 
	0, 12, 15, 11, 13, 14, 186, 13, 
	0, 13, 14, 186, 13, 0, 14, 3, 
	14, 0, 11, 17, 0, 18, 0, 19, 
	0, 13, 14, 186, 13, 0, 21, 21, 
	0, 22, 22, 0, 23, 23, 0, 24, 
	0, 24, 25, 24, 0, 25, 26, 29, 
	25, 0, 27, 28, 26, 13, 14, 186, 
	13, 0, 26, 30, 0, 31, 0, 32, 
	0, 13, 14, 186, 13, 0, 34, 59, 
	34, 59, 0, 35, 35, 0, 36, 0, 
	37, 37, 0, 38, 38, 0, 39, 39, 
	0, 40, 0, 40, 41, 40, 0, 41, 
	42, 55, 41, 0, 43, 49, 43, 49, 
	0, 44, 44, 0, 45, 45, 0, 46, 
	46, 0, 47, 47, 0, 48, 0, 13, 
	14, 186, 13, 0, 50, 50, 0, 51, 
	51, 0, 52, 52, 0, 53, 53, 0, 
	54, 0, 13, 14, 186, 13, 0, 56, 
	0, 57, 0, 58, 0, 13, 14, 186, 
	13, 0, 60, 60, 0, 61, 61, 0, 
	62, 0, 62, 63, 62, 0, 63, 64, 
	64, 66, 63, 65, 0, 65, 0, 13, 
	14, 186, 13, 65, 0, 67, 0, 68, 
	0, 69, 0, 13, 14, 186, 13, 0, 
	71, 71, 0, 72, 72, 0, 73, 0, 
	74, 74, 0, 75, 75, 0, 76, 76, 
	0, 77, 77, 0, 78, 0, 78, 79, 
	78, 0, 79, 80, 80, 82, 79, 81, 
	0, 81, 0, 13, 14, 186, 13, 81, 
	0, 83, 0, 84, 0, 85, 0, 13, 
	14, 186, 13, 0, 87, 87, 0, 88, 
	88, 0, 89, 89, 0, 90, 90, 0, 
	91, 91, 0, 92, 92, 0, 93, 0, 
	93, 94, 93, 0, 94, 95, 98, 94, 
	0, 96, 97, 95, 13, 14, 186, 13, 
	0, 95, 99, 0, 100, 0, 101, 0, 
	13, 14, 186, 13, 0, 103, 103, 0, 
	104, 104, 0, 105, 0, 105, 106, 105, 
	0, 106, 107, 110, 106, 0, 108, 109, 
	107, 13, 14, 186, 13, 0, 107, 111, 
	0, 112, 0, 113, 0, 13, 14, 186, 
	13, 0, 115, 132, 147, 159, 115, 132, 
	147, 159, 0, 116, 116, 0, 117, 117, 
	0, 118, 118, 0, 119, 0, 120, 120, 
	0, 121, 121, 0, 122, 122, 0, 123, 
	123, 0, 124, 0, 124, 125, 124, 0, 
	125, 126, 126, 128, 125, 127, 0, 127, 
	0, 13, 14, 186, 13, 127, 0, 129, 
	0, 130, 0, 131, 0, 13, 14, 186, 
	13, 0, 133, 133, 0, 134, 134, 0, 
	135, 0, 135, 136, 135, 0, 136, 137, 
	136, 0, 138, 142, 138, 142, 0, 139, 
	139, 0, 140, 140, 0, 141, 0, 13, 
	14, 186, 13, 0, 143, 143, 0, 144, 
	144, 0, 145, 145, 0, 146, 0, 13, 
	14, 186, 13, 0, 148, 148, 0, 149, 
	149, 0, 150, 150, 0, 151, 0, 151, 
	152, 151, 0, 152, 153, 153, 155, 152, 
	154, 0, 154, 0, 13, 14, 186, 13, 
	154, 0, 156, 0, 157, 0, 158, 0, 
	13, 14, 186, 13, 0, 160, 160, 0, 
	161, 161, 0, 162, 162, 0, 163, 163, 
	0, 164, 0, 164, 165, 164, 0, 165, 
	166, 169, 165, 0, 167, 168, 166, 13, 
	14, 186, 13, 0, 166, 170, 0, 171, 
	0, 172, 0, 13, 14, 186, 13, 0, 
	174, 174, 0, 175, 175, 0, 176, 176, 
	0, 177, 177, 0, 178, 0, 178, 179, 
	178, 0, 179, 180, 180, 182, 179, 181, 
	0, 181, 0, 13, 14, 186, 13, 181, 
	0, 183, 0, 184, 0, 185, 0, 13, 
	14, 186, 13, 0, 186, 186, 0, 0
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
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 49, 
	49, 49, 49, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 51, 51, 51, 51, 0, 0, 
	0, 0, 0, 0, 0, 47, 47, 47, 
	47, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 1, 
	57, 0, 0, 60, 0, 3, 0, 41, 
	41, 41, 41, 3, 0, 0, 0, 0, 
	0, 0, 0, 39, 39, 39, 39, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 1, 57, 0, 0, 60, 
	0, 3, 0, 45, 45, 45, 45, 3, 
	0, 0, 0, 0, 0, 0, 0, 43, 
	43, 43, 43, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 69, 0, 0, 
	0, 0, 0, 5, 17, 17, 17, 17, 
	0, 5, 0, 0, 0, 0, 0, 0, 
	15, 15, 15, 15, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 72, 0, 0, 0, 0, 0, 
	5, 25, 25, 25, 25, 0, 5, 0, 
	0, 0, 0, 0, 0, 23, 23, 23, 
	23, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 57, 0, 0, 60, 0, 3, 
	0, 21, 21, 21, 21, 3, 0, 0, 
	0, 0, 0, 0, 0, 19, 19, 19, 
	19, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 35, 
	35, 35, 35, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 37, 
	37, 37, 37, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 1, 57, 0, 0, 
	60, 0, 3, 0, 29, 29, 29, 29, 
	3, 0, 0, 0, 0, 0, 0, 0, 
	27, 27, 27, 27, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	63, 0, 0, 0, 0, 0, 5, 9, 
	9, 9, 9, 0, 5, 0, 0, 0, 
	0, 0, 0, 7, 7, 7, 7, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 1, 57, 0, 0, 60, 
	0, 3, 0, 33, 33, 33, 33, 3, 
	0, 0, 0, 0, 0, 0, 0, 31, 
	31, 31, 31, 0, 0, 0, 0, 0
};

static const int json_start = 1;
static const int json_first_final = 186;
static const int json_error = 0;

static const int json_en_main = 1;


#line 264 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"

} // anonymous

RestBody::~RestBody() = default;

void RestBody::reset(bool clear) noexcept
{
  decltype(cs_) cs;
  
#line 402 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.cpp"
	{
	cs = json_start;
	}

#line 273 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
  cs_ = cs;

  if (!clear) {
    return;
  }
  fields_ = 0;

  symbol_.len = 0;
  accnt_.len = 0;
  product_.len = 0;
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
  
#line 437 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.cpp"
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
    fields_ &= ~Symbol;
    symbol_.len = 0;
  }
	break;
	case 6:
#line 82 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    str_.len = &symbol_.len;
    str_.buf = symbol_.buf;
    str_.max = MaxSymbol;
  }
	break;
	case 7:
#line 87 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= Symbol;
  }
	break;
	case 8:
#line 93 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~Accnt;
    accnt_.len = 0;
  }
	break;
	case 9:
#line 97 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    str_.len = &accnt_.len;
    str_.buf = accnt_.buf;
    str_.max = MaxSymbol;
  }
	break;
	case 10:
#line 102 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= Accnt;
  }
	break;
	case 11:
#line 108 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~Product;
    product_.len = 0;
  }
	break;
	case 12:
#line 112 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    str_.len = &product_.len;
    str_.buf = product_.buf;
    str_.max = MaxSymbol;
  }
	break;
	case 13:
#line 117 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= Product;
  }
	break;
	case 14:
#line 123 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~SettlDate;
    settl_date_ = 0_ymd;
  }
	break;
	case 15:
#line 127 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= SettlDate;
    settl_date_ = IsoDate{num()};
  }
	break;
	case 16:
#line 134 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~Ref;
    ref_.len = 0;
  }
	break;
	case 17:
#line 138 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    str_.len = &ref_.len;
    str_.buf = ref_.buf;
    str_.max = MaxRef;
  }
	break;
	case 18:
#line 143 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= Ref;
  }
	break;
	case 19:
#line 149 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~State;
    state_ = 0;
  }
	break;
	case 20:
#line 153 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
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
#line 164 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~Ticks;
    ticks_ = 0_tks;
  }
	break;
	case 22:
#line 168 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= Ticks;
    ticks_ = swirly::Ticks{num()};
  }
	break;
	case 23:
#line 175 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= Side;
    side_ = swirly::Side::Buy;
  }
	break;
	case 24:
#line 179 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= Side;
    side_ = swirly::Side::Sell;
  }
	break;
	case 25:
#line 186 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~Lots;
    lots_ = 0_lts;
  }
	break;
	case 26:
#line 190 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= Lots;
    lots_ = swirly::Lots{num()};
  }
	break;
	case 27:
#line 197 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~MinLots;
    min_lots_ = 0_lts;
  }
	break;
	case 28:
#line 201 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= MinLots;
    min_lots_ = swirly::Lots{num()};
  }
	break;
	case 29:
#line 208 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~LiqInd;
    liq_ind_ = swirly::LiqInd::None;
  }
	break;
	case 30:
#line 212 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= LiqInd;
    liq_ind_ = swirly::LiqInd::Maker;
  }
	break;
	case 31:
#line 216 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= LiqInd;
    liq_ind_ = swirly::LiqInd::Taker;
  }
	break;
	case 32:
#line 224 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ &= ~Cpty;
    cpty_.len = 0;
  }
	break;
	case 33:
#line 228 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    str_.len = &cpty_.len;
    str_.buf = cpty_.buf;
    str_.max = MaxSymbol;
  }
	break;
	case 34:
#line 233 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= Cpty;
  }
	break;
#line 761 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.cpp"
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

#line 302 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
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

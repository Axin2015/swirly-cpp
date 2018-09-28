
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
	93, 98, 100, 102, 104, 106, 107, 111, 
	116, 118, 123, 123, 124, 125, 126, 131, 
	135, 137, 138, 140, 142, 144, 145, 149, 
	154, 158, 160, 162, 164, 166, 167, 172, 
	174, 176, 178, 180, 181, 186, 187, 188, 
	189, 194, 196, 198, 199, 203, 211, 213, 
	220, 221, 222, 223, 228, 230, 232, 233, 
	235, 237, 239, 241, 242, 246, 254, 256, 
	263, 264, 265, 266, 271, 273, 275, 276, 
	280, 285, 287, 292, 292, 293, 294, 295, 
	300, 308, 310, 312, 314, 315, 317, 319, 
	321, 323, 324, 328, 336, 338, 345, 346, 
	347, 348, 353, 355, 357, 358, 362, 366, 
	370, 372, 374, 375, 380, 382, 384, 386, 
	387, 392, 394, 396, 398, 399, 403, 411, 
	413, 420, 421, 422, 423, 428, 430, 432, 
	434, 436, 437, 441, 446, 448, 453, 453, 
	454, 455, 456, 461, 463, 465, 467, 469, 
	470, 474, 482, 484, 491, 492, 493, 494, 
	499
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
	44, 125, 9, 13, 69, 73, 84, 89, 
	101, 105, 116, 121, 84, 116, 84, 116, 
	76, 108, 95, 68, 100, 65, 97, 84, 
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
	44, 125, 9, 13, 77, 109, 66, 98, 
	79, 111, 76, 108, 34, 32, 58, 9, 
	13, 32, 34, 110, 9, 13, 34, 92, 
	32, 44, 125, 9, 13, 117, 108, 108, 
	32, 44, 125, 9, 13, 73, 105, 67, 
	99, 75, 107, 83, 115, 34, 32, 58, 
	9, 13, 32, 43, 45, 110, 9, 13, 
	48, 57, 48, 57, 32, 44, 125, 9, 
	13, 48, 57, 117, 108, 108, 32, 44, 
	125, 9, 13, 32, 9, 13, 0
};

static const char _json_single_lengths[] = {
	0, 2, 3, 16, 2, 2, 2, 2, 
	1, 2, 3, 2, 3, 3, 2, 0, 
	1, 1, 1, 3, 2, 2, 2, 1, 
	2, 3, 2, 3, 0, 1, 1, 1, 
	3, 2, 2, 2, 2, 1, 2, 3, 
	2, 3, 0, 1, 1, 1, 3, 4, 
	2, 1, 2, 2, 2, 1, 2, 3, 
	4, 2, 2, 2, 2, 1, 3, 2, 
	2, 2, 2, 1, 3, 1, 1, 1, 
	3, 2, 2, 1, 2, 4, 0, 3, 
	1, 1, 1, 3, 2, 2, 1, 2, 
	2, 2, 2, 1, 2, 4, 0, 3, 
	1, 1, 1, 3, 2, 2, 1, 2, 
	3, 2, 3, 0, 1, 1, 1, 3, 
	8, 2, 2, 2, 1, 2, 2, 2, 
	2, 1, 2, 4, 0, 3, 1, 1, 
	1, 3, 2, 2, 1, 2, 2, 4, 
	2, 2, 1, 3, 2, 2, 2, 1, 
	3, 2, 2, 2, 1, 2, 4, 0, 
	3, 1, 1, 1, 3, 2, 2, 2, 
	2, 1, 2, 3, 2, 3, 0, 1, 
	1, 1, 3, 2, 2, 2, 2, 1, 
	2, 4, 0, 3, 1, 1, 1, 3, 
	1
};

static const char _json_range_lengths[] = {
	0, 1, 1, 0, 0, 0, 0, 0, 
	0, 1, 1, 0, 1, 1, 1, 0, 
	0, 0, 0, 1, 0, 0, 0, 0, 
	1, 1, 0, 1, 0, 0, 0, 0, 
	1, 0, 0, 0, 0, 0, 1, 1, 
	0, 1, 0, 0, 0, 0, 1, 0, 
	0, 0, 0, 0, 0, 0, 1, 1, 
	0, 0, 0, 0, 0, 0, 1, 0, 
	0, 0, 0, 0, 1, 0, 0, 0, 
	1, 0, 0, 0, 1, 2, 1, 2, 
	0, 0, 0, 1, 0, 0, 0, 0, 
	0, 0, 0, 0, 1, 2, 1, 2, 
	0, 0, 0, 1, 0, 0, 0, 1, 
	1, 0, 1, 0, 0, 0, 0, 1, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 2, 1, 2, 0, 0, 
	0, 1, 0, 0, 0, 1, 1, 0, 
	0, 0, 0, 1, 0, 0, 0, 0, 
	1, 0, 0, 0, 0, 1, 2, 1, 
	2, 0, 0, 0, 1, 0, 0, 0, 
	0, 0, 1, 1, 0, 1, 0, 0, 
	0, 0, 1, 0, 0, 0, 0, 0, 
	1, 2, 1, 2, 0, 0, 0, 1, 
	1
};

static const short _json_index_offsets[] = {
	0, 0, 4, 9, 26, 29, 32, 35, 
	38, 40, 44, 49, 52, 57, 62, 66, 
	67, 69, 71, 73, 78, 81, 84, 87, 
	89, 93, 98, 101, 106, 107, 109, 111, 
	113, 118, 121, 124, 127, 130, 132, 136, 
	141, 144, 149, 150, 152, 154, 156, 161, 
	166, 169, 171, 174, 177, 180, 182, 186, 
	191, 196, 199, 202, 205, 208, 210, 215, 
	218, 221, 224, 227, 229, 234, 236, 238, 
	240, 245, 248, 251, 253, 257, 264, 266, 
	272, 274, 276, 278, 283, 286, 289, 291, 
	294, 297, 300, 303, 305, 309, 316, 318, 
	324, 326, 328, 330, 335, 338, 341, 343, 
	347, 352, 355, 360, 361, 363, 365, 367, 
	372, 381, 384, 387, 390, 392, 395, 398, 
	401, 404, 406, 410, 417, 419, 425, 427, 
	429, 431, 436, 439, 442, 444, 448, 452, 
	457, 460, 463, 465, 470, 473, 476, 479, 
	481, 486, 489, 492, 495, 497, 501, 508, 
	510, 516, 518, 520, 522, 527, 530, 533, 
	536, 539, 541, 545, 550, 553, 558, 559, 
	561, 563, 565, 570, 573, 576, 579, 582, 
	584, 588, 595, 597, 603, 605, 607, 609, 
	614
};

static const unsigned char _json_trans_targs[] = {
	1, 2, 1, 0, 2, 3, 184, 2, 
	0, 4, 20, 33, 47, 84, 100, 112, 
	171, 4, 20, 33, 47, 84, 100, 112, 
	171, 0, 5, 5, 0, 6, 6, 0, 
	7, 7, 0, 8, 8, 0, 9, 0, 
	9, 10, 9, 0, 10, 11, 16, 10, 
	0, 12, 15, 11, 13, 14, 184, 13, 
	0, 13, 14, 184, 13, 0, 14, 3, 
	14, 0, 11, 17, 0, 18, 0, 19, 
	0, 13, 14, 184, 13, 0, 21, 21, 
	0, 22, 22, 0, 23, 23, 0, 24, 
	0, 24, 25, 24, 0, 25, 26, 29, 
	25, 0, 27, 28, 26, 13, 14, 184, 
	13, 0, 26, 30, 0, 31, 0, 32, 
	0, 13, 14, 184, 13, 0, 34, 34, 
	0, 35, 35, 0, 36, 36, 0, 37, 
	37, 0, 38, 0, 38, 39, 38, 0, 
	39, 40, 43, 39, 0, 41, 42, 40, 
	13, 14, 184, 13, 0, 40, 44, 0, 
	45, 0, 46, 0, 13, 14, 184, 13, 
	0, 48, 73, 48, 73, 0, 49, 49, 
	0, 50, 0, 51, 51, 0, 52, 52, 
	0, 53, 53, 0, 54, 0, 54, 55, 
	54, 0, 55, 56, 69, 55, 0, 57, 
	63, 57, 63, 0, 58, 58, 0, 59, 
	59, 0, 60, 60, 0, 61, 61, 0, 
	62, 0, 13, 14, 184, 13, 0, 64, 
	64, 0, 65, 65, 0, 66, 66, 0, 
	67, 67, 0, 68, 0, 13, 14, 184, 
	13, 0, 70, 0, 71, 0, 72, 0, 
	13, 14, 184, 13, 0, 74, 74, 0, 
	75, 75, 0, 76, 0, 76, 77, 76, 
	0, 77, 78, 78, 80, 77, 79, 0, 
	79, 0, 13, 14, 184, 13, 79, 0, 
	81, 0, 82, 0, 83, 0, 13, 14, 
	184, 13, 0, 85, 85, 0, 86, 86, 
	0, 87, 0, 88, 88, 0, 89, 89, 
	0, 90, 90, 0, 91, 91, 0, 92, 
	0, 92, 93, 92, 0, 93, 94, 94, 
	96, 93, 95, 0, 95, 0, 13, 14, 
	184, 13, 95, 0, 97, 0, 98, 0, 
	99, 0, 13, 14, 184, 13, 0, 101, 
	101, 0, 102, 102, 0, 103, 0, 103, 
	104, 103, 0, 104, 105, 108, 104, 0, 
	106, 107, 105, 13, 14, 184, 13, 0, 
	105, 109, 0, 110, 0, 111, 0, 13, 
	14, 184, 13, 0, 113, 130, 145, 157, 
	113, 130, 145, 157, 0, 114, 114, 0, 
	115, 115, 0, 116, 116, 0, 117, 0, 
	118, 118, 0, 119, 119, 0, 120, 120, 
	0, 121, 121, 0, 122, 0, 122, 123, 
	122, 0, 123, 124, 124, 126, 123, 125, 
	0, 125, 0, 13, 14, 184, 13, 125, 
	0, 127, 0, 128, 0, 129, 0, 13, 
	14, 184, 13, 0, 131, 131, 0, 132, 
	132, 0, 133, 0, 133, 134, 133, 0, 
	134, 135, 134, 0, 136, 140, 136, 140, 
	0, 137, 137, 0, 138, 138, 0, 139, 
	0, 13, 14, 184, 13, 0, 141, 141, 
	0, 142, 142, 0, 143, 143, 0, 144, 
	0, 13, 14, 184, 13, 0, 146, 146, 
	0, 147, 147, 0, 148, 148, 0, 149, 
	0, 149, 150, 149, 0, 150, 151, 151, 
	153, 150, 152, 0, 152, 0, 13, 14, 
	184, 13, 152, 0, 154, 0, 155, 0, 
	156, 0, 13, 14, 184, 13, 0, 158, 
	158, 0, 159, 159, 0, 160, 160, 0, 
	161, 161, 0, 162, 0, 162, 163, 162, 
	0, 163, 164, 167, 163, 0, 165, 166, 
	164, 13, 14, 184, 13, 0, 164, 168, 
	0, 169, 0, 170, 0, 13, 14, 184, 
	13, 0, 172, 172, 0, 173, 173, 0, 
	174, 174, 0, 175, 175, 0, 176, 0, 
	176, 177, 176, 0, 177, 178, 178, 180, 
	177, 179, 0, 179, 0, 13, 14, 184, 
	13, 179, 0, 181, 0, 182, 0, 183, 
	0, 13, 14, 184, 13, 0, 184, 184, 
	0, 0
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
	0, 0, 49, 49, 49, 49, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 51, 51, 51, 
	51, 0, 0, 0, 0, 0, 0, 0, 
	47, 47, 47, 47, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 57, 0, 0, 60, 0, 
	3, 0, 41, 41, 41, 41, 3, 0, 
	0, 0, 0, 0, 0, 0, 39, 39, 
	39, 39, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 1, 57, 
	0, 0, 60, 0, 3, 0, 45, 45, 
	45, 45, 3, 0, 0, 0, 0, 0, 
	0, 0, 43, 43, 43, 43, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 72, 0, 0, 0, 
	0, 0, 5, 25, 25, 25, 25, 0, 
	5, 0, 0, 0, 0, 0, 0, 23, 
	23, 23, 23, 0, 0, 0, 0, 0, 
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
	0, 0, 63, 0, 0, 0, 0, 0, 
	5, 9, 9, 9, 9, 0, 5, 0, 
	0, 0, 0, 0, 0, 7, 7, 7, 
	7, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 1, 57, 0, 
	0, 60, 0, 3, 0, 33, 33, 33, 
	33, 3, 0, 0, 0, 0, 0, 0, 
	0, 31, 31, 31, 31, 0, 0, 0, 
	0, 0
};

static const int json_start = 1;
static const int json_first_final = 184;
static const int json_error = 0;

static const int json_en_main = 1;


#line 264 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"

} // anonymous

RestBody::~RestBody() = default;

void RestBody::reset(bool clear) noexcept
{
  decltype(cs_) cs;
  
#line 401 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.cpp"
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
  
#line 436 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.cpp"
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
    fields_ &= ~Instr;
    instr_.len = 0;
  }
	break;
	case 12:
#line 112 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    str_.len = &instr_.len;
    str_.buf = instr_.buf;
    str_.max = MaxSymbol;
  }
	break;
	case 13:
#line 117 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.rl"
	{
    fields_ |= Instr;
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
#line 760 "/home/marayl/repo/swirly-cpp/src/swirlyd/RestBody.cpp"
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

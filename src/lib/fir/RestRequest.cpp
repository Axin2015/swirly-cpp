
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


#line 284 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"



#line 36 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.cpp"
static const char _json_actions[] = {
	0, 1, 0, 1, 2, 1, 4, 1, 
	5, 1, 7, 1, 8, 1, 10, 1, 
	11, 1, 13, 1, 14, 1, 16, 1, 
	17, 1, 18, 1, 19, 1, 21, 1, 
	22, 1, 23, 1, 24, 1, 25, 1, 
	26, 1, 27, 1, 28, 1, 29, 1, 
	30, 1, 31, 1, 32, 1, 33, 1, 
	34, 1, 35, 1, 36, 1, 38, 2, 
	0, 1, 2, 0, 2, 2, 6, 3, 
	2, 9, 3, 2, 12, 3, 2, 15, 
	3, 2, 20, 3, 2, 37, 3
};

static const short _json_key_offsets[] = {
	0, 0, 4, 9, 25, 27, 29, 31, 
	33, 34, 38, 43, 45, 50, 55, 59, 
	59, 60, 61, 62, 67, 71, 73, 75, 
	77, 78, 82, 87, 89, 94, 94, 95, 
	96, 97, 102, 104, 106, 107, 111, 116, 
	118, 123, 123, 124, 125, 126, 131, 133, 
	135, 137, 139, 141, 143, 144, 148, 153, 
	155, 160, 160, 161, 162, 163, 168, 172, 
	174, 176, 178, 180, 181, 185, 190, 194, 
	196, 198, 200, 202, 203, 208, 210, 212, 
	214, 216, 217, 222, 223, 224, 225, 230, 
	232, 234, 235, 239, 247, 249, 256, 257, 
	258, 259, 264, 268, 270, 272, 274, 276, 
	278, 279, 283, 291, 293, 300, 301, 302, 
	303, 308, 310, 312, 313, 317, 322, 324, 
	329, 329, 330, 331, 332, 337, 339, 341, 
	342, 346, 351, 353, 358, 358, 359, 360, 
	361, 366, 372, 374, 376, 378, 380, 382, 
	384, 386, 387, 391, 399, 401, 408, 409, 
	410, 411, 416, 418, 420, 421, 425, 430, 
	434, 436, 438, 439, 444, 446, 448, 450, 
	451, 456, 457, 458, 459, 464, 466, 468, 
	470, 471, 475, 483, 485, 492, 493, 494, 
	495, 500, 502, 504, 506, 508, 509, 513, 
	521, 523, 530, 531, 532, 533, 538
};

static const char _json_trans_keys[] = {
	32, 123, 9, 13, 32, 34, 125, 9, 
	13, 65, 67, 68, 76, 77, 82, 83, 
	84, 97, 99, 100, 108, 109, 114, 115, 
	116, 67, 99, 67, 99, 78, 110, 84, 
	116, 34, 32, 58, 9, 13, 32, 34, 
	110, 9, 13, 34, 92, 32, 44, 125, 
	9, 13, 32, 44, 125, 9, 13, 32, 
	34, 9, 13, 117, 108, 108, 32, 44, 
	125, 9, 13, 79, 80, 111, 112, 78, 
	110, 84, 116, 82, 114, 34, 32, 58, 
	9, 13, 32, 34, 110, 9, 13, 34, 
	92, 32, 44, 125, 9, 13, 117, 108, 
	108, 32, 44, 125, 9, 13, 84, 116, 
	89, 121, 34, 32, 58, 9, 13, 32, 
	34, 110, 9, 13, 34, 92, 32, 44, 
	125, 9, 13, 117, 108, 108, 32, 44, 
	125, 9, 13, 73, 105, 83, 115, 80, 
	112, 76, 108, 65, 97, 89, 121, 34, 
	32, 58, 9, 13, 32, 34, 110, 9, 
	13, 34, 92, 32, 44, 125, 9, 13, 
	117, 108, 108, 32, 44, 125, 9, 13, 
	73, 79, 105, 111, 81, 113, 73, 105, 
	78, 110, 68, 100, 34, 32, 58, 9, 
	13, 32, 34, 110, 9, 13, 77, 84, 
	109, 116, 65, 97, 75, 107, 69, 101, 
	82, 114, 34, 32, 44, 125, 9, 13, 
	65, 97, 75, 107, 69, 101, 82, 114, 
	34, 32, 44, 125, 9, 13, 117, 108, 
	108, 32, 44, 125, 9, 13, 84, 116, 
	83, 115, 34, 32, 58, 9, 13, 32, 
	43, 45, 110, 9, 13, 48, 57, 48, 
	57, 32, 44, 125, 9, 13, 48, 57, 
	117, 108, 108, 32, 44, 125, 9, 13, 
	73, 78, 105, 110, 78, 110, 76, 108, 
	79, 111, 84, 116, 83, 115, 34, 32, 
	58, 9, 13, 32, 43, 45, 110, 9, 
	13, 48, 57, 48, 57, 32, 44, 125, 
	9, 13, 48, 57, 117, 108, 108, 32, 
	44, 125, 9, 13, 69, 101, 77, 109, 
	34, 32, 58, 9, 13, 32, 34, 110, 
	9, 13, 34, 92, 32, 44, 125, 9, 
	13, 117, 108, 108, 32, 44, 125, 9, 
	13, 69, 101, 70, 102, 34, 32, 58, 
	9, 13, 32, 34, 110, 9, 13, 34, 
	92, 32, 44, 125, 9, 13, 117, 108, 
	108, 32, 44, 125, 9, 13, 69, 73, 
	84, 101, 105, 116, 84, 116, 84, 116, 
	76, 108, 68, 100, 65, 97, 84, 116, 
	69, 101, 34, 32, 58, 9, 13, 32, 
	43, 45, 110, 9, 13, 48, 57, 48, 
	57, 32, 44, 125, 9, 13, 48, 57, 
	117, 108, 108, 32, 44, 125, 9, 13, 
	68, 100, 69, 101, 34, 32, 58, 9, 
	13, 32, 34, 110, 9, 13, 66, 83, 
	98, 115, 85, 117, 89, 121, 34, 32, 
	44, 125, 9, 13, 69, 101, 76, 108, 
	76, 108, 34, 32, 44, 125, 9, 13, 
	117, 108, 108, 32, 44, 125, 9, 13, 
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
	0, 2, 3, 16, 2, 2, 2, 2, 
	1, 2, 3, 2, 3, 3, 2, 0, 
	1, 1, 1, 3, 4, 2, 2, 2, 
	1, 2, 3, 2, 3, 0, 1, 1, 
	1, 3, 2, 2, 1, 2, 3, 2, 
	3, 0, 1, 1, 1, 3, 2, 2, 
	2, 2, 2, 2, 1, 2, 3, 2, 
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
	1, 3, 2, 2, 1, 2, 3, 4, 
	2, 2, 1, 3, 2, 2, 2, 1, 
	3, 1, 1, 1, 3, 2, 2, 2, 
	1, 2, 4, 0, 3, 1, 1, 1, 
	3, 2, 2, 2, 2, 1, 2, 4, 
	0, 3, 1, 1, 1, 3, 1
};

static const char _json_range_lengths[] = {
	0, 1, 1, 0, 0, 0, 0, 0, 
	0, 1, 1, 0, 1, 1, 1, 0, 
	0, 0, 0, 1, 0, 0, 0, 0, 
	0, 1, 1, 0, 1, 0, 0, 0, 
	0, 1, 0, 0, 0, 1, 1, 0, 
	1, 0, 0, 0, 0, 1, 0, 0, 
	0, 0, 0, 0, 0, 1, 1, 0, 
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
	1, 0, 0, 0, 1, 0, 0, 0, 
	0, 1, 2, 1, 2, 0, 0, 0, 
	1, 0, 0, 0, 0, 0, 1, 2, 
	1, 2, 0, 0, 0, 1, 1
};

static const short _json_index_offsets[] = {
	0, 0, 4, 9, 26, 29, 32, 35, 
	38, 40, 44, 49, 52, 57, 62, 66, 
	67, 69, 71, 73, 78, 83, 86, 89, 
	92, 94, 98, 103, 106, 111, 112, 114, 
	116, 118, 123, 126, 129, 131, 135, 140, 
	143, 148, 149, 151, 153, 155, 160, 163, 
	166, 169, 172, 175, 178, 180, 184, 189, 
	192, 197, 198, 200, 202, 204, 209, 214, 
	217, 220, 223, 226, 228, 232, 237, 242, 
	245, 248, 251, 254, 256, 261, 264, 267, 
	270, 273, 275, 280, 282, 284, 286, 291, 
	294, 297, 299, 303, 310, 312, 318, 320, 
	322, 324, 329, 334, 337, 340, 343, 346, 
	349, 351, 355, 362, 364, 370, 372, 374, 
	376, 381, 384, 387, 389, 393, 398, 401, 
	406, 407, 409, 411, 413, 418, 421, 424, 
	426, 430, 435, 438, 443, 444, 446, 448, 
	450, 455, 462, 465, 468, 471, 474, 477, 
	480, 483, 485, 489, 496, 498, 504, 506, 
	508, 510, 515, 518, 521, 523, 527, 532, 
	537, 540, 543, 545, 550, 553, 556, 559, 
	561, 566, 568, 570, 572, 577, 580, 583, 
	586, 588, 592, 599, 601, 607, 609, 611, 
	613, 618, 621, 624, 627, 630, 632, 636, 
	643, 645, 651, 653, 655, 657, 662
};

static const unsigned char _json_trans_targs[] = {
	1, 2, 1, 0, 2, 3, 198, 2, 
	0, 4, 20, 46, 62, 98, 125, 137, 
	185, 4, 20, 46, 62, 98, 125, 137, 
	185, 0, 5, 5, 0, 6, 6, 0, 
	7, 7, 0, 8, 8, 0, 9, 0, 
	9, 10, 9, 0, 10, 11, 16, 10, 
	0, 12, 15, 11, 13, 14, 198, 13, 
	0, 13, 14, 198, 13, 0, 14, 3, 
	14, 0, 11, 17, 0, 18, 0, 19, 
	0, 13, 14, 198, 13, 0, 21, 34, 
	21, 34, 0, 22, 22, 0, 23, 23, 
	0, 24, 24, 0, 25, 0, 25, 26, 
	25, 0, 26, 27, 30, 26, 0, 28, 
	29, 27, 13, 14, 198, 13, 0, 27, 
	31, 0, 32, 0, 33, 0, 13, 14, 
	198, 13, 0, 35, 35, 0, 36, 36, 
	0, 37, 0, 37, 38, 37, 0, 38, 
	39, 42, 38, 0, 40, 41, 39, 13, 
	14, 198, 13, 0, 39, 43, 0, 44, 
	0, 45, 0, 13, 14, 198, 13, 0, 
	47, 47, 0, 48, 48, 0, 49, 49, 
	0, 50, 50, 0, 51, 51, 0, 52, 
	52, 0, 53, 0, 53, 54, 53, 0, 
	54, 55, 58, 54, 0, 56, 57, 55, 
	13, 14, 198, 13, 0, 55, 59, 0, 
	60, 0, 61, 0, 13, 14, 198, 13, 
	0, 63, 87, 63, 87, 0, 64, 64, 
	0, 65, 65, 0, 66, 66, 0, 67, 
	67, 0, 68, 0, 68, 69, 68, 0, 
	69, 70, 83, 69, 0, 71, 77, 71, 
	77, 0, 72, 72, 0, 73, 73, 0, 
	74, 74, 0, 75, 75, 0, 76, 0, 
	13, 14, 198, 13, 0, 78, 78, 0, 
	79, 79, 0, 80, 80, 0, 81, 81, 
	0, 82, 0, 13, 14, 198, 13, 0, 
	84, 0, 85, 0, 86, 0, 13, 14, 
	198, 13, 0, 88, 88, 0, 89, 89, 
	0, 90, 0, 90, 91, 90, 0, 91, 
	92, 92, 94, 91, 93, 0, 93, 0, 
	13, 14, 198, 13, 93, 0, 95, 0, 
	96, 0, 97, 0, 13, 14, 198, 13, 
	0, 99, 113, 99, 113, 0, 100, 100, 
	0, 101, 101, 0, 102, 102, 0, 103, 
	103, 0, 104, 104, 0, 105, 0, 105, 
	106, 105, 0, 106, 107, 107, 109, 106, 
	108, 0, 108, 0, 13, 14, 198, 13, 
	108, 0, 110, 0, 111, 0, 112, 0, 
	13, 14, 198, 13, 0, 114, 114, 0, 
	115, 115, 0, 116, 0, 116, 117, 116, 
	0, 117, 118, 121, 117, 0, 119, 120, 
	118, 13, 14, 198, 13, 0, 118, 122, 
	0, 123, 0, 124, 0, 13, 14, 198, 
	13, 0, 126, 126, 0, 127, 127, 0, 
	128, 0, 128, 129, 128, 0, 129, 130, 
	133, 129, 0, 131, 132, 130, 13, 14, 
	198, 13, 0, 130, 134, 0, 135, 0, 
	136, 0, 13, 14, 198, 13, 0, 138, 
	154, 173, 138, 154, 173, 0, 139, 139, 
	0, 140, 140, 0, 141, 141, 0, 142, 
	142, 0, 143, 143, 0, 144, 144, 0, 
	145, 145, 0, 146, 0, 146, 147, 146, 
	0, 147, 148, 148, 150, 147, 149, 0, 
	149, 0, 13, 14, 198, 13, 149, 0, 
	151, 0, 152, 0, 153, 0, 13, 14, 
	198, 13, 0, 155, 155, 0, 156, 156, 
	0, 157, 0, 157, 158, 157, 0, 158, 
	159, 169, 158, 0, 160, 164, 160, 164, 
	0, 161, 161, 0, 162, 162, 0, 163, 
	0, 13, 14, 198, 13, 0, 165, 165, 
	0, 166, 166, 0, 167, 167, 0, 168, 
	0, 13, 14, 198, 13, 0, 170, 0, 
	171, 0, 172, 0, 13, 14, 198, 13, 
	0, 174, 174, 0, 175, 175, 0, 176, 
	176, 0, 177, 0, 177, 178, 177, 0, 
	178, 179, 179, 181, 178, 180, 0, 180, 
	0, 13, 14, 198, 13, 180, 0, 182, 
	0, 183, 0, 184, 0, 13, 14, 198, 
	13, 0, 186, 186, 0, 187, 187, 0, 
	188, 188, 0, 189, 189, 0, 190, 0, 
	190, 191, 190, 0, 191, 192, 192, 194, 
	191, 193, 0, 193, 0, 13, 14, 198, 
	13, 193, 0, 195, 0, 196, 0, 197, 
	0, 13, 14, 198, 13, 0, 198, 198, 
	0, 0
};

static const char _json_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 75, 0, 0, 
	0, 0, 0, 5, 17, 17, 17, 17, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 5, 0, 0, 0, 0, 0, 
	0, 15, 15, 15, 15, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 78, 0, 0, 0, 0, 
	0, 5, 21, 21, 21, 21, 0, 5, 
	0, 0, 0, 0, 0, 0, 19, 19, 
	19, 19, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	84, 0, 0, 0, 0, 0, 5, 61, 
	61, 61, 61, 0, 5, 0, 0, 0, 
	0, 0, 0, 59, 59, 59, 59, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 72, 0, 0, 0, 0, 0, 5, 
	13, 13, 13, 13, 0, 5, 0, 0, 
	0, 0, 0, 0, 11, 11, 11, 11, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	55, 55, 55, 55, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 57, 57, 57, 57, 0, 
	0, 0, 0, 0, 0, 0, 53, 53, 
	53, 53, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	1, 63, 0, 0, 66, 0, 3, 0, 
	47, 47, 47, 47, 3, 0, 0, 0, 
	0, 0, 0, 0, 45, 45, 45, 45, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 1, 63, 0, 0, 
	66, 0, 3, 0, 51, 51, 51, 51, 
	3, 0, 0, 0, 0, 0, 0, 0, 
	49, 49, 49, 49, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 69, 0, 0, 0, 0, 0, 
	5, 9, 9, 9, 9, 0, 5, 0, 
	0, 0, 0, 0, 0, 7, 7, 7, 
	7, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 81, 
	0, 0, 0, 0, 0, 5, 29, 29, 
	29, 29, 0, 5, 0, 0, 0, 0, 
	0, 0, 27, 27, 27, 27, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 63, 0, 0, 66, 0, 
	3, 0, 25, 25, 25, 25, 3, 0, 
	0, 0, 0, 0, 0, 0, 23, 23, 
	23, 23, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 41, 41, 41, 41, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 43, 43, 43, 43, 0, 0, 0, 
	0, 0, 0, 0, 39, 39, 39, 39, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 63, 0, 0, 66, 0, 3, 
	0, 33, 33, 33, 33, 3, 0, 0, 
	0, 0, 0, 0, 0, 31, 31, 31, 
	31, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 1, 63, 0, 
	0, 66, 0, 3, 0, 37, 37, 37, 
	37, 3, 0, 0, 0, 0, 0, 0, 
	0, 35, 35, 35, 35, 0, 0, 0, 
	0, 0
};

static const int json_start = 1;
static const int json_first_final = 198;
static const int json_error = 0;

static const int json_en_main = 1;


#line 287 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"

} // anonymous

RestRequest::~RestRequest() noexcept = default;

void RestRequest::reset(bool clear) noexcept
{
  decltype(cs_) cs;
  
#line 425 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.cpp"
	{
	cs = json_start;
	}

#line 296 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
  cs_ = cs;

  if (!clear) {
    return;
  }
  fields_ = 0;

  mnem_.len = 0;
  display_.len = 0;
  accnt_.len = 0;
  contr_.len = 0;
  settlDate_ = 0_ymd;
  ref_.len = 0;
  state_ = 0;
  side_ = box<swirly::Side>(0);
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
  
#line 461 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.cpp"
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
    fields_ &= ~Display;
    display_.len = 0;
  }
	break;
	case 9:
#line 99 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    str_.len = &display_.len;
    str_.buf = display_.buf;
    str_.max = MaxDisplay;
  }
	break;
	case 10:
#line 104 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Display;
  }
	break;
	case 11:
#line 110 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Accnt;
    accnt_.len = 0;
  }
	break;
	case 12:
#line 114 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    str_.len = &accnt_.len;
    str_.buf = accnt_.buf;
    str_.max = MaxMnem;
  }
	break;
	case 13:
#line 119 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Accnt;
  }
	break;
	case 14:
#line 125 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Contr;
    contr_.len = 0;
  }
	break;
	case 15:
#line 129 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    str_.len = &contr_.len;
    str_.buf = contr_.buf;
    str_.max = MaxMnem;
  }
	break;
	case 16:
#line 134 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Contr;
  }
	break;
	case 17:
#line 140 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~SettlDate;
    settlDate_ = 0_ymd;
  }
	break;
	case 18:
#line 144 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= SettlDate;
    settlDate_ = box<IsoDate>(num());
  }
	break;
	case 19:
#line 151 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Ref;
    ref_.len = 0;
  }
	break;
	case 20:
#line 155 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    str_.len = &ref_.len;
    str_.buf = ref_.buf;
    str_.max = MaxRef;
  }
	break;
	case 21:
#line 160 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Ref;
  }
	break;
	case 22:
#line 166 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~State;
    state_ = 0;
  }
	break;
	case 23:
#line 170 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    if (num_.sign >= 0) {
      fields_ |= State;
      state_ = num();
    } else {
      cs = json_error; msg = "negative state field";
    }
  }
	break;
	case 24:
#line 181 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Ticks;
    ticks_ = 0_tks;
  }
	break;
	case 25:
#line 185 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Ticks;
    ticks_ = box<swirly::Ticks>(num());
  }
	break;
	case 26:
#line 192 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Side;
    side_ = box<swirly::Side>(0);
  }
	break;
	case 27:
#line 196 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Side;
    side_ = swirly::Side::Buy;
  }
	break;
	case 28:
#line 200 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Side;
    side_ = swirly::Side::Sell;
  }
	break;
	case 29:
#line 208 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Lots;
    lots_ = 0_lts;
  }
	break;
	case 30:
#line 212 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Lots;
    lots_ = box<swirly::Lots>(num());
  }
	break;
	case 31:
#line 219 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~MinLots;
    minLots_ = 0_lts;
  }
	break;
	case 32:
#line 223 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= MinLots;
    minLots_ = box<swirly::Lots>(num());
  }
	break;
	case 33:
#line 230 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~LiqInd;
    liqInd_ = swirly::LiqInd::None;
  }
	break;
	case 34:
#line 234 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= LiqInd;
    liqInd_ = swirly::LiqInd::Maker;
  }
	break;
	case 35:
#line 238 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= LiqInd;
    liqInd_ = swirly::LiqInd::Taker;
  }
	break;
	case 36:
#line 246 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Cpty;
    cpty_.len = 0;
  }
	break;
	case 37:
#line 250 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    str_.len = &cpty_.len;
    str_.buf = cpty_.buf;
    str_.max = MaxMnem;
  }
	break;
	case 38:
#line 255 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Cpty;
  }
	break;
#line 813 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.cpp"
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

#line 326 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
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

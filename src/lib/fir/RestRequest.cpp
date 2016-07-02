
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


#line 312 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"



#line 36 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.cpp"
static const char _json_actions[] = {
	0, 1, 0, 1, 2, 1, 4, 1, 
	5, 1, 7, 1, 8, 1, 10, 1, 
	11, 1, 13, 1, 14, 1, 16, 1, 
	17, 1, 19, 1, 20, 1, 21, 1, 
	22, 1, 23, 1, 24, 1, 26, 1, 
	27, 1, 28, 1, 29, 1, 30, 1, 
	31, 1, 32, 1, 33, 1, 34, 1, 
	35, 1, 36, 1, 37, 1, 38, 1, 
	39, 1, 40, 1, 41, 1, 43, 2, 
	0, 1, 2, 0, 2, 2, 6, 3, 
	2, 9, 3, 2, 12, 3, 2, 15, 
	3, 2, 18, 3, 2, 25, 3, 2, 
	42, 3
};

static const short _json_key_offsets[] = {
	0, 0, 4, 9, 25, 29, 31, 33, 
	35, 36, 40, 45, 47, 52, 57, 61, 
	61, 62, 63, 64, 69, 71, 73, 74, 
	78, 83, 85, 90, 90, 91, 92, 93, 
	98, 100, 102, 104, 106, 108, 110, 111, 
	115, 120, 122, 127, 127, 128, 129, 130, 
	135, 139, 141, 143, 145, 146, 150, 155, 
	157, 162, 162, 163, 164, 165, 170, 172, 
	174, 176, 178, 180, 182, 184, 186, 187, 
	191, 199, 201, 208, 209, 210, 211, 216, 
	220, 222, 224, 226, 228, 229, 233, 238, 
	242, 244, 246, 248, 250, 251, 256, 258, 
	260, 262, 264, 265, 270, 271, 272, 273, 
	278, 280, 282, 283, 287, 295, 297, 304, 
	305, 306, 307, 312, 316, 318, 320, 322, 
	324, 326, 327, 331, 339, 341, 348, 349, 
	350, 351, 356, 358, 360, 361, 365, 370, 
	372, 377, 377, 378, 379, 380, 385, 387, 
	389, 390, 394, 399, 401, 406, 406, 407, 
	408, 409, 414, 420, 422, 424, 426, 428, 
	430, 432, 434, 435, 439, 447, 449, 456, 
	457, 458, 459, 464, 466, 468, 469, 473, 
	478, 482, 484, 486, 487, 492, 494, 496, 
	498, 499, 504, 505, 506, 507, 512, 514, 
	516, 518, 519, 523, 531, 533, 540, 541, 
	542, 543, 548, 552, 554, 556, 558, 559, 
	563, 571, 573, 580, 581, 582, 583, 588, 
	590, 592, 594, 596, 597, 601, 606, 608, 
	613, 613, 614, 615, 616, 621
};

static const char _json_trans_keys[] = {
	32, 123, 9, 13, 32, 34, 125, 9, 
	13, 67, 68, 69, 76, 77, 82, 83, 
	84, 99, 100, 101, 108, 109, 114, 115, 
	116, 79, 80, 111, 112, 78, 110, 84, 
	116, 82, 114, 34, 32, 58, 9, 13, 
	32, 34, 110, 9, 13, 34, 92, 32, 
	44, 125, 9, 13, 32, 44, 125, 9, 
	13, 32, 34, 9, 13, 117, 108, 108, 
	32, 44, 125, 9, 13, 84, 116, 89, 
	121, 34, 32, 58, 9, 13, 32, 34, 
	110, 9, 13, 34, 92, 32, 44, 125, 
	9, 13, 117, 108, 108, 32, 44, 125, 
	9, 13, 73, 105, 83, 115, 80, 112, 
	76, 108, 65, 97, 89, 121, 34, 32, 
	58, 9, 13, 32, 34, 110, 9, 13, 
	34, 92, 32, 44, 125, 9, 13, 117, 
	108, 108, 32, 44, 125, 9, 13, 77, 
	88, 109, 120, 65, 97, 73, 105, 76, 
	108, 34, 32, 58, 9, 13, 32, 34, 
	110, 9, 13, 34, 92, 32, 44, 125, 
	9, 13, 117, 108, 108, 32, 44, 125, 
	9, 13, 80, 112, 73, 105, 82, 114, 
	89, 121, 68, 100, 65, 97, 84, 116, 
	69, 101, 34, 32, 58, 9, 13, 32, 
	43, 45, 110, 9, 13, 48, 57, 48, 
	57, 32, 44, 125, 9, 13, 48, 57, 
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
	44, 125, 9, 13, 73, 82, 105, 114, 
	67, 99, 75, 107, 83, 115, 34, 32, 
	58, 9, 13, 32, 43, 45, 110, 9, 
	13, 48, 57, 48, 57, 32, 44, 125, 
	9, 13, 48, 57, 117, 108, 108, 32, 
	44, 125, 9, 13, 65, 97, 68, 100, 
	69, 101, 82, 114, 34, 32, 58, 9, 
	13, 32, 34, 110, 9, 13, 34, 92, 
	32, 44, 125, 9, 13, 117, 108, 108, 
	32, 44, 125, 9, 13, 32, 9, 13, 
	0
};

static const char _json_single_lengths[] = {
	0, 2, 3, 16, 4, 2, 2, 2, 
	1, 2, 3, 2, 3, 3, 2, 0, 
	1, 1, 1, 3, 2, 2, 1, 2, 
	3, 2, 3, 0, 1, 1, 1, 3, 
	2, 2, 2, 2, 2, 2, 1, 2, 
	3, 2, 3, 0, 1, 1, 1, 3, 
	4, 2, 2, 2, 1, 2, 3, 2, 
	3, 0, 1, 1, 1, 3, 2, 2, 
	2, 2, 2, 2, 2, 2, 1, 2, 
	4, 0, 3, 1, 1, 1, 3, 4, 
	2, 2, 2, 2, 1, 2, 3, 4, 
	2, 2, 2, 2, 1, 3, 2, 2, 
	2, 2, 1, 3, 1, 1, 1, 3, 
	2, 2, 1, 2, 4, 0, 3, 1, 
	1, 1, 3, 4, 2, 2, 2, 2, 
	2, 1, 2, 4, 0, 3, 1, 1, 
	1, 3, 2, 2, 1, 2, 3, 2, 
	3, 0, 1, 1, 1, 3, 2, 2, 
	1, 2, 3, 2, 3, 0, 1, 1, 
	1, 3, 6, 2, 2, 2, 2, 2, 
	2, 2, 1, 2, 4, 0, 3, 1, 
	1, 1, 3, 2, 2, 1, 2, 3, 
	4, 2, 2, 1, 3, 2, 2, 2, 
	1, 3, 1, 1, 1, 3, 2, 2, 
	2, 1, 2, 4, 0, 3, 1, 1, 
	1, 3, 4, 2, 2, 2, 1, 2, 
	4, 0, 3, 1, 1, 1, 3, 2, 
	2, 2, 2, 1, 2, 3, 2, 3, 
	0, 1, 1, 1, 3, 1
};

static const char _json_range_lengths[] = {
	0, 1, 1, 0, 0, 0, 0, 0, 
	0, 1, 1, 0, 1, 1, 1, 0, 
	0, 0, 0, 1, 0, 0, 0, 1, 
	1, 0, 1, 0, 0, 0, 0, 1, 
	0, 0, 0, 0, 0, 0, 0, 1, 
	1, 0, 1, 0, 0, 0, 0, 1, 
	0, 0, 0, 0, 0, 1, 1, 0, 
	1, 0, 0, 0, 0, 1, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 1, 
	2, 1, 2, 0, 0, 0, 1, 0, 
	0, 0, 0, 0, 0, 1, 1, 0, 
	0, 0, 0, 0, 0, 1, 0, 0, 
	0, 0, 0, 1, 0, 0, 0, 1, 
	0, 0, 0, 1, 2, 1, 2, 0, 
	0, 0, 1, 0, 0, 0, 0, 0, 
	0, 0, 1, 2, 1, 2, 0, 0, 
	0, 1, 0, 0, 0, 1, 1, 0, 
	1, 0, 0, 0, 0, 1, 0, 0, 
	0, 1, 1, 0, 1, 0, 0, 0, 
	0, 1, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 1, 2, 1, 2, 0, 
	0, 0, 1, 0, 0, 0, 1, 1, 
	0, 0, 0, 0, 1, 0, 0, 0, 
	0, 1, 0, 0, 0, 1, 0, 0, 
	0, 0, 1, 2, 1, 2, 0, 0, 
	0, 1, 0, 0, 0, 0, 0, 1, 
	2, 1, 2, 0, 0, 0, 1, 0, 
	0, 0, 0, 0, 1, 1, 0, 1, 
	0, 0, 0, 0, 1, 1
};

static const short _json_index_offsets[] = {
	0, 0, 4, 9, 26, 31, 34, 37, 
	40, 42, 46, 51, 54, 59, 64, 68, 
	69, 71, 73, 75, 80, 83, 86, 88, 
	92, 97, 100, 105, 106, 108, 110, 112, 
	117, 120, 123, 126, 129, 132, 135, 137, 
	141, 146, 149, 154, 155, 157, 159, 161, 
	166, 171, 174, 177, 180, 182, 186, 191, 
	194, 199, 200, 202, 204, 206, 211, 214, 
	217, 220, 223, 226, 229, 232, 235, 237, 
	241, 248, 250, 256, 258, 260, 262, 267, 
	272, 275, 278, 281, 284, 286, 290, 295, 
	300, 303, 306, 309, 312, 314, 319, 322, 
	325, 328, 331, 333, 338, 340, 342, 344, 
	349, 352, 355, 357, 361, 368, 370, 376, 
	378, 380, 382, 387, 392, 395, 398, 401, 
	404, 407, 409, 413, 420, 422, 428, 430, 
	432, 434, 439, 442, 445, 447, 451, 456, 
	459, 464, 465, 467, 469, 471, 476, 479, 
	482, 484, 488, 493, 496, 501, 502, 504, 
	506, 508, 513, 520, 523, 526, 529, 532, 
	535, 538, 541, 543, 547, 554, 556, 562, 
	564, 566, 568, 573, 576, 579, 581, 585, 
	590, 595, 598, 601, 603, 608, 611, 614, 
	617, 619, 624, 626, 628, 630, 635, 638, 
	641, 644, 646, 650, 657, 659, 665, 667, 
	669, 671, 676, 681, 684, 687, 690, 692, 
	696, 703, 705, 711, 713, 715, 717, 722, 
	725, 728, 731, 734, 736, 740, 745, 748, 
	753, 754, 756, 758, 760, 765
};

static const unsigned char _json_trans_targs[] = {
	1, 2, 1, 0, 2, 3, 229, 2, 
	0, 4, 32, 48, 79, 115, 142, 154, 
	202, 4, 32, 48, 79, 115, 142, 154, 
	202, 0, 5, 20, 5, 20, 0, 6, 
	6, 0, 7, 7, 0, 8, 8, 0, 
	9, 0, 9, 10, 9, 0, 10, 11, 
	16, 10, 0, 12, 15, 11, 13, 14, 
	229, 13, 0, 13, 14, 229, 13, 0, 
	14, 3, 14, 0, 11, 17, 0, 18, 
	0, 19, 0, 13, 14, 229, 13, 0, 
	21, 21, 0, 22, 22, 0, 23, 0, 
	23, 24, 23, 0, 24, 25, 28, 24, 
	0, 26, 27, 25, 13, 14, 229, 13, 
	0, 25, 29, 0, 30, 0, 31, 0, 
	13, 14, 229, 13, 0, 33, 33, 0, 
	34, 34, 0, 35, 35, 0, 36, 36, 
	0, 37, 37, 0, 38, 38, 0, 39, 
	0, 39, 40, 39, 0, 40, 41, 44, 
	40, 0, 42, 43, 41, 13, 14, 229, 
	13, 0, 41, 45, 0, 46, 0, 47, 
	0, 13, 14, 229, 13, 0, 49, 62, 
	49, 62, 0, 50, 50, 0, 51, 51, 
	0, 52, 52, 0, 53, 0, 53, 54, 
	53, 0, 54, 55, 58, 54, 0, 56, 
	57, 55, 13, 14, 229, 13, 0, 55, 
	59, 0, 60, 0, 61, 0, 13, 14, 
	229, 13, 0, 63, 63, 0, 64, 64, 
	0, 65, 65, 0, 66, 66, 0, 67, 
	67, 0, 68, 68, 0, 69, 69, 0, 
	70, 70, 0, 71, 0, 71, 72, 71, 
	0, 72, 73, 73, 75, 72, 74, 0, 
	74, 0, 13, 14, 229, 13, 74, 0, 
	76, 0, 77, 0, 78, 0, 13, 14, 
	229, 13, 0, 80, 104, 80, 104, 0, 
	81, 81, 0, 82, 82, 0, 83, 83, 
	0, 84, 84, 0, 85, 0, 85, 86, 
	85, 0, 86, 87, 100, 86, 0, 88, 
	94, 88, 94, 0, 89, 89, 0, 90, 
	90, 0, 91, 91, 0, 92, 92, 0, 
	93, 0, 13, 14, 229, 13, 0, 95, 
	95, 0, 96, 96, 0, 97, 97, 0, 
	98, 98, 0, 99, 0, 13, 14, 229, 
	13, 0, 101, 0, 102, 0, 103, 0, 
	13, 14, 229, 13, 0, 105, 105, 0, 
	106, 106, 0, 107, 0, 107, 108, 107, 
	0, 108, 109, 109, 111, 108, 110, 0, 
	110, 0, 13, 14, 229, 13, 110, 0, 
	112, 0, 113, 0, 114, 0, 13, 14, 
	229, 13, 0, 116, 130, 116, 130, 0, 
	117, 117, 0, 118, 118, 0, 119, 119, 
	0, 120, 120, 0, 121, 121, 0, 122, 
	0, 122, 123, 122, 0, 123, 124, 124, 
	126, 123, 125, 0, 125, 0, 13, 14, 
	229, 13, 125, 0, 127, 0, 128, 0, 
	129, 0, 13, 14, 229, 13, 0, 131, 
	131, 0, 132, 132, 0, 133, 0, 133, 
	134, 133, 0, 134, 135, 138, 134, 0, 
	136, 137, 135, 13, 14, 229, 13, 0, 
	135, 139, 0, 140, 0, 141, 0, 13, 
	14, 229, 13, 0, 143, 143, 0, 144, 
	144, 0, 145, 0, 145, 146, 145, 0, 
	146, 147, 150, 146, 0, 148, 149, 147, 
	13, 14, 229, 13, 0, 147, 151, 0, 
	152, 0, 153, 0, 13, 14, 229, 13, 
	0, 155, 171, 190, 155, 171, 190, 0, 
	156, 156, 0, 157, 157, 0, 158, 158, 
	0, 159, 159, 0, 160, 160, 0, 161, 
	161, 0, 162, 162, 0, 163, 0, 163, 
	164, 163, 0, 164, 165, 165, 167, 164, 
	166, 0, 166, 0, 13, 14, 229, 13, 
	166, 0, 168, 0, 169, 0, 170, 0, 
	13, 14, 229, 13, 0, 172, 172, 0, 
	173, 173, 0, 174, 0, 174, 175, 174, 
	0, 175, 176, 186, 175, 0, 177, 181, 
	177, 181, 0, 178, 178, 0, 179, 179, 
	0, 180, 0, 13, 14, 229, 13, 0, 
	182, 182, 0, 183, 183, 0, 184, 184, 
	0, 185, 0, 13, 14, 229, 13, 0, 
	187, 0, 188, 0, 189, 0, 13, 14, 
	229, 13, 0, 191, 191, 0, 192, 192, 
	0, 193, 193, 0, 194, 0, 194, 195, 
	194, 0, 195, 196, 196, 198, 195, 197, 
	0, 197, 0, 13, 14, 229, 13, 197, 
	0, 199, 0, 200, 0, 201, 0, 13, 
	14, 229, 13, 0, 203, 215, 203, 215, 
	0, 204, 204, 0, 205, 205, 0, 206, 
	206, 0, 207, 0, 207, 208, 207, 0, 
	208, 209, 209, 211, 208, 210, 0, 210, 
	0, 13, 14, 229, 13, 210, 0, 212, 
	0, 213, 0, 214, 0, 13, 14, 229, 
	13, 0, 216, 216, 0, 217, 217, 0, 
	218, 218, 0, 219, 219, 0, 220, 0, 
	220, 221, 220, 0, 221, 222, 225, 221, 
	0, 223, 224, 222, 13, 14, 229, 13, 
	0, 222, 226, 0, 227, 0, 228, 0, 
	13, 14, 229, 13, 0, 229, 229, 0, 
	0
};

static const char _json_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 89, 
	0, 0, 0, 0, 0, 5, 25, 25, 
	25, 25, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 5, 0, 0, 0, 
	0, 0, 0, 23, 23, 23, 23, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 95, 0, 0, 
	0, 0, 0, 5, 69, 69, 69, 69, 
	0, 5, 0, 0, 0, 0, 0, 0, 
	67, 67, 67, 67, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 80, 0, 
	0, 0, 0, 0, 5, 13, 13, 13, 
	13, 0, 5, 0, 0, 0, 0, 0, 
	0, 11, 11, 11, 11, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 83, 0, 0, 0, 0, 
	0, 5, 17, 17, 17, 17, 0, 5, 
	0, 0, 0, 0, 0, 0, 15, 15, 
	15, 15, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 71, 0, 0, 74, 0, 
	3, 0, 33, 33, 33, 33, 3, 0, 
	0, 0, 0, 0, 0, 0, 31, 31, 
	31, 31, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 63, 63, 63, 63, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 65, 65, 65, 
	65, 0, 0, 0, 0, 0, 0, 0, 
	61, 61, 61, 61, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 71, 0, 0, 74, 0, 
	3, 0, 55, 55, 55, 55, 3, 0, 
	0, 0, 0, 0, 0, 0, 53, 53, 
	53, 53, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 1, 71, 
	0, 0, 74, 0, 3, 0, 59, 59, 
	59, 59, 3, 0, 0, 0, 0, 0, 
	0, 0, 57, 57, 57, 57, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 77, 0, 0, 0, 
	0, 0, 5, 9, 9, 9, 9, 0, 
	5, 0, 0, 0, 0, 0, 0, 7, 
	7, 7, 7, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 92, 0, 0, 0, 0, 0, 5, 
	37, 37, 37, 37, 0, 5, 0, 0, 
	0, 0, 0, 0, 35, 35, 35, 35, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 1, 71, 0, 0, 
	74, 0, 3, 0, 29, 29, 29, 29, 
	3, 0, 0, 0, 0, 0, 0, 0, 
	27, 27, 27, 27, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 49, 49, 49, 49, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 51, 51, 51, 51, 0, 
	0, 0, 0, 0, 0, 0, 47, 47, 
	47, 47, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 1, 71, 0, 0, 74, 
	0, 3, 0, 41, 41, 41, 41, 3, 
	0, 0, 0, 0, 0, 0, 0, 39, 
	39, 39, 39, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 71, 0, 0, 74, 0, 3, 
	0, 45, 45, 45, 45, 3, 0, 0, 
	0, 0, 0, 0, 0, 43, 43, 43, 
	43, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 86, 0, 0, 
	0, 0, 0, 5, 21, 21, 21, 21, 
	0, 5, 0, 0, 0, 0, 0, 0, 
	19, 19, 19, 19, 0, 0, 0, 0, 
	0
};

static const int json_start = 1;
static const int json_first_final = 229;
static const int json_error = 0;

static const int json_en_main = 1;


#line 315 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"

} // anonymous

RestRequest::~RestRequest() noexcept = default;

void RestRequest::reset(bool clear) noexcept
{
  decltype(cs_) cs;
  
#line 480 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.cpp"
	{
	cs = json_start;
	}

#line 324 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
  cs_ = cs;

  if (!clear) {
    return;
  }
  fields_ = 0;

  mnem_.len = 0;
  display_.len = 0;
  email_.len = 0;
  trader_.len = 0;
  contr_.len = 0;
  settlDate_ = 0_dt;
  expiryDate_ = 0_dt;
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
  
#line 518 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.cpp"
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
    fields_ &= ~Email;
    email_.len = 0;
  }
	break;
	case 12:
#line 114 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    str_.len = &email_.len;
    str_.buf = email_.buf;
    str_.max = MaxEmail;
  }
	break;
	case 13:
#line 119 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Email;
  }
	break;
	case 14:
#line 125 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Trader;
    trader_.len = 0;
  }
	break;
	case 15:
#line 129 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    str_.len = &trader_.len;
    str_.buf = trader_.buf;
    str_.max = MaxMnem;
  }
	break;
	case 16:
#line 134 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Trader;
  }
	break;
	case 17:
#line 140 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Contr;
    contr_.len = 0;
  }
	break;
	case 18:
#line 144 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    str_.len = &contr_.len;
    str_.buf = contr_.buf;
    str_.max = MaxMnem;
  }
	break;
	case 19:
#line 149 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Contr;
  }
	break;
	case 20:
#line 155 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~SettlDate;
    settlDate_ = 0_dt;
  }
	break;
	case 21:
#line 159 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= SettlDate;
    settlDate_ = box<IsoDate>(num());
  }
	break;
	case 22:
#line 166 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~ExpiryDate;
    expiryDate_ = 0_dt;
  }
	break;
	case 23:
#line 170 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= ExpiryDate;
    expiryDate_ = box<IsoDate>(num());
  }
	break;
	case 24:
#line 177 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Ref;
    ref_.len = 0;
  }
	break;
	case 25:
#line 181 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    str_.len = &ref_.len;
    str_.buf = ref_.buf;
    str_.max = MaxRef;
  }
	break;
	case 26:
#line 186 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Ref;
  }
	break;
	case 27:
#line 192 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~State;
    state_ = 0;
  }
	break;
	case 28:
#line 196 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    if (num_.sign >= 0) {
      fields_ |= State;
      state_ = num();
    } else {
      cs = json_error; msg = "negative state field";
    }
  }
	break;
	case 29:
#line 207 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Ticks;
    ticks_ = 0_tks;
  }
	break;
	case 30:
#line 211 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Ticks;
    ticks_ = box<swirly::Ticks>(num());
  }
	break;
	case 31:
#line 218 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Side;
    side_ = box<swirly::Side>(0);
  }
	break;
	case 32:
#line 222 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Side;
    side_ = swirly::Side::Buy;
  }
	break;
	case 33:
#line 226 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Side;
    side_ = swirly::Side::Sell;
  }
	break;
	case 34:
#line 234 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Lots;
    lots_ = 0_lts;
  }
	break;
	case 35:
#line 238 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Lots;
    lots_ = box<swirly::Lots>(num());
  }
	break;
	case 36:
#line 245 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~MinLots;
    minLots_ = 0_lts;
  }
	break;
	case 37:
#line 249 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= MinLots;
    minLots_ = box<swirly::Lots>(num());
  }
	break;
	case 38:
#line 256 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~LiqInd;
    liqInd_ = swirly::LiqInd::None;
  }
	break;
	case 39:
#line 260 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= LiqInd;
    liqInd_ = swirly::LiqInd::Maker;
  }
	break;
	case 40:
#line 264 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= LiqInd;
    liqInd_ = swirly::LiqInd::Taker;
  }
	break;
	case 41:
#line 272 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Cpty;
    cpty_.len = 0;
  }
	break;
	case 42:
#line 276 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    str_.len = &cpty_.len;
    str_.buf = cpty_.buf;
    str_.max = MaxMnem;
  }
	break;
	case 43:
#line 281 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Cpty;
  }
	break;
#line 905 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.cpp"
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

#line 356 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
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

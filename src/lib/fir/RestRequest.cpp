
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


#line 296 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"



#line 36 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.cpp"
static const char _json_actions[] = {
	0, 1, 0, 1, 2, 1, 4, 1, 
	5, 1, 7, 1, 8, 1, 10, 1, 
	11, 1, 13, 1, 14, 1, 16, 1, 
	17, 1, 18, 1, 19, 1, 20, 1, 
	21, 1, 23, 1, 24, 1, 25, 1, 
	26, 1, 27, 1, 28, 1, 29, 1, 
	30, 1, 31, 1, 32, 1, 33, 1, 
	34, 1, 35, 1, 36, 1, 37, 1, 
	38, 1, 40, 2, 0, 1, 2, 0, 
	2, 2, 6, 3, 2, 9, 3, 2, 
	12, 3, 2, 15, 3, 2, 22, 3, 
	2, 39, 3
};

static const short _json_key_offsets[] = {
	0, 0, 4, 9, 27, 29, 31, 33, 
	35, 36, 40, 45, 47, 52, 57, 61, 
	61, 62, 63, 64, 69, 73, 75, 77, 
	79, 80, 84, 89, 91, 96, 96, 97, 
	98, 99, 104, 106, 108, 109, 113, 118, 
	120, 125, 125, 126, 127, 128, 133, 135, 
	137, 139, 141, 143, 145, 146, 150, 155, 
	157, 162, 162, 163, 164, 165, 170, 172, 
	174, 176, 178, 180, 182, 184, 186, 188, 
	189, 193, 201, 203, 210, 211, 212, 213, 
	218, 222, 224, 226, 228, 230, 231, 235, 
	240, 244, 246, 248, 250, 252, 253, 258, 
	260, 262, 264, 266, 267, 272, 273, 274, 
	275, 280, 282, 284, 285, 289, 297, 299, 
	306, 307, 308, 309, 314, 318, 320, 322, 
	324, 326, 328, 329, 333, 341, 343, 350, 
	351, 352, 353, 358, 360, 362, 363, 367, 
	372, 374, 379, 379, 380, 381, 382, 387, 
	389, 391, 392, 396, 401, 403, 408, 408, 
	409, 410, 411, 416, 422, 424, 426, 428, 
	430, 432, 434, 436, 437, 441, 449, 451, 
	458, 459, 460, 461, 466, 468, 470, 471, 
	475, 480, 484, 486, 488, 489, 494, 496, 
	498, 500, 501, 506, 507, 508, 509, 514, 
	516, 518, 520, 521, 525, 533, 535, 542, 
	543, 544, 545, 550, 552, 554, 556, 558, 
	559, 563, 571, 573, 580, 581, 582, 583, 
	588
};

static const char _json_trans_keys[] = {
	32, 123, 9, 13, 32, 34, 125, 9, 
	13, 65, 67, 68, 69, 76, 77, 82, 
	83, 84, 97, 99, 100, 101, 108, 109, 
	114, 115, 116, 67, 99, 67, 99, 78, 
	110, 84, 116, 34, 32, 58, 9, 13, 
	32, 34, 110, 9, 13, 34, 92, 32, 
	44, 125, 9, 13, 32, 44, 125, 9, 
	13, 32, 34, 9, 13, 117, 108, 108, 
	32, 44, 125, 9, 13, 79, 80, 111, 
	112, 78, 110, 84, 116, 82, 114, 34, 
	32, 58, 9, 13, 32, 34, 110, 9, 
	13, 34, 92, 32, 44, 125, 9, 13, 
	117, 108, 108, 32, 44, 125, 9, 13, 
	84, 116, 89, 121, 34, 32, 58, 9, 
	13, 32, 34, 110, 9, 13, 34, 92, 
	32, 44, 125, 9, 13, 117, 108, 108, 
	32, 44, 125, 9, 13, 73, 105, 83, 
	115, 80, 112, 76, 108, 65, 97, 89, 
	121, 34, 32, 58, 9, 13, 32, 34, 
	110, 9, 13, 34, 92, 32, 44, 125, 
	9, 13, 117, 108, 108, 32, 44, 125, 
	9, 13, 88, 120, 80, 112, 73, 105, 
	82, 114, 89, 121, 68, 100, 65, 97, 
	84, 116, 69, 101, 34, 32, 58, 9, 
	13, 32, 43, 45, 110, 9, 13, 48, 
	57, 48, 57, 32, 44, 125, 9, 13, 
	48, 57, 117, 108, 108, 32, 44, 125, 
	9, 13, 73, 79, 105, 111, 81, 113, 
	73, 105, 78, 110, 68, 100, 34, 32, 
	58, 9, 13, 32, 34, 110, 9, 13, 
	77, 84, 109, 116, 65, 97, 75, 107, 
	69, 101, 82, 114, 34, 32, 44, 125, 
	9, 13, 65, 97, 75, 107, 69, 101, 
	82, 114, 34, 32, 44, 125, 9, 13, 
	117, 108, 108, 32, 44, 125, 9, 13, 
	84, 116, 83, 115, 34, 32, 58, 9, 
	13, 32, 43, 45, 110, 9, 13, 48, 
	57, 48, 57, 32, 44, 125, 9, 13, 
	48, 57, 117, 108, 108, 32, 44, 125, 
	9, 13, 73, 78, 105, 110, 78, 110, 
	76, 108, 79, 111, 84, 116, 83, 115, 
	34, 32, 58, 9, 13, 32, 43, 45, 
	110, 9, 13, 48, 57, 48, 57, 32, 
	44, 125, 9, 13, 48, 57, 117, 108, 
	108, 32, 44, 125, 9, 13, 69, 101, 
	77, 109, 34, 32, 58, 9, 13, 32, 
	34, 110, 9, 13, 34, 92, 32, 44, 
	125, 9, 13, 117, 108, 108, 32, 44, 
	125, 9, 13, 69, 101, 70, 102, 34, 
	32, 58, 9, 13, 32, 34, 110, 9, 
	13, 34, 92, 32, 44, 125, 9, 13, 
	117, 108, 108, 32, 44, 125, 9, 13, 
	69, 73, 84, 101, 105, 116, 84, 116, 
	84, 116, 76, 108, 68, 100, 65, 97, 
	84, 116, 69, 101, 34, 32, 58, 9, 
	13, 32, 43, 45, 110, 9, 13, 48, 
	57, 48, 57, 32, 44, 125, 9, 13, 
	48, 57, 117, 108, 108, 32, 44, 125, 
	9, 13, 68, 100, 69, 101, 34, 32, 
	58, 9, 13, 32, 34, 110, 9, 13, 
	66, 83, 98, 115, 85, 117, 89, 121, 
	34, 32, 44, 125, 9, 13, 69, 101, 
	76, 108, 76, 108, 34, 32, 44, 125, 
	9, 13, 117, 108, 108, 32, 44, 125, 
	9, 13, 65, 97, 84, 116, 69, 101, 
	34, 32, 58, 9, 13, 32, 43, 45, 
	110, 9, 13, 48, 57, 48, 57, 32, 
	44, 125, 9, 13, 48, 57, 117, 108, 
	108, 32, 44, 125, 9, 13, 73, 105, 
	67, 99, 75, 107, 83, 115, 34, 32, 
	58, 9, 13, 32, 43, 45, 110, 9, 
	13, 48, 57, 48, 57, 32, 44, 125, 
	9, 13, 48, 57, 117, 108, 108, 32, 
	44, 125, 9, 13, 32, 9, 13, 0
};

static const char _json_single_lengths[] = {
	0, 2, 3, 18, 2, 2, 2, 2, 
	1, 2, 3, 2, 3, 3, 2, 0, 
	1, 1, 1, 3, 4, 2, 2, 2, 
	1, 2, 3, 2, 3, 0, 1, 1, 
	1, 3, 2, 2, 1, 2, 3, 2, 
	3, 0, 1, 1, 1, 3, 2, 2, 
	2, 2, 2, 2, 1, 2, 3, 2, 
	3, 0, 1, 1, 1, 3, 2, 2, 
	2, 2, 2, 2, 2, 2, 2, 1, 
	2, 4, 0, 3, 1, 1, 1, 3, 
	4, 2, 2, 2, 2, 1, 2, 3, 
	4, 2, 2, 2, 2, 1, 3, 2, 
	2, 2, 2, 1, 3, 1, 1, 1, 
	3, 2, 2, 1, 2, 4, 0, 3, 
	1, 1, 1, 3, 4, 2, 2, 2, 
	2, 2, 1, 2, 4, 0, 3, 1, 
	1, 1, 3, 2, 2, 1, 2, 3, 
	2, 3, 0, 1, 1, 1, 3, 2, 
	2, 1, 2, 3, 2, 3, 0, 1, 
	1, 1, 3, 6, 2, 2, 2, 2, 
	2, 2, 2, 1, 2, 4, 0, 3, 
	1, 1, 1, 3, 2, 2, 1, 2, 
	3, 4, 2, 2, 1, 3, 2, 2, 
	2, 1, 3, 1, 1, 1, 3, 2, 
	2, 2, 1, 2, 4, 0, 3, 1, 
	1, 1, 3, 2, 2, 2, 2, 1, 
	2, 4, 0, 3, 1, 1, 1, 3, 
	1
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
	0, 0, 0, 0, 0, 0, 0, 0, 
	1, 2, 1, 2, 0, 0, 0, 1, 
	0, 0, 0, 0, 0, 0, 1, 1, 
	0, 0, 0, 0, 0, 0, 1, 0, 
	0, 0, 0, 0, 1, 0, 0, 0, 
	1, 0, 0, 0, 1, 2, 1, 2, 
	0, 0, 0, 1, 0, 0, 0, 0, 
	0, 0, 0, 1, 2, 1, 2, 0, 
	0, 0, 1, 0, 0, 0, 1, 1, 
	0, 1, 0, 0, 0, 0, 1, 0, 
	0, 0, 1, 1, 0, 1, 0, 0, 
	0, 0, 1, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 1, 2, 1, 2, 
	0, 0, 0, 1, 0, 0, 0, 1, 
	1, 0, 0, 0, 0, 1, 0, 0, 
	0, 0, 1, 0, 0, 0, 1, 0, 
	0, 0, 0, 1, 2, 1, 2, 0, 
	0, 0, 1, 0, 0, 0, 0, 0, 
	1, 2, 1, 2, 0, 0, 0, 1, 
	1
};

static const short _json_index_offsets[] = {
	0, 0, 4, 9, 28, 31, 34, 37, 
	40, 42, 46, 51, 54, 59, 64, 68, 
	69, 71, 73, 75, 80, 85, 88, 91, 
	94, 96, 100, 105, 108, 113, 114, 116, 
	118, 120, 125, 128, 131, 133, 137, 142, 
	145, 150, 151, 153, 155, 157, 162, 165, 
	168, 171, 174, 177, 180, 182, 186, 191, 
	194, 199, 200, 202, 204, 206, 211, 214, 
	217, 220, 223, 226, 229, 232, 235, 238, 
	240, 244, 251, 253, 259, 261, 263, 265, 
	270, 275, 278, 281, 284, 287, 289, 293, 
	298, 303, 306, 309, 312, 315, 317, 322, 
	325, 328, 331, 334, 336, 341, 343, 345, 
	347, 352, 355, 358, 360, 364, 371, 373, 
	379, 381, 383, 385, 390, 395, 398, 401, 
	404, 407, 410, 412, 416, 423, 425, 431, 
	433, 435, 437, 442, 445, 448, 450, 454, 
	459, 462, 467, 468, 470, 472, 474, 479, 
	482, 485, 487, 491, 496, 499, 504, 505, 
	507, 509, 511, 516, 523, 526, 529, 532, 
	535, 538, 541, 544, 546, 550, 557, 559, 
	565, 567, 569, 571, 576, 579, 582, 584, 
	588, 593, 598, 601, 604, 606, 611, 614, 
	617, 620, 622, 627, 629, 631, 633, 638, 
	641, 644, 647, 649, 653, 660, 662, 668, 
	670, 672, 674, 679, 682, 685, 688, 691, 
	693, 697, 704, 706, 712, 714, 716, 718, 
	723
};

static const unsigned char _json_trans_targs[] = {
	1, 2, 1, 0, 2, 3, 216, 2, 
	0, 4, 20, 46, 62, 80, 116, 143, 
	155, 203, 4, 20, 46, 62, 80, 116, 
	143, 155, 203, 0, 5, 5, 0, 6, 
	6, 0, 7, 7, 0, 8, 8, 0, 
	9, 0, 9, 10, 9, 0, 10, 11, 
	16, 10, 0, 12, 15, 11, 13, 14, 
	216, 13, 0, 13, 14, 216, 13, 0, 
	14, 3, 14, 0, 11, 17, 0, 18, 
	0, 19, 0, 13, 14, 216, 13, 0, 
	21, 34, 21, 34, 0, 22, 22, 0, 
	23, 23, 0, 24, 24, 0, 25, 0, 
	25, 26, 25, 0, 26, 27, 30, 26, 
	0, 28, 29, 27, 13, 14, 216, 13, 
	0, 27, 31, 0, 32, 0, 33, 0, 
	13, 14, 216, 13, 0, 35, 35, 0, 
	36, 36, 0, 37, 0, 37, 38, 37, 
	0, 38, 39, 42, 38, 0, 40, 41, 
	39, 13, 14, 216, 13, 0, 39, 43, 
	0, 44, 0, 45, 0, 13, 14, 216, 
	13, 0, 47, 47, 0, 48, 48, 0, 
	49, 49, 0, 50, 50, 0, 51, 51, 
	0, 52, 52, 0, 53, 0, 53, 54, 
	53, 0, 54, 55, 58, 54, 0, 56, 
	57, 55, 13, 14, 216, 13, 0, 55, 
	59, 0, 60, 0, 61, 0, 13, 14, 
	216, 13, 0, 63, 63, 0, 64, 64, 
	0, 65, 65, 0, 66, 66, 0, 67, 
	67, 0, 68, 68, 0, 69, 69, 0, 
	70, 70, 0, 71, 71, 0, 72, 0, 
	72, 73, 72, 0, 73, 74, 74, 76, 
	73, 75, 0, 75, 0, 13, 14, 216, 
	13, 75, 0, 77, 0, 78, 0, 79, 
	0, 13, 14, 216, 13, 0, 81, 105, 
	81, 105, 0, 82, 82, 0, 83, 83, 
	0, 84, 84, 0, 85, 85, 0, 86, 
	0, 86, 87, 86, 0, 87, 88, 101, 
	87, 0, 89, 95, 89, 95, 0, 90, 
	90, 0, 91, 91, 0, 92, 92, 0, 
	93, 93, 0, 94, 0, 13, 14, 216, 
	13, 0, 96, 96, 0, 97, 97, 0, 
	98, 98, 0, 99, 99, 0, 100, 0, 
	13, 14, 216, 13, 0, 102, 0, 103, 
	0, 104, 0, 13, 14, 216, 13, 0, 
	106, 106, 0, 107, 107, 0, 108, 0, 
	108, 109, 108, 0, 109, 110, 110, 112, 
	109, 111, 0, 111, 0, 13, 14, 216, 
	13, 111, 0, 113, 0, 114, 0, 115, 
	0, 13, 14, 216, 13, 0, 117, 131, 
	117, 131, 0, 118, 118, 0, 119, 119, 
	0, 120, 120, 0, 121, 121, 0, 122, 
	122, 0, 123, 0, 123, 124, 123, 0, 
	124, 125, 125, 127, 124, 126, 0, 126, 
	0, 13, 14, 216, 13, 126, 0, 128, 
	0, 129, 0, 130, 0, 13, 14, 216, 
	13, 0, 132, 132, 0, 133, 133, 0, 
	134, 0, 134, 135, 134, 0, 135, 136, 
	139, 135, 0, 137, 138, 136, 13, 14, 
	216, 13, 0, 136, 140, 0, 141, 0, 
	142, 0, 13, 14, 216, 13, 0, 144, 
	144, 0, 145, 145, 0, 146, 0, 146, 
	147, 146, 0, 147, 148, 151, 147, 0, 
	149, 150, 148, 13, 14, 216, 13, 0, 
	148, 152, 0, 153, 0, 154, 0, 13, 
	14, 216, 13, 0, 156, 172, 191, 156, 
	172, 191, 0, 157, 157, 0, 158, 158, 
	0, 159, 159, 0, 160, 160, 0, 161, 
	161, 0, 162, 162, 0, 163, 163, 0, 
	164, 0, 164, 165, 164, 0, 165, 166, 
	166, 168, 165, 167, 0, 167, 0, 13, 
	14, 216, 13, 167, 0, 169, 0, 170, 
	0, 171, 0, 13, 14, 216, 13, 0, 
	173, 173, 0, 174, 174, 0, 175, 0, 
	175, 176, 175, 0, 176, 177, 187, 176, 
	0, 178, 182, 178, 182, 0, 179, 179, 
	0, 180, 180, 0, 181, 0, 13, 14, 
	216, 13, 0, 183, 183, 0, 184, 184, 
	0, 185, 185, 0, 186, 0, 13, 14, 
	216, 13, 0, 188, 0, 189, 0, 190, 
	0, 13, 14, 216, 13, 0, 192, 192, 
	0, 193, 193, 0, 194, 194, 0, 195, 
	0, 195, 196, 195, 0, 196, 197, 197, 
	199, 196, 198, 0, 198, 0, 13, 14, 
	216, 13, 198, 0, 200, 0, 201, 0, 
	202, 0, 13, 14, 216, 13, 0, 204, 
	204, 0, 205, 205, 0, 206, 206, 0, 
	207, 207, 0, 208, 0, 208, 209, 208, 
	0, 209, 210, 210, 212, 209, 211, 0, 
	211, 0, 13, 14, 216, 13, 211, 0, 
	213, 0, 214, 0, 215, 0, 13, 14, 
	216, 13, 0, 216, 216, 0, 0
};

static const char _json_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 79, 
	0, 0, 0, 0, 0, 5, 17, 17, 
	17, 17, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 5, 0, 0, 0, 
	0, 0, 0, 15, 15, 15, 15, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 82, 0, 0, 
	0, 0, 0, 5, 21, 21, 21, 21, 
	0, 5, 0, 0, 0, 0, 0, 0, 
	19, 19, 19, 19, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 88, 0, 0, 0, 0, 0, 
	5, 65, 65, 65, 65, 0, 5, 0, 
	0, 0, 0, 0, 0, 63, 63, 63, 
	63, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 76, 0, 0, 0, 0, 
	0, 5, 13, 13, 13, 13, 0, 5, 
	0, 0, 0, 0, 0, 0, 11, 11, 
	11, 11, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 1, 67, 0, 
	0, 70, 0, 3, 0, 29, 29, 29, 
	29, 3, 0, 0, 0, 0, 0, 0, 
	0, 27, 27, 27, 27, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 59, 59, 59, 
	59, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	61, 61, 61, 61, 0, 0, 0, 0, 
	0, 0, 0, 57, 57, 57, 57, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 1, 67, 0, 
	0, 70, 0, 3, 0, 51, 51, 51, 
	51, 3, 0, 0, 0, 0, 0, 0, 
	0, 49, 49, 49, 49, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 67, 0, 0, 70, 0, 3, 
	0, 55, 55, 55, 55, 3, 0, 0, 
	0, 0, 0, 0, 0, 53, 53, 53, 
	53, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 73, 
	0, 0, 0, 0, 0, 5, 9, 9, 
	9, 9, 0, 5, 0, 0, 0, 0, 
	0, 0, 7, 7, 7, 7, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 85, 0, 0, 0, 
	0, 0, 5, 33, 33, 33, 33, 0, 
	5, 0, 0, 0, 0, 0, 0, 31, 
	31, 31, 31, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 1, 
	67, 0, 0, 70, 0, 3, 0, 25, 
	25, 25, 25, 3, 0, 0, 0, 0, 
	0, 0, 0, 23, 23, 23, 23, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 45, 45, 
	45, 45, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 47, 47, 
	47, 47, 0, 0, 0, 0, 0, 0, 
	0, 43, 43, 43, 43, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 1, 67, 
	0, 0, 70, 0, 3, 0, 37, 37, 
	37, 37, 3, 0, 0, 0, 0, 0, 
	0, 0, 35, 35, 35, 35, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 67, 0, 0, 70, 0, 
	3, 0, 41, 41, 41, 41, 3, 0, 
	0, 0, 0, 0, 0, 0, 39, 39, 
	39, 39, 0, 0, 0, 0, 0
};

static const int json_start = 1;
static const int json_first_final = 216;
static const int json_error = 0;

static const int json_en_main = 1;


#line 299 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"

} // anonymous

RestRequest::~RestRequest() noexcept = default;

void RestRequest::reset(bool clear) noexcept
{
  decltype(cs_) cs;
  
#line 458 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.cpp"
	{
	cs = json_start;
	}

#line 308 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
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
  expiryDate_ = 0_ymd;
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
  
#line 495 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.cpp"
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
    fields_ &= ~ExpiryDate;
    expiryDate_ = 0_ymd;
  }
	break;
	case 20:
#line 155 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= ExpiryDate;
    expiryDate_ = box<IsoDate>(num());
  }
	break;
	case 21:
#line 162 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Ref;
    ref_.len = 0;
  }
	break;
	case 22:
#line 166 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    str_.len = &ref_.len;
    str_.buf = ref_.buf;
    str_.max = MaxRef;
  }
	break;
	case 23:
#line 171 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Ref;
  }
	break;
	case 24:
#line 177 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~State;
    state_ = 0;
  }
	break;
	case 25:
#line 181 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    if (num_.sign >= 0) {
      fields_ |= State;
      state_ = num();
    } else {
      cs = json_error; msg = "negative state field";
    }
  }
	break;
	case 26:
#line 192 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Ticks;
    ticks_ = 0_tks;
  }
	break;
	case 27:
#line 196 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Ticks;
    ticks_ = box<swirly::Ticks>(num());
  }
	break;
	case 28:
#line 203 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Side;
    side_ = box<swirly::Side>(0);
  }
	break;
	case 29:
#line 207 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Side;
    side_ = swirly::Side::Buy;
  }
	break;
	case 30:
#line 211 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Side;
    side_ = swirly::Side::Sell;
  }
	break;
	case 31:
#line 219 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Lots;
    lots_ = 0_lts;
  }
	break;
	case 32:
#line 223 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Lots;
    lots_ = box<swirly::Lots>(num());
  }
	break;
	case 33:
#line 230 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~MinLots;
    minLots_ = 0_lts;
  }
	break;
	case 34:
#line 234 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= MinLots;
    minLots_ = box<swirly::Lots>(num());
  }
	break;
	case 35:
#line 241 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~LiqInd;
    liqInd_ = swirly::LiqInd::None;
  }
	break;
	case 36:
#line 245 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= LiqInd;
    liqInd_ = swirly::LiqInd::Maker;
  }
	break;
	case 37:
#line 249 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= LiqInd;
    liqInd_ = swirly::LiqInd::Taker;
  }
	break;
	case 38:
#line 257 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ &= ~Cpty;
    cpty_.len = 0;
  }
	break;
	case 39:
#line 261 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    str_.len = &cpty_.len;
    str_.buf = cpty_.buf;
    str_.max = MaxMnem;
  }
	break;
	case 40:
#line 266 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
	{
    fields_ |= Cpty;
  }
	break;
#line 861 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.cpp"
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

#line 339 "/home/marayl/repo/swirly/src/lib/fir/RestRequest.rl"
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

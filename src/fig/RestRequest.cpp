
#line 1 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
// -*- C++ -*-
/*
 * Swirly Order-Book and Matching-Engine.
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
#include "RestRequest.hpp"

#include "Exception.hpp"

namespace swirly {

namespace {

#line 310 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"

#line 34 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.cpp"
static const char _json_actions[]
    = {0,  1, 0,  1, 2,  1, 4,  1, 5,  1,  7,  1, 8,  1, 10, 1,  11, 1, 13, 1, 14, 1, 16, 1, 17, 1,
       19, 1, 20, 1, 21, 1, 22, 1, 23, 1,  24, 1, 26, 1, 27, 1,  28, 1, 29, 1, 30, 1, 31, 1, 32, 1,
       33, 1, 34, 1, 35, 1, 36, 1, 37, 1,  38, 1, 39, 1, 40, 1,  41, 1, 43, 2, 0,  1, 2,  0, 2,  2,
       6,  3, 2,  9, 3,  2, 12, 3, 2,  15, 3,  2, 18, 3, 2,  25, 3,  2, 42, 3};

static const short _json_key_offsets[] = {
    0,   0,   4,   9,   25,  29,  31,  33,  35,  36,  40,  45,  47,  52,  57,  61,  61,  62,  63,
    64,  69,  71,  73,  74,  78,  83,  85,  90,  90,  91,  92,  93,  98,  100, 102, 104, 106, 108,
    110, 111, 115, 120, 122, 127, 127, 128, 129, 130, 135, 139, 141, 143, 145, 146, 150, 155, 157,
    162, 162, 163, 164, 165, 170, 172, 174, 176, 178, 180, 182, 184, 186, 187, 191, 199, 201, 208,
    209, 210, 211, 216, 218, 220, 222, 223, 227, 235, 237, 244, 245, 246, 247, 252, 256, 258, 260,
    262, 264, 266, 267, 271, 279, 281, 288, 289, 290, 291, 296, 298, 300, 301, 305, 310, 312, 317,
    317, 318, 319, 320, 325, 329, 331, 332, 336, 341, 343, 348, 348, 349, 350, 351, 356, 358, 360,
    361, 365, 370, 374, 376, 378, 380, 382, 383, 388, 390, 392, 394, 396, 397, 402, 403, 404, 405,
    410, 416, 418, 420, 422, 424, 426, 428, 430, 431, 435, 443, 445, 452, 453, 454, 455, 460, 462,
    464, 465, 469, 474, 478, 480, 482, 483, 488, 490, 492, 494, 495, 500, 501, 502, 503, 508, 510,
    512, 514, 515, 519, 527, 529, 536, 537, 538, 539, 544, 548, 550, 552, 554, 555, 559, 567, 569,
    576, 577, 578, 579, 584, 586, 588, 590, 592, 593, 597, 602, 604, 609, 609, 610, 611, 612, 617};

static const char _json_trans_keys[] = {
    32,  123, 9,   13,  32,  34,  125, 9,   13,  67,  68,  69,  76,  77,  82,  83,  84,  99,  100,
    101, 108, 109, 114, 115, 116, 79,  80,  111, 112, 78,  110, 84,  116, 82,  114, 34,  32,  58,
    9,   13,  32,  34,  110, 9,   13,  34,  92,  32,  44,  125, 9,   13,  32,  44,  125, 9,   13,
    32,  34,  9,   13,  117, 108, 108, 32,  44,  125, 9,   13,  84,  116, 89,  121, 34,  32,  58,
    9,   13,  32,  34,  110, 9,   13,  34,  92,  32,  44,  125, 9,   13,  117, 108, 108, 32,  44,
    125, 9,   13,  73,  105, 83,  115, 80,  112, 76,  108, 65,  97,  89,  121, 34,  32,  58,  9,
    13,  32,  34,  110, 9,   13,  34,  92,  32,  44,  125, 9,   13,  117, 108, 108, 32,  44,  125,
    9,   13,  77,  88,  109, 120, 65,  97,  73,  105, 76,  108, 34,  32,  58,  9,   13,  32,  34,
    110, 9,   13,  34,  92,  32,  44,  125, 9,   13,  117, 108, 108, 32,  44,  125, 9,   13,  80,
    112, 73,  105, 82,  114, 89,  121, 68,  100, 65,  97,  84,  116, 69,  101, 34,  32,  58,  9,
    13,  32,  43,  45,  110, 9,   13,  48,  57,  48,  57,  32,  44,  125, 9,   13,  48,  57,  117,
    108, 108, 32,  44,  125, 9,   13,  79,  111, 84,  116, 83,  115, 34,  32,  58,  9,   13,  32,
    43,  45,  110, 9,   13,  48,  57,  48,  57,  32,  44,  125, 9,   13,  48,  57,  117, 108, 108,
    32,  44,  125, 9,   13,  73,  78,  105, 110, 78,  110, 76,  108, 79,  111, 84,  116, 83,  115,
    34,  32,  58,  9,   13,  32,  43,  45,  110, 9,   13,  48,  57,  48,  57,  32,  44,  125, 9,
    13,  48,  57,  117, 108, 108, 32,  44,  125, 9,   13,  69,  101, 77,  109, 34,  32,  58,  9,
    13,  32,  34,  110, 9,   13,  34,  92,  32,  44,  125, 9,   13,  117, 108, 108, 32,  44,  125,
    9,   13,  69,  79,  101, 111, 70,  102, 34,  32,  58,  9,   13,  32,  34,  110, 9,   13,  34,
    92,  32,  44,  125, 9,   13,  117, 108, 108, 32,  44,  125, 9,   13,  76,  108, 69,  101, 34,
    32,  58,  9,   13,  32,  34,  110, 9,   13,  77,  84,  109, 116, 65,  97,  75,  107, 69,  101,
    82,  114, 34,  32,  44,  125, 9,   13,  65,  97,  75,  107, 69,  101, 82,  114, 34,  32,  44,
    125, 9,   13,  117, 108, 108, 32,  44,  125, 9,   13,  69,  73,  84,  101, 105, 116, 84,  116,
    84,  116, 76,  108, 68,  100, 65,  97,  84,  116, 69,  101, 34,  32,  58,  9,   13,  32,  43,
    45,  110, 9,   13,  48,  57,  48,  57,  32,  44,  125, 9,   13,  48,  57,  117, 108, 108, 32,
    44,  125, 9,   13,  68,  100, 69,  101, 34,  32,  58,  9,   13,  32,  34,  110, 9,   13,  66,
    83,  98,  115, 85,  117, 89,  121, 34,  32,  44,  125, 9,   13,  69,  101, 76,  108, 76,  108,
    34,  32,  44,  125, 9,   13,  117, 108, 108, 32,  44,  125, 9,   13,  65,  97,  84,  116, 69,
    101, 34,  32,  58,  9,   13,  32,  43,  45,  110, 9,   13,  48,  57,  48,  57,  32,  44,  125,
    9,   13,  48,  57,  117, 108, 108, 32,  44,  125, 9,   13,  73,  82,  105, 114, 67,  99,  75,
    107, 83,  115, 34,  32,  58,  9,   13,  32,  43,  45,  110, 9,   13,  48,  57,  48,  57,  32,
    44,  125, 9,   13,  48,  57,  117, 108, 108, 32,  44,  125, 9,   13,  65,  97,  68,  100, 69,
    101, 82,  114, 34,  32,  58,  9,   13,  32,  34,  110, 9,   13,  34,  92,  32,  44,  125, 9,
    13,  117, 108, 108, 32,  44,  125, 9,   13,  32,  9,   13,  0};

static const char _json_single_lengths[]
    = {0, 2, 3, 16, 4, 2, 2, 2, 1, 2, 3, 2, 3, 3, 2, 0, 1, 1, 1, 3, 2, 2, 1, 2, 3, 2, 3, 0, 1,
       1, 1, 3, 2,  2, 2, 2, 2, 2, 1, 2, 3, 2, 3, 0, 1, 1, 1, 3, 4, 2, 2, 2, 1, 2, 3, 2, 3, 0,
       1, 1, 1, 3,  2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 4, 0, 3, 1, 1, 1, 3, 2, 2, 2, 1, 2, 4, 0, 3,
       1, 1, 1, 3,  4, 2, 2, 2, 2, 2, 1, 2, 4, 0, 3, 1, 1, 1, 3, 2, 2, 1, 2, 3, 2, 3, 0, 1, 1,
       1, 3, 4, 2,  1, 2, 3, 2, 3, 0, 1, 1, 1, 3, 2, 2, 1, 2, 3, 4, 2, 2, 2, 2, 1, 3, 2, 2, 2,
       2, 1, 3, 1,  1, 1, 3, 6, 2, 2, 2, 2, 2, 2, 2, 1, 2, 4, 0, 3, 1, 1, 1, 3, 2, 2, 1, 2, 3,
       4, 2, 2, 1,  3, 2, 2, 2, 1, 3, 1, 1, 1, 3, 2, 2, 2, 1, 2, 4, 0, 3, 1, 1, 1, 3, 4, 2, 2,
       2, 1, 2, 4,  0, 3, 1, 1, 1, 3, 2, 2, 2, 2, 1, 2, 3, 2, 3, 0, 1, 1, 1, 3, 1};

static const char _json_range_lengths[]
    = {0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0,
       0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0,
       0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 2, 0, 0, 0, 1, 0, 0, 0, 0, 1, 2, 1, 2,
       0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 2, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0,
       0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
       0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 2, 0, 0, 0, 1, 0, 0, 0, 1, 1,
       0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 2, 1, 2, 0, 0, 0, 1, 0, 0, 0,
       0, 0, 1, 2, 1, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1};

static const short _json_index_offsets[] = {
    0,   0,   4,   9,   26,  31,  34,  37,  40,  42,  46,  51,  54,  59,  64,  68,  69,  71,  73,
    75,  80,  83,  86,  88,  92,  97,  100, 105, 106, 108, 110, 112, 117, 120, 123, 126, 129, 132,
    135, 137, 141, 146, 149, 154, 155, 157, 159, 161, 166, 171, 174, 177, 180, 182, 186, 191, 194,
    199, 200, 202, 204, 206, 211, 214, 217, 220, 223, 226, 229, 232, 235, 237, 241, 248, 250, 256,
    258, 260, 262, 267, 270, 273, 276, 278, 282, 289, 291, 297, 299, 301, 303, 308, 313, 316, 319,
    322, 325, 328, 330, 334, 341, 343, 349, 351, 353, 355, 360, 363, 366, 368, 372, 377, 380, 385,
    386, 388, 390, 392, 397, 402, 405, 407, 411, 416, 419, 424, 425, 427, 429, 431, 436, 439, 442,
    444, 448, 453, 458, 461, 464, 467, 470, 472, 477, 480, 483, 486, 489, 491, 496, 498, 500, 502,
    507, 514, 517, 520, 523, 526, 529, 532, 535, 537, 541, 548, 550, 556, 558, 560, 562, 567, 570,
    573, 575, 579, 584, 589, 592, 595, 597, 602, 605, 608, 611, 613, 618, 620, 622, 624, 629, 632,
    635, 638, 640, 644, 651, 653, 659, 661, 663, 665, 670, 675, 678, 681, 684, 686, 690, 697, 699,
    705, 707, 709, 711, 716, 719, 722, 725, 728, 730, 734, 739, 742, 747, 748, 750, 752, 754, 759};

static const unsigned char _json_trans_targs[] = {
    1,   2,   1,   0,   2,   3,   227, 2,   0,   4,   32,  48,  79,  91,  118, 152, 200, 4,   32,
    48,  79,  91,  118, 152, 200, 0,   5,   20,  5,   20,  0,   6,   6,   0,   7,   7,   0,   8,
    8,   0,   9,   0,   9,   10,  9,   0,   10,  11,  16,  10,  0,   12,  15,  11,  13,  14,  227,
    13,  0,   13,  14,  227, 13,  0,   14,  3,   14,  0,   11,  17,  0,   18,  0,   19,  0,   13,
    14,  227, 13,  0,   21,  21,  0,   22,  22,  0,   23,  0,   23,  24,  23,  0,   24,  25,  28,
    24,  0,   26,  27,  25,  13,  14,  227, 13,  0,   25,  29,  0,   30,  0,   31,  0,   13,  14,
    227, 13,  0,   33,  33,  0,   34,  34,  0,   35,  35,  0,   36,  36,  0,   37,  37,  0,   38,
    38,  0,   39,  0,   39,  40,  39,  0,   40,  41,  44,  40,  0,   42,  43,  41,  13,  14,  227,
    13,  0,   41,  45,  0,   46,  0,   47,  0,   13,  14,  227, 13,  0,   49,  62,  49,  62,  0,
    50,  50,  0,   51,  51,  0,   52,  52,  0,   53,  0,   53,  54,  53,  0,   54,  55,  58,  54,
    0,   56,  57,  55,  13,  14,  227, 13,  0,   55,  59,  0,   60,  0,   61,  0,   13,  14,  227,
    13,  0,   63,  63,  0,   64,  64,  0,   65,  65,  0,   66,  66,  0,   67,  67,  0,   68,  68,
    0,   69,  69,  0,   70,  70,  0,   71,  0,   71,  72,  71,  0,   72,  73,  73,  75,  72,  74,
    0,   74,  0,   13,  14,  227, 13,  74,  0,   76,  0,   77,  0,   78,  0,   13,  14,  227, 13,
    0,   80,  80,  0,   81,  81,  0,   82,  82,  0,   83,  0,   83,  84,  83,  0,   84,  85,  85,
    87,  84,  86,  0,   86,  0,   13,  14,  227, 13,  86,  0,   88,  0,   89,  0,   90,  0,   13,
    14,  227, 13,  0,   92,  106, 92,  106, 0,   93,  93,  0,   94,  94,  0,   95,  95,  0,   96,
    96,  0,   97,  97,  0,   98,  0,   98,  99,  98,  0,   99,  100, 100, 102, 99,  101, 0,   101,
    0,   13,  14,  227, 13,  101, 0,   103, 0,   104, 0,   105, 0,   13,  14,  227, 13,  0,   107,
    107, 0,   108, 108, 0,   109, 0,   109, 110, 109, 0,   110, 111, 114, 110, 0,   112, 113, 111,
    13,  14,  227, 13,  0,   111, 115, 0,   116, 0,   117, 0,   13,  14,  227, 13,  0,   119, 130,
    119, 130, 0,   120, 120, 0,   121, 0,   121, 122, 121, 0,   122, 123, 126, 122, 0,   124, 125,
    123, 13,  14,  227, 13,  0,   123, 127, 0,   128, 0,   129, 0,   13,  14,  227, 13,  0,   131,
    131, 0,   132, 132, 0,   133, 0,   133, 134, 133, 0,   134, 135, 148, 134, 0,   136, 142, 136,
    142, 0,   137, 137, 0,   138, 138, 0,   139, 139, 0,   140, 140, 0,   141, 0,   13,  14,  227,
    13,  0,   143, 143, 0,   144, 144, 0,   145, 145, 0,   146, 146, 0,   147, 0,   13,  14,  227,
    13,  0,   149, 0,   150, 0,   151, 0,   13,  14,  227, 13,  0,   153, 169, 188, 153, 169, 188,
    0,   154, 154, 0,   155, 155, 0,   156, 156, 0,   157, 157, 0,   158, 158, 0,   159, 159, 0,
    160, 160, 0,   161, 0,   161, 162, 161, 0,   162, 163, 163, 165, 162, 164, 0,   164, 0,   13,
    14,  227, 13,  164, 0,   166, 0,   167, 0,   168, 0,   13,  14,  227, 13,  0,   170, 170, 0,
    171, 171, 0,   172, 0,   172, 173, 172, 0,   173, 174, 184, 173, 0,   175, 179, 175, 179, 0,
    176, 176, 0,   177, 177, 0,   178, 0,   13,  14,  227, 13,  0,   180, 180, 0,   181, 181, 0,
    182, 182, 0,   183, 0,   13,  14,  227, 13,  0,   185, 0,   186, 0,   187, 0,   13,  14,  227,
    13,  0,   189, 189, 0,   190, 190, 0,   191, 191, 0,   192, 0,   192, 193, 192, 0,   193, 194,
    194, 196, 193, 195, 0,   195, 0,   13,  14,  227, 13,  195, 0,   197, 0,   198, 0,   199, 0,
    13,  14,  227, 13,  0,   201, 213, 201, 213, 0,   202, 202, 0,   203, 203, 0,   204, 204, 0,
    205, 0,   205, 206, 205, 0,   206, 207, 207, 209, 206, 208, 0,   208, 0,   13,  14,  227, 13,
    208, 0,   210, 0,   211, 0,   212, 0,   13,  14,  227, 13,  0,   214, 214, 0,   215, 215, 0,
    216, 216, 0,   217, 217, 0,   218, 0,   218, 219, 218, 0,   219, 220, 223, 219, 0,   221, 222,
    220, 13,  14,  227, 13,  0,   220, 224, 0,   225, 0,   226, 0,   13,  14,  227, 13,  0,   227,
    227, 0,   0};

static const char _json_trans_actions[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  89,
    0,  0,  0,  0,  0,  5,  25, 25, 25, 25, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  0,  0,  0,
    0,  0,  0,  23, 23, 23, 23, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  95, 0,  0,
    0,  0,  0,  5,  69, 69, 69, 69, 0,  5,  0,  0,  0,  0,  0,  0,  67, 67, 67, 67, 0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  80, 0,
    0,  0,  0,  0,  5,  13, 13, 13, 13, 0,  5,  0,  0,  0,  0,  0,  0,  11, 11, 11, 11, 0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  83, 0,  0,  0,  0,
    0,  5,  17, 17, 17, 17, 0,  5,  0,  0,  0,  0,  0,  0,  15, 15, 15, 15, 0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  1,  71, 0,  0,  74, 0,  3,  0,  33, 33, 33, 33, 3,  0,  0,  0,  0,  0,  0,  0,  31, 31,
    31, 31, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  71, 0,  0,  74,
    0,  3,  0,  55, 55, 55, 55, 3,  0,  0,  0,  0,  0,  0,  0,  53, 53, 53, 53, 0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,
    71, 0,  0,  74, 0,  3,  0,  59, 59, 59, 59, 3,  0,  0,  0,  0,  0,  0,  0,  57, 57, 57, 57, 0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  77, 0,  0,  0,  0,  0,  5,  9,  9,  9,  9,
    0,  5,  0,  0,  0,  0,  0,  0,  7,  7,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  92, 0,  0,  0,  0,  0,  5,  37, 37, 37, 37, 0,  5,  0,  0,  0,  0,  0,  0,  35,
    35, 35, 35, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  63, 63, 63, 63, 0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  65, 65, 65, 65, 0,  0,  0,  0,  0,  0,  0,  61, 61,
    61, 61, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  71, 0,  0,  74, 0,  3,  0,  29, 29,
    29, 29, 3,  0,  0,  0,  0,  0,  0,  0,  27, 27, 27, 27, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  49, 49, 49,
    49, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  51, 51, 51, 51, 0,  0,  0,  0,  0,  0,  0,
    47, 47, 47, 47, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  71, 0,
    0,  74, 0,  3,  0,  41, 41, 41, 41, 3,  0,  0,  0,  0,  0,  0,  0,  39, 39, 39, 39, 0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  71, 0,  0,  74,
    0,  3,  0,  45, 45, 45, 45, 3,  0,  0,  0,  0,  0,  0,  0,  43, 43, 43, 43, 0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  86, 0,  0,  0,  0,  0,  5,  21, 21,
    21, 21, 0,  5,  0,  0,  0,  0,  0,  0,  19, 19, 19, 19, 0,  0,  0,  0,  0};

static const int json_start = 1;
static const int json_first_final = 227;
static const int json_error = 0;

static const int json_en_main = 1;

#line 313 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"

} // anonymous

RestRequest::~RestRequest() noexcept = default;

void RestRequest::reset(bool clear) noexcept
{
    decltype(cs_) cs;

#line 475 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.cpp"
    {
        cs = json_start;
    }

#line 322 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
    cs_ = cs;

    if (!clear)
        return;

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
    side_ = box<Side>(0);
    lots_ = 0_lts;
    ticks_ = 0_tks;
    minLots_ = 0_lts;
    role_ = Role::NONE;
    cpty_.len = 0;
}

bool RestRequest::parse(const StringView& buf)
{
    const char* p{buf.data()};
    const char* pe{p + buf.size()};
    const char* msg{"parse error"};

    auto cs = cs_;

#line 513 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.cpp"
    {
        int _klen;
        unsigned int _trans;
        const char* _acts;
        unsigned int _nacts;
        const char* _keys;

        if (p == pe)
            goto _test_eof;
        if (cs == 0)
            goto _out;
    _resume:
        _keys = _json_trans_keys + _json_key_offsets[cs];
        _trans = _json_index_offsets[cs];

        _klen = _json_single_lengths[cs];
        if (_klen > 0) {
            const char* _lower = _keys;
            const char* _mid;
            const char* _upper = _keys + _klen - 1;
            while (1) {
                if (_upper < _lower)
                    break;

                _mid = _lower + ((_upper - _lower) >> 1);
                if ((*p) < *_mid)
                    _upper = _mid - 1;
                else if ((*p) > *_mid)
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
        if (_klen > 0) {
            const char* _lower = _keys;
            const char* _mid;
            const char* _upper = _keys + (_klen << 1) - 2;
            while (1) {
                if (_upper < _lower)
                    break;

                _mid = _lower + (((_upper - _lower) >> 1) & ~1);
                if ((*p) < _mid[0])
                    _upper = _mid - 2;
                else if ((*p) > _mid[1])
                    _lower = _mid + 2;
                else {
                    _trans += (unsigned int)((_mid - _keys) >> 1);
                    goto _match;
                }
            }
            _trans += _klen;
        }

    _match:
        cs = _json_trans_targs[_trans];

        if (_json_trans_actions[_trans] == 0)
            goto _again;

        _acts = _json_actions + _json_trans_actions[_trans];
        _nacts = (unsigned int)*_acts++;
        while (_nacts-- > 0) {
            switch (*_acts++) {
            case 0:
#line 29 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                num_.sign = 1;
                num_.digits = 0;
            } break;
            case 1:
#line 33 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                num_.sign = -1;
            } break;
            case 2:
#line 36 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                num_.digits = num_.digits * 10 + ((*p) - '0');
            } break;
            case 3:
#line 55 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                *str_.len = 0;
            } break;
            case 4:
#line 58 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                auto& len = *str_.len;
                if (len < str_.max)
                    str_.buf[len++] = (*p);
                else {
                    cs = json_error;
                    msg = "max length exceeded";
                    {
                        p++;
                        goto _out;
                    }
                }
            } break;
            case 5:
#line 78 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ &= ~RestRequest::MNEM;
                mnem_.len = 0;
            } break;
            case 6:
#line 82 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                str_.len = &mnem_.len;
                str_.buf = mnem_.buf;
                str_.max = MNEM_MAX;
            } break;
            case 7:
#line 87 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::MNEM;
            } break;
            case 8:
#line 93 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ &= ~RestRequest::DISPLAY;
                display_.len = 0;
            } break;
            case 9:
#line 97 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                str_.len = &display_.len;
                str_.buf = display_.buf;
                str_.max = DISPLAY_MAX;
            } break;
            case 10:
#line 102 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::DISPLAY;
            } break;
            case 11:
#line 108 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ &= ~RestRequest::EMAIL;
                email_.len = 0;
            } break;
            case 12:
#line 112 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                str_.len = &email_.len;
                str_.buf = email_.buf;
                str_.max = EMAIL_MAX;
            } break;
            case 13:
#line 117 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::EMAIL;
            } break;
            case 14:
#line 123 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ &= ~RestRequest::TRADER;
                trader_.len = 0;
            } break;
            case 15:
#line 127 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                str_.len = &trader_.len;
                str_.buf = trader_.buf;
                str_.max = MNEM_MAX;
            } break;
            case 16:
#line 132 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::TRADER;
            } break;
            case 17:
#line 138 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ &= ~RestRequest::CONTR;
                contr_.len = 0;
            } break;
            case 18:
#line 142 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                str_.len = &contr_.len;
                str_.buf = contr_.buf;
                str_.max = MNEM_MAX;
            } break;
            case 19:
#line 147 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::CONTR;
            } break;
            case 20:
#line 153 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ &= ~RestRequest::SETTL_DATE;
                settlDate_ = 0_dt;
            } break;
            case 21:
#line 157 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::SETTL_DATE;
                settlDate_ = box<IsoDate>(num());
            } break;
            case 22:
#line 164 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ &= ~RestRequest::EXPIRY_DATE;
                expiryDate_ = 0_dt;
            } break;
            case 23:
#line 168 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::EXPIRY_DATE;
                expiryDate_ = box<IsoDate>(num());
            } break;
            case 24:
#line 175 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ &= ~RestRequest::REF;
                ref_.len = 0;
            } break;
            case 25:
#line 179 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                str_.len = &ref_.len;
                str_.buf = ref_.buf;
                str_.max = REF_MAX;
            } break;
            case 26:
#line 184 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::REF;
            } break;
            case 27:
#line 190 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ &= ~RestRequest::STATE;
                state_ = 0;
            } break;
            case 28:
#line 194 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                if (num_.sign >= 0) {
                    fields_ |= RestRequest::STATE;
                    state_ = num();
                } else {
                    cs = json_error;
                    msg = "negative state field";
                }
            } break;
            case 29:
#line 205 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ &= ~RestRequest::TICKS;
                ticks_ = 0_tks;
            } break;
            case 30:
#line 209 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::TICKS;
                ticks_ = box<Ticks>(num());
            } break;
            case 31:
#line 216 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ &= ~RestRequest::SIDE;
                side_ = box<Side>(0);
            } break;
            case 32:
#line 220 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::SIDE;
                side_ = Side::BUY;
            } break;
            case 33:
#line 224 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::SIDE;
                side_ = Side::SELL;
            } break;
            case 34:
#line 232 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ &= ~RestRequest::LOTS;
                lots_ = 0_lts;
            } break;
            case 35:
#line 236 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::LOTS;
                lots_ = box<Lots>(num());
            } break;
            case 36:
#line 243 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ &= ~RestRequest::MIN_LOTS;
                minLots_ = 0_lts;
            } break;
            case 37:
#line 247 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::MIN_LOTS;
                minLots_ = box<Lots>(num());
            } break;
            case 38:
#line 254 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ &= ~RestRequest::ROLE;
                role_ = Role::NONE;
            } break;
            case 39:
#line 258 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::ROLE;
                role_ = Role::MAKER;
            } break;
            case 40:
#line 262 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::ROLE;
                role_ = Role::TAKER;
            } break;
            case 41:
#line 270 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ &= ~RestRequest::CPTY;
                cpty_.len = 0;
            } break;
            case 42:
#line 274 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                str_.len = &cpty_.len;
                str_.buf = cpty_.buf;
                str_.max = MNEM_MAX;
            } break;
            case 43:
#line 279 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::CPTY;
            } break;
#line 900 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.cpp"
            }
        }

    _again:
        if (cs == 0)
            goto _out;
        if (++p != pe)
            goto _resume;
    _test_eof : {
    }
    _out : {
    }
    }

#line 354 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
    cs_ = cs;

    if (cs == json_error)
        swirly::throwException<ParseException>("%s", msg);

    if (cs < json_first_final)
        return false;

    return true;
}

} // swirly

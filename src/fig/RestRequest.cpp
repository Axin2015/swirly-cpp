
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

#line 284 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"

#line 34 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.cpp"
static const char _json_actions[]
    = {0,  1,  2, 1,  4,  1,  10, 1,  12, 1,  14, 1,  16, 1, 18, 1,  20, 1,  22, 1,  24, 1, 26,
       1,  27, 1, 30, 1,  32, 1,  34, 1,  35, 1,  38, 2,  5, 28, 2,  6,  28, 2,  7,  36, 2, 8,
       36, 2,  9, 3,  2,  11, 3,  2,  13, 3,  2,  15, 3,  2, 17, 3,  2,  19, 0,  2,  21, 0, 2,
       23, 3,  2, 25, 0,  2,  29, 0,  2,  31, 0,  2,  33, 0, 2,  37, 3,  3,  19, 0,  1,  3, 19,
       0,  2,  3, 21, 0,  1,  3,  21, 0,  2,  3,  25, 0,  1, 3,  25, 0,  2,  3,  29, 0,  1, 3,
       29, 0,  2, 3,  31, 0,  1,  3,  31, 0,  2,  3,  33, 0, 1,  3,  33, 0,  2};

static const short _json_key_offsets[] = {
    0,   0,   4,   9,   25,  29,  31,  33,  35,  36,  40,  44,  46,  51,  56,  60,  60,  62,  64,
    65,  69,  73,  75,  80,  80,  82,  84,  86,  88,  90,  92,  93,  97,  101, 103, 108, 108, 112,
    114, 116, 118, 119, 123, 127, 129, 134, 134, 136, 138, 140, 142, 144, 146, 148, 150, 151, 155,
    162, 164, 171, 173, 175, 177, 178, 182, 189, 191, 198, 202, 204, 206, 208, 210, 212, 213, 217,
    224, 226, 233, 235, 237, 238, 242, 246, 248, 253, 253, 257, 259, 260, 264, 268, 270, 275, 275,
    277, 279, 280, 284, 288, 292, 294, 296, 298, 300, 301, 306, 308, 310, 312, 314, 315, 320, 326,
    328, 330, 332, 334, 336, 338, 340, 341, 345, 352, 354, 361, 363, 365, 366, 370, 374, 378, 380,
    382, 383, 388, 390, 392, 394, 395, 400, 402, 404, 406, 407, 411, 418, 420, 427, 431, 433, 435,
    437, 438, 442, 449, 451, 458, 460, 462, 464, 466, 467, 471, 475, 477, 482, 482};

static const char _json_trans_keys[] = {
    32,  123, 9,   13,  32,  34,  125, 9,   13,  67,  68,  69,  76,  77,  82,  83,  84,  99,  100,
    101, 108, 109, 114, 115, 116, 79,  80,  111, 112, 78,  110, 84,  116, 82,  114, 34,  32,  58,
    9,   13,  32,  34,  9,   13,  34,  92,  32,  44,  125, 9,   13,  32,  44,  125, 9,   13,  32,
    34,  9,   13,  84,  116, 89,  121, 34,  32,  58,  9,   13,  32,  34,  9,   13,  34,  92,  32,
    44,  125, 9,   13,  73,  105, 83,  115, 80,  112, 76,  108, 65,  97,  89,  121, 34,  32,  58,
    9,   13,  32,  34,  9,   13,  34,  92,  32,  44,  125, 9,   13,  77,  88,  109, 120, 65,  97,
    73,  105, 76,  108, 34,  32,  58,  9,   13,  32,  34,  9,   13,  34,  92,  32,  44,  125, 9,
    13,  80,  112, 73,  105, 82,  114, 89,  121, 68,  100, 65,  97,  84,  116, 69,  101, 34,  32,
    58,  9,   13,  32,  43,  45,  9,   13,  48,  57,  48,  57,  32,  44,  125, 9,   13,  48,  57,
    79,  111, 84,  116, 83,  115, 34,  32,  58,  9,   13,  32,  43,  45,  9,   13,  48,  57,  48,
    57,  32,  44,  125, 9,   13,  48,  57,  73,  78,  105, 110, 78,  110, 77,  109, 79,  111, 84,
    116, 83,  115, 34,  32,  58,  9,   13,  32,  43,  45,  9,   13,  48,  57,  48,  57,  32,  44,
    125, 9,   13,  48,  57,  69,  101, 77,  109, 34,  32,  58,  9,   13,  32,  34,  9,   13,  34,
    92,  32,  44,  125, 9,   13,  69,  79,  101, 111, 70,  102, 34,  32,  58,  9,   13,  32,  34,
    9,   13,  34,  92,  32,  44,  125, 9,   13,  76,  108, 69,  101, 34,  32,  58,  9,   13,  32,
    34,  9,   13,  77,  84,  109, 116, 65,  97,  75,  107, 69,  101, 82,  114, 34,  32,  44,  125,
    9,   13,  65,  97,  75,  107, 69,  101, 82,  114, 34,  32,  44,  125, 9,   13,  69,  73,  84,
    101, 105, 116, 84,  116, 84,  116, 76,  108, 68,  100, 65,  97,  84,  116, 69,  101, 34,  32,
    58,  9,   13,  32,  43,  45,  9,   13,  48,  57,  48,  57,  32,  44,  125, 9,   13,  48,  57,
    68,  100, 69,  101, 34,  32,  58,  9,   13,  32,  34,  9,   13,  66,  83,  98,  115, 85,  117,
    89,  121, 34,  32,  44,  125, 9,   13,  69,  101, 76,  108, 76,  108, 34,  32,  44,  125, 9,
    13,  65,  97,  84,  116, 69,  101, 34,  32,  58,  9,   13,  32,  43,  45,  9,   13,  48,  57,
    48,  57,  32,  44,  125, 9,   13,  48,  57,  73,  82,  105, 114, 67,  99,  75,  107, 83,  115,
    34,  32,  58,  9,   13,  32,  43,  45,  9,   13,  48,  57,  48,  57,  32,  44,  125, 9,   13,
    48,  57,  65,  97,  68,  100, 69,  101, 82,  114, 34,  32,  58,  9,   13,  32,  34,  9,   13,
    34,  92,  32,  44,  125, 9,   13,  32,  9,   13,  0};

static const char _json_single_lengths[]
    = {0, 2, 3, 16, 4, 2, 2, 2, 1, 2, 2, 2, 3, 3, 2, 0, 2, 2, 1, 2, 2, 2, 3, 0, 2, 2, 2, 2,
       2, 2, 1, 2,  2, 2, 3, 0, 4, 2, 2, 2, 1, 2, 2, 2, 3, 0, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2,
       3, 0, 3, 2,  2, 2, 1, 2, 3, 0, 3, 4, 2, 2, 2, 2, 2, 1, 2, 3, 0, 3, 2, 2, 1, 2, 2, 2,
       3, 0, 4, 2,  1, 2, 2, 2, 3, 0, 2, 2, 1, 2, 2, 4, 2, 2, 2, 2, 1, 3, 2, 2, 2, 2, 1, 3,
       6, 2, 2, 2,  2, 2, 2, 2, 1, 2, 3, 0, 3, 2, 2, 1, 2, 2, 4, 2, 2, 1, 3, 2, 2, 2, 1, 3,
       2, 2, 2, 1,  2, 3, 0, 3, 4, 2, 2, 2, 1, 2, 3, 0, 3, 2, 2, 2, 2, 1, 2, 2, 2, 3, 0, 1};

static const char _json_range_lengths[]
    = {0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0,
       0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
       2, 1, 2, 0, 0, 0, 0, 1, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 2, 0, 0, 0, 1, 1, 0,
       1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 2, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
       0, 0, 0, 0, 1, 2, 1, 2, 0, 0, 0, 0, 0, 1, 2, 1, 2, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1};

static const short _json_index_offsets[] = {
    0,   0,   4,   9,   26,  31,  34,  37,  40,  42,  46,  50,  53,  58,  63,  67,  68,  71,  74,
    76,  80,  84,  87,  92,  93,  96,  99,  102, 105, 108, 111, 113, 117, 121, 124, 129, 130, 135,
    138, 141, 144, 146, 150, 154, 157, 162, 163, 166, 169, 172, 175, 178, 181, 184, 187, 189, 193,
    199, 201, 207, 210, 213, 216, 218, 222, 228, 230, 236, 241, 244, 247, 250, 253, 256, 258, 262,
    268, 270, 276, 279, 282, 284, 288, 292, 295, 300, 301, 306, 309, 311, 315, 319, 322, 327, 328,
    331, 334, 336, 340, 344, 349, 352, 355, 358, 361, 363, 368, 371, 374, 377, 380, 382, 387, 394,
    397, 400, 403, 406, 409, 412, 415, 417, 421, 427, 429, 435, 438, 441, 443, 447, 451, 456, 459,
    462, 464, 469, 472, 475, 478, 480, 485, 488, 491, 494, 496, 500, 506, 508, 514, 519, 522, 525,
    528, 530, 534, 540, 542, 548, 551, 554, 557, 560, 562, 566, 570, 573, 578, 579};

static const unsigned char _json_indicies[] = {
    0,   2,   0,   1,   2,   3,   4,   2,   1,   5,   6,   7,   8,   9,   10,  11,  12,  5,   6,
    7,   8,   9,   10,  11,  12,  1,   13,  14,  13,  14,  1,   15,  15,  1,   16,  16,  1,   17,
    17,  1,   18,  1,   18,  19,  18,  1,   19,  20,  19,  1,   22,  23,  21,  24,  25,  26,  24,
    1,   27,  28,  4,   27,  1,   28,  3,   28,  1,   21,  29,  29,  1,   30,  30,  1,   31,  1,
    31,  32,  31,  1,   32,  33,  32,  1,   35,  36,  34,  37,  38,  39,  37,  1,   34,  40,  40,
    1,   41,  41,  1,   42,  42,  1,   43,  43,  1,   44,  44,  1,   45,  45,  1,   46,  1,   46,
    47,  46,  1,   47,  48,  47,  1,   50,  51,  49,  52,  53,  54,  52,  1,   49,  55,  56,  55,
    56,  1,   57,  57,  1,   58,  58,  1,   59,  59,  1,   60,  1,   60,  61,  60,  1,   61,  62,
    61,  1,   64,  65,  63,  66,  67,  68,  66,  1,   63,  69,  69,  1,   70,  70,  1,   71,  71,
    1,   72,  72,  1,   73,  73,  1,   74,  74,  1,   75,  75,  1,   76,  76,  1,   77,  1,   77,
    78,  77,  1,   78,  79,  80,  78,  81,  1,   82,  1,   83,  84,  85,  83,  82,  1,   86,  86,
    1,   87,  87,  1,   88,  88,  1,   89,  1,   89,  90,  89,  1,   90,  91,  92,  90,  93,  1,
    94,  1,   95,  96,  97,  95,  94,  1,   98,  99,  98,  99,  1,   100, 100, 1,   101, 101, 1,
    102, 102, 1,   103, 103, 1,   104, 104, 1,   105, 1,   105, 106, 105, 1,   106, 107, 108, 106,
    109, 1,   110, 1,   111, 112, 113, 111, 110, 1,   114, 114, 1,   115, 115, 1,   116, 1,   116,
    117, 116, 1,   117, 118, 117, 1,   120, 121, 119, 122, 123, 124, 122, 1,   119, 125, 126, 125,
    126, 1,   127, 127, 1,   128, 1,   128, 129, 128, 1,   129, 130, 129, 1,   132, 133, 131, 134,
    135, 136, 134, 1,   131, 137, 137, 1,   138, 138, 1,   139, 1,   139, 140, 139, 1,   140, 141,
    140, 1,   142, 143, 142, 143, 1,   144, 144, 1,   145, 145, 1,   146, 146, 1,   147, 147, 1,
    148, 1,   149, 150, 151, 149, 1,   152, 152, 1,   153, 153, 1,   154, 154, 1,   155, 155, 1,
    156, 1,   157, 158, 159, 157, 1,   160, 161, 162, 160, 161, 162, 1,   163, 163, 1,   164, 164,
    1,   165, 165, 1,   166, 166, 1,   167, 167, 1,   168, 168, 1,   169, 169, 1,   170, 1,   170,
    171, 170, 1,   171, 172, 173, 171, 174, 1,   175, 1,   176, 177, 178, 176, 175, 1,   179, 179,
    1,   180, 180, 1,   181, 1,   181, 182, 181, 1,   182, 183, 182, 1,   184, 185, 184, 185, 1,
    186, 186, 1,   187, 187, 1,   188, 1,   189, 190, 191, 189, 1,   192, 192, 1,   193, 193, 1,
    194, 194, 1,   195, 1,   196, 197, 198, 196, 1,   199, 199, 1,   200, 200, 1,   201, 201, 1,
    202, 1,   202, 203, 202, 1,   203, 204, 205, 203, 206, 1,   207, 1,   208, 209, 210, 208, 207,
    1,   211, 212, 211, 212, 1,   213, 213, 1,   214, 214, 1,   215, 215, 1,   216, 1,   216, 217,
    216, 1,   217, 218, 219, 217, 220, 1,   221, 1,   222, 223, 224, 222, 221, 1,   225, 225, 1,
    226, 226, 1,   227, 227, 1,   228, 228, 1,   229, 1,   229, 230, 229, 1,   230, 231, 230, 1,
    233, 234, 232, 235, 236, 237, 235, 1,   232, 4,   4,   1,   0};

static const unsigned char _json_trans_targs[] = {
    1,   0,   2,   3,   167, 4,   24,  36,  59,  67,  86,  112, 148, 5,   16,  6,   7,   8,   9,
    10,  11,  11,  12,  15,  13,  14,  167, 13,  14,  17,  18,  19,  20,  21,  21,  22,  23,  13,
    14,  167, 25,  26,  27,  28,  29,  30,  31,  32,  33,  33,  34,  35,  13,  14,  167, 37,  46,
    38,  39,  40,  41,  42,  43,  43,  44,  45,  13,  14,  167, 47,  48,  49,  50,  51,  52,  53,
    54,  55,  56,  57,  57,  58,  58,  13,  14,  167, 60,  61,  62,  63,  64,  65,  65,  66,  66,
    13,  14,  167, 68,  78,  69,  70,  71,  72,  73,  74,  75,  76,  76,  77,  77,  13,  14,  167,
    79,  80,  81,  82,  83,  83,  84,  85,  13,  14,  167, 87,  94,  88,  89,  90,  91,  91,  92,
    93,  13,  14,  167, 95,  96,  97,  98,  99,  100, 106, 101, 102, 103, 104, 105, 13,  14,  167,
    107, 108, 109, 110, 111, 13,  14,  167, 113, 125, 140, 114, 115, 116, 117, 118, 119, 120, 121,
    122, 123, 123, 124, 124, 13,  14,  167, 126, 127, 128, 129, 130, 131, 135, 132, 133, 134, 13,
    14,  167, 136, 137, 138, 139, 13,  14,  167, 141, 142, 143, 144, 145, 146, 146, 147, 147, 13,
    14,  167, 149, 157, 150, 151, 152, 153, 154, 155, 155, 156, 156, 13,  14,  167, 158, 159, 160,
    161, 162, 163, 164, 164, 165, 166, 13,  14,  167};

static const unsigned char _json_trans_actions[] = {
    0,   0,  0,  0,  0,   0,   0,  0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  59,  3,
    0,   0,  13, 13, 13,  0,   0,  0,   0,   0,  0,  83, 3,  0,  0,  33, 33, 33, 0,  0,  0,   0,
    0,   0,  0,  0,  50,  3,   0,  0,   7,   7,  7,  0,  0,  0,  0,  0,  0,  0,  53, 3,  0,   0,
    9,   9,  9,  0,  0,   0,   0,  0,   0,   0,  0,  0,  0,  65, 94, 98, 1,  17, 17, 17, 0,   0,
    0,   0,  0,  77, 118, 122, 1,  27,  27,  27, 0,  0,  0,  0,  0,  0,  0,  0,  0,  80, 126, 130,
    1,   29, 29, 29, 0,   0,   0,  0,   47,  3,  0,  0,  5,  5,  5,  0,  0,  0,  0,  0,  68,  3,
    0,   0,  19, 19, 19,  0,   0,  0,   0,   31, 0,  0,  0,  0,  0,  0,  0,  41, 41, 41, 0,   0,
    0,   0,  0,  44, 44,  44,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  62, 86, 90,  1,
    15,  15, 15, 0,  0,   0,   0,  23,  0,   0,  0,  0,  0,  35, 35, 35, 0,  0,  0,  0,  38,  38,
    38,  0,  0,  0,  0,   0,   71, 102, 106, 1,  21, 21, 21, 0,  0,  0,  0,  0,  0,  0,  74,  110,
    114, 1,  25, 25, 25,  0,   0,  0,   0,   0,  0,  56, 3,  0,  0,  11, 11, 11};

static const int json_start = 1;
static const int json_first_final = 167;
static const int json_error = 0;

static const int json_en_main = 1;

#line 287 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"

} // anonymous

RestRequest::~RestRequest() noexcept = default;

void RestRequest::reset() noexcept
{
    auto cs = cs_;

#line 374 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.cpp"
    {
        cs = json_start;
    }

#line 296 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
    cs_ = cs;

    fields_ = 0;
}

bool RestRequest::parse(const StringView& buf)
{
    const char* p{buf.data()};
    const char* pe{p + buf.size()};
    const char* msg{"parse error"};

    auto cs = cs_;

#line 393 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.cpp"
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
        _trans = _json_indicies[_trans];
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
                    msg = "max string length exceeded";
                    {
                        p++;
                        goto _out;
                    }
                }
            } break;
            case 5:
#line 78 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                side_ = Side::BUY;
            } break;
            case 6:
#line 81 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                side_ = Side::SELL;
            } break;
            case 7:
#line 87 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                role_ = Role::MAKER;
            } break;
            case 8:
#line 90 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                role_ = Role::TAKER;
            } break;
            case 9:
#line 99 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                if (fields_ & RestRequest::MNEM) {
                    cs = json_error;
                    msg = "mnem already specified";
                    {
                        p++;
                        goto _out;
                    }
                }
                str_.len = &mnem_.len;
                str_.buf = mnem_.buf;
                str_.max = MNEM_MAX;
            } break;
            case 10:
#line 108 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::MNEM;
            } break;
            case 11:
#line 111 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                if (fields_ & RestRequest::DISPLAY) {
                    cs = json_error;
                    msg = "display already specified";
                    {
                        p++;
                        goto _out;
                    }
                }
                str_.len = &display_.len;
                str_.buf = display_.buf;
                str_.max = DISPLAY_MAX;
            } break;
            case 12:
#line 120 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::DISPLAY;
            } break;
            case 13:
#line 123 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                if (fields_ & RestRequest::EMAIL) {
                    cs = json_error;
                    msg = "email already specified";
                    {
                        p++;
                        goto _out;
                    }
                }
                str_.len = &email_.len;
                str_.buf = email_.buf;
                str_.max = EMAIL_MAX;
            } break;
            case 14:
#line 132 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::EMAIL;
            } break;
            case 15:
#line 135 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                if (fields_ & RestRequest::TRADER) {
                    cs = json_error;
                    msg = "trader already specified";
                    {
                        p++;
                        goto _out;
                    }
                }
                str_.len = &trader_.len;
                str_.buf = trader_.buf;
                str_.max = MNEM_MAX;
            } break;
            case 16:
#line 144 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::TRADER;
            } break;
            case 17:
#line 147 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                if (fields_ & RestRequest::CONTR) {
                    cs = json_error;
                    msg = "contr already specified";
                    {
                        p++;
                        goto _out;
                    }
                }
                str_.len = &contr_.len;
                str_.buf = contr_.buf;
                str_.max = MNEM_MAX;
            } break;
            case 18:
#line 156 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::CONTR;
            } break;
            case 19:
#line 159 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                if (fields_ & RestRequest::SETTL_DATE) {
                    cs = json_error;
                    msg = "settl-date already specified";
                    {
                        p++;
                        goto _out;
                    }
                }
            } break;
            case 20:
#line 165 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::SETTL_DATE;
                settlDate_ = box<IsoDate>(num());
            } break;
            case 21:
#line 169 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                if (fields_ & RestRequest::EXPIRY_DATE) {
                    cs = json_error;
                    msg = "expiry-date already specified";
                    {
                        p++;
                        goto _out;
                    }
                }
            } break;
            case 22:
#line 175 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::EXPIRY_DATE;
                expiryDate_ = box<IsoDate>(num());
            } break;
            case 23:
#line 179 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                if (fields_ & RestRequest::REF) {
                    cs = json_error;
                    msg = "ref already specified";
                    {
                        p++;
                        goto _out;
                    }
                }
                str_.len = &ref_.len;
                str_.buf = ref_.buf;
                str_.max = REF_MAX;
            } break;
            case 24:
#line 188 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::REF;
            } break;
            case 25:
#line 191 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                if (fields_ & RestRequest::STATE) {
                    cs = json_error;
                    msg = "state already specified";
                    {
                        p++;
                        goto _out;
                    }
                }
            } break;
            case 26:
#line 197 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::STATE;
                state_ = num();
            } break;
            case 27:
#line 201 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                if (fields_ & RestRequest::SIDE) {
                    cs = json_error;
                    msg = "side already specified";
                    {
                        p++;
                        goto _out;
                    }
                }
            } break;
            case 28:
#line 207 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::SIDE;
            } break;
            case 29:
#line 210 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                if (fields_ & RestRequest::TICKS) {
                    cs = json_error;
                    msg = "ticks already specified";
                    {
                        p++;
                        goto _out;
                    }
                }
            } break;
            case 30:
#line 216 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::TICKS;
                ticks_ = box<Ticks>(num());
            } break;
            case 31:
#line 220 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                if (fields_ & RestRequest::LOTS) {
                    cs = json_error;
                    msg = "lots already specified";
                    {
                        p++;
                        goto _out;
                    }
                }
            } break;
            case 32:
#line 226 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::LOTS;
                lots_ = box<Lots>(num());
            } break;
            case 33:
#line 230 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                if (fields_ & RestRequest::MIN_LOTS) {
                    cs = json_error;
                    msg = "min-lots already specified";
                    {
                        p++;
                        goto _out;
                    }
                }
            } break;
            case 34:
#line 236 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::MIN_LOTS;
                minLots_ = box<Lots>(num());
            } break;
            case 35:
#line 240 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                if (fields_ & RestRequest::ROLE) {
                    cs = json_error;
                    msg = "role already specified";
                    {
                        p++;
                        goto _out;
                    }
                }
            } break;
            case 36:
#line 246 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::ROLE;
            } break;
            case 37:
#line 249 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                if (fields_ & RestRequest::CPTY) {
                    cs = json_error;
                    msg = "cpty already specified";
                    {
                        p++;
                        goto _out;
                    }
                }
                str_.len = &cpty_.len;
                str_.buf = cpty_.buf;
                str_.max = MNEM_MAX;
            } break;
            case 38:
#line 258 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
            {
                fields_ |= RestRequest::CPTY;
            } break;
#line 780 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.cpp"
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

#line 309 "/Users/marayl/repo/swirlyc/src/fig/RestRequest.rl"
    cs_ = cs;

    if (cs == json_error)
        swirly::throwException<ParseException>("%s", msg);

    if (cs < json_first_final)
        return false;

    return true;
}

} // swirly

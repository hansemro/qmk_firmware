/*
 * Copyright (c) 2022 Hansem Ro <hansemro@outlook.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include "quantum.h"
#include <stdint.h>

#define XXX KC_NO

// Matrix keymap
// clang-format off
#define LAYOUT_tkl_ansi( \
      K0C,        K1C,  K2C,  K3C,  K0D,  K1D,  K2D,  K0E,  K1E,  K2E,  K38,  K48,  K58,      K6C,  K7C,  K3D,  \
      K00,  K10,  K01,  K11,  K02,  K12,  K03,  K13,  K04,  K14,  K05,  K15,  K06,  K16,      K07,  K17,  K08,  \
      K20,  K30,  K21,  K31,  K22,  K32,  K23,  K33,  K24,  K34,  K25,  K35,  K26,  K36,      K27,  K37,  K28,  \
      K40,  K50,  K41,  K51,  K42,  K52,  K43,  K53,  K44,  K54,  K45,  K55,        K46,                        \
      K60,        K70,  K61,  K71,  K62,  K72,  K63,  K73,  K64,  K74,  K65,        K75,            K57,        \
      K3B,  K2B,  K4B,                    K5B,                    K6B,  K7B,  K47,  K66,      K77,  K67,  K68   \
) { \
    /*          Col1  Col2  Col3  Col4  Col5  Col6  Col7  Col8  Col9  Col10 Col11 Col12 Col13 Col14 Col15 */ \
    /*Row1*/ {  K00,  K01,  K02,  K03,  K04,  K05,  K06,  K07,  K08,  XXX,  XXX,  XXX,  K0C,  K0D,  K0E },  \
    /*Row2*/ {  K10,  K11,  K12,  K13,  K14,  K15,  K16,  K17,  XXX,  XXX,  XXX,  XXX,  K1C,  K1D,  K1E },  \
    /*Row3*/ {  K20,  K21,  K22,  K23,  K24,  K25,  K26,  K27,  K28,  XXX,  XXX,  K2B,  K2C,  K2D,  K2E },  \
    /*Row4*/ {  K30,  K31,  K32,  K33,  K34,  K35,  K36,  K37,  K38,  XXX,  XXX,  K3B,  K3C,  K3D,  XXX },  \
    /*Row5*/ {  K40,  K41,  K42,  K43,  K44,  K45,  K46,  K47,  K48,  XXX,  XXX,  K4B,  XXX,  XXX,  XXX },  \
    /*Row6*/ {  K50,  K51,  K52,  K53,  K54,  K55,  XXX,  K57,  K58,  XXX,  XXX,  K5B,  XXX,  XXX,  XXX },  \
    /*Row7*/ {  K60,  K61,  K62,  K63,  K64,  K65,  K66,  K67,  K68,  XXX,  XXX,  K6B,  K6C,  XXX,  XXX },  \
    /*Row8*/ {  K70,  K71,  K72,  K73,  K74,  K75,  XXX,  K77,  XXX,  XXX,  XXX,  K7B,  K7C,  XXX,  XXX }   \
}

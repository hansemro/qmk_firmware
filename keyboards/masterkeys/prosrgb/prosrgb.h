/*
 * Copyright (c) 2022 Hansem Ro
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
      K00,        K01,  K02,  K03,  K04,  K05,  K06,  K07,  K08,  K09,  K0A,  K0B,  K0C,      K0D,  K0E,  K61,  \
      K10,  K11,  K12,  K13,  K14,  K15,  K16,  K17,  K18,  K19,  K1A,  K1B,  K1C,  K1E,      K2E,  K62,  K63,  \
      K20,  K21,  K22,  K23,  K24,  K25,  K26,  K27,  K28,  K29,  K2A,  K2B,  K2C,  K2D,      K3E,  K68,  K69,  \
      K30,  K31,  K32,  K33,  K34,  K35,  K36,  K37,  K38,  K39,  K3A,  K3B,        K3D,                        \
      K40,        K42,  K43,  K44,  K45,  K46,  K47,  K48,  K49,  K4A,  K4B,        K4D,            K4E,        \
      K50,  K51,  K52,                    K56,                    K59,  K5A,  K5B,  K5C,      K5D,  K5E,  K6E   \
) { \
    { K10,  K12,  K14,  K16,  K18,  K1A,  K1C,  K2E,  K63,  XXX,  XXX,  XXX,  K00,  K04,  K07 },  \
    { K11,  K13,  K15,  K17,  K19,  K1B,  K1E,  K62,  XXX,  XXX,  XXX,  XXX,  K01,  K05,  K08 },  \
    { K20,  K22,  K24,  K26,  K28,  K2A,  K2C,  K3E,  K69,  XXX,  XXX,  K51,  K02,  K06,  K09 },  \
    { K21,  K23,  K25,  K27,  K29,  K2B,  K2D,  K68,  K0A,  XXX,  XXX,  K50,  K03,  K61,  XXX },  \
    { K30,  K32,  K34,  K36,  K38,  K3A,  K3D,  K5B,  K0B,  XXX,  XXX,  K52,  XXX,  XXX,  XXX },  \
    { K31,  K33,  K35,  K37,  K39,  K3B,  XXX,  K4E,  K0C,  XXX,  XXX,  K56,  XXX,  XXX,  XXX },  \
    { K40,  K43,  K45,  K47,  K49,  K4B,  K5C,  K5E,  K6E,  XXX,  XXX,  K59,  K0D,  XXX,  XXX },  \
    { K42,  K44,  K46,  K48,  K4A,  K4D,  XXX,  K5D,  XXX,  XXX,  XXX,  K5A,  K0E,  XXX,  XXX }   \
}

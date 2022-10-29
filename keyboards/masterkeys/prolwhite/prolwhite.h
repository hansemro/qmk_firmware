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
#define LAYOUT( \
    k31,      k62, k63, k13, k33, k70, k36, k17, k67, k6A, k7A, k3A, k5A,   k79, k09, k00,   k5C, k5D, k5E, k7E, \
    k61, k71, k72, k73, k74, k64, k65, k75, k76, k77, k78, k68, k66, k1A,   k30, k10, k20,   k4C, k4D, k4E, k6E, \
    k11, k01, k02, k03, k04, k14, k15, k05, k06, k07, k08, k18, k16, k2A,   k6B, k4B, k0B,   k0C, k0D, k0E, k6D, \
    k12, k21, k22, k23, k24, k34, k35, k25, k26, k27, k28, k38,      k4A,                    k1C, k1D, k1E, \
    k19,      k41, k42, k43, k44, k54, k55, k45, k46, k47, k58,      k29,        k0A,        k2C, k2D, k2E, k7D, \
    k60, k2B, k39,                k3B,                k59, k69, k57, k40,   k7B, k5B, k50,   k3D,      k3E \
) { \
    { k00, k01, k02, k03, k04, k05, k06, k07, k08, k09, k0A, k0B, k0C, k0D, k0E, XXX }, \
    { k10, k11, k12, k13, k14, k15, k16, k17, k18, k19, k1A, XXX, k1C, k1D, k1E, XXX }, \
    { k20, k21, k22, k23, k24, k25, k26, k27, k28, k29, k2A, k2B, k2C, k2D, k2E, XXX }, \
    { k30, k31, XXX, k33, k34, k35, k36, XXX, k38, k39, k3A, k3B, XXX, k3D, k3E, XXX }, \
    { k40, k41, k42, k43, k44, k45, k46, k47, XXX, XXX, k4A, k4B, k4C, k4D, k4E, XXX }, \
    { k50, XXX, XXX, XXX, k54, k55, XXX, k57, k58, k59, k5A, k5B, k5C, k5D, k5E, XXX }, \
    { k60, k61, k62, k63, k64, k65, k66, k67, k68, k69, k6A, k6B, XXX, k6D, k6E, XXX }, \
    { k70, k71, k72, k73, k74, k75, k76, k77, k78, k79, k7A, k7B, XXX, k7D, k7E, XXX }, \
}

#define LAYOUT_fullsize_ansi( \
    k31,      k62, k63, k13, k33, k70, k36, k17, k67, k6A, k7A, k3A, k5A,   k79, k09, k00, \
    k61, k71, k72, k73, k74, k64, k65, k75, k76, k77, k78, k68, k66, k1A,   k30, k10, k20,   k4C, k4D, k4E, k6E, \
    k11, k01, k02, k03, k04, k14, k15, k05, k06, k07, k08, k18, k16, k2A,   k6B, k4B, k0B,   k0C, k0D, k0E, k6D, \
    k12, k21, k22, k23, k24, k34, k35, k25, k26, k27, k28, k38,      k4A,                    k1C, k1D, k1E, \
    k19,      k41, k42, k43, k44, k54, k55, k45, k46, k47, k58,      k29,        k0A,        k2C, k2D, k2E, k7D, \
    k60, k2B, k39,                k3B,                k59, k69, k57, k40,   k7B, k5B, k50,   k3D,      k3E \
) { \
    { k00, k01, k02, k03, k04, k05, k06, k07, k08, k09, k0A, k0B, k0C, k0D, k0E, XXX }, \
    { k10, k11, k12, k13, k14, k15, k16, k17, k18, k19, k1A, XXX, k1C, k1D, k1E, XXX }, \
    { k20, k21, k22, k23, k24, k25, k26, k27, k28, k29, k2A, k2B, k2C, k2D, k2E, XXX }, \
    { k30, k31, XXX, k33, k34, k35, k36, XXX, k38, k39, k3A, k3B, XXX, k3D, k3E, XXX }, \
    { k40, k41, k42, k43, k44, k45, k46, k47, XXX, XXX, k4A, k4B, k4C, k4D, k4E, XXX }, \
    { k50, XXX, XXX, XXX, k54, k55, XXX, k57, k58, k59, k5A, k5B, XXX, XXX, XXX, XXX }, \
    { k60, k61, k62, k63, k64, k65, k66, k67, k68, k69, k6A, k6B, XXX, k6D, k6E, XXX }, \
    { k70, k71, k72, k73, k74, k75, k76, k77, k78, k79, k7A, k7B, XXX, k7D, XXX, XXX }, \
}
// clang-format on

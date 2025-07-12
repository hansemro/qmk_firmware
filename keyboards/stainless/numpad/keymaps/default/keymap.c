// Copyright 2024 ccoffey (@ccoffey)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base layer - Numpad layout */
    [0] = LAYOUT(
        MO(1),   KC_ESC,  KC_TAB,  KC_BSPC,
        KC_NUM,  KC_PSLS, KC_PAST, KC_PMNS,
        KC_P7,   KC_P8,   KC_P9,   KC_PPLS,
        KC_P4,   KC_P5,   KC_P6,   KC_NO,
        KC_P1,   KC_P2,   KC_P3,   KC_PENT,
        KC_P0,   KC_NO,   KC_PDOT, KC_NO
    ),
    
    /* Function layer - Media controls */
    [1] = LAYOUT(
        _______,  _______, _______, _______,
        _______,  _______, _______, KC_VOLD,
        KC_MPRV,  KC_MPLY, KC_MNXT, KC_VOLU,
        _______,  _______, _______, _______,
        _______,  _______, _______, _______,
        _______,  _______, KC_MSTP, _______
    )
};
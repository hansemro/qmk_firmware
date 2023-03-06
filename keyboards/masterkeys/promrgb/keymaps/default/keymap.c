/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2023 Hansem Ro <hansemro@outlook.com>
 */
#include QMK_KEYBOARD_H
#include "mbia043.h"

enum promrgb_keys {
    TG_NUM
};

enum promrgb_layers {
    BASE,
    NUM,
    FN1,
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * ┌───┐   ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐
     * │Esc│   │F1 │F2 │F3 │F4 │ │F5 │F6 │F7 │F8 │ │F9 │F10│F11│F12│
     * └───┘   └───┴───┴───┴───┘ └───┴───┴───┴───┘ └───┴───┴───┴───┘
     * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───────┐ ┌───┬───┬───┬───┐
     * │ ` │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │ - │ = │ Backsp│ │Num│PSc│Slk│Pse│
     * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┤ ├───┼───┼───┼───┤
     * │ Tab │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │ [ │ ] │  \  │ │Ins│Hom│PgU│   │
     * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┤ ├───┼───┼───┤   │
     * │ Caps │ A │ S │ D │ F │ G │ H │ J │ K │ L │ ; │ ' │  Enter │ │Del│End│PgD│   │
     * ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────────┤ ├───┼───┼───┼───┤
     * │ Shift  │ Z │ X │ C │ V │ B │ N │ M │ , │ . │ / │    Shift │ │   │ ↑ │   │   │
     * ├────┬───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴───┼───┴┬────┬────┤ ├───┼───┼───┤   │
     * │Ctrl│GUI │Alt │                        │ Alt│ GUI│ FN │Ctrl│ │ ← │ ↓ │ → │   │
     * └────┴────┴────┴────────────────────────┴────┴────┴────┴────┘ └───┴───┴───┴───┘
     */
    [BASE] = LAYOUT(
        KC_ESC,           KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,

        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,     TG_NUM,  KC_PSCR, KC_SCRL, KC_PAUS,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,     KC_INS,  KC_HOME, KC_PGUP, _______,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,      KC_DEL,  KC_END,  KC_PGDN,
        KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT,     _______, KC_UP,   _______, _______,
        KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_RALT, KC_RGUI, MO(FN1), KC_RCTL,     KC_LEFT, KC_DOWN, KC_RGHT
    ),
    /*
     * ┌───┐   ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐
     * │Esc│   │F1 │F2 │F3 │F4 │ │F5 │F6 │F7 │F8 │ │F9 │F10│F11│F12│
     * └───┘   └───┴───┴───┴───┘ └───┴───┴───┴───┘ └───┴───┴───┴───┘
     * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───────┐ ┌───┬───┬───┬───┐
     * │ ` │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │ - │ = │ Backsp│ │Num│ / │ * │ - │
     * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┤ ├───┼───┼───┼───┤
     * │ Tab │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │ [ │ ] │  \  │ │ 7 │ 8 │ 9 │   │
     * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┤ ├───┼───┼───┤ + │
     * │ Caps │ A │ S │ D │ F │ G │ H │ J │ K │ L │ ; │ ' │  Enter │ │ 4 │ 5 │ 6 │   │
     * ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────────┤ ├───┼───┼───┼───┤
     * │ Shift  │ Z │ X │ C │ V │ B │ N │ M │ , │ . │ / │    Shift │ │ 1 │ 2 │ 3 │   │
     * ├────┬───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴───┼───┴┬────┬────┤ ├───┼───┼───┤Ent│
     * │Ctrl│GUI │Alt │                        │ Alt│ GUI│ FN │Ctrl│ │ 0 │ 0 │ . │   │
     * └────┴────┴────┴────────────────────────┴────┴────┴────┴────┘ └───┴───┴───┴───┘
     */
    [NUM] = LAYOUT(
        _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,

        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,    _______,  KC_PSLS, KC_PAST, KC_PMNS,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,    KC_P7,    KC_P8,   KC_P9,   KC_PPLS,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,    KC_P4,    KC_P5,   KC_P6,
        _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,    KC_P1,    KC_P2,   KC_P3,   KC_PENT,
        _______, _______, _______,                            _______,                            _______, _______, _______, _______,    KC_P0,    KC_P0,   KC_PDOT
    ),
    [FN1] = LAYOUT(
        QK_BOOT,          RGB_HUI, RGB_SAI, RGB_VAI, RGB_MOD, _______, _______, _______, _______, _______, _______, _______, _______,

        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,    _______,  _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,    KC_MPLY,  KC_MSTP, KC_VOLU, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,    KC_MPRV,  KC_MNXT, KC_VOLD,
        _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,    _______,  _______, _______, _______,
        _______, _______, _______,                            _______,                            _______, _______, _______, _______,    _______,  _______, _______
    )
};
// clang-format on

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TG_NUM:
            if (record->event.pressed) {
                tap_code(KC_NUM);
                if (layer_state_is(BASE)) {
                    layer_move(NUM);
                } else {
                    layer_move(BASE);
                }
            }
            return false;
        default:
            return true;
    }
}

#ifdef RGB_MATRIX_ENABLE
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (host_keyboard_led_state().caps_lock) {
        RGB_MATRIX_INDICATOR_SET_COLOR(93, 0x0, 0x0, 0xff);
    } else {
        RGB_MATRIX_INDICATOR_SET_COLOR(93, 0, 0, 0);
    }
    if (host_keyboard_led_state().scroll_lock) {
        RGB_MATRIX_INDICATOR_SET_COLOR(94, 0x0, 0x0, 0xff);
    } else {
        RGB_MATRIX_INDICATOR_SET_COLOR(94, 0, 0, 0);
    }
    if (host_keyboard_led_state().num_lock) {
        if (layer_state_is(BASE)) {
            layer_move(NUM);
        }
        RGB_MATRIX_INDICATOR_SET_COLOR(92, 0x0, 0x0, 0xff);
    } else {
        if (layer_state_is(NUM)) {
            layer_move(BASE);
        }
        RGB_MATRIX_INDICATOR_SET_COLOR(92, 0, 0, 0);
    }
    return false;
}
#endif

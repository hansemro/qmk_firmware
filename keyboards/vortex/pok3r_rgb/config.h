/*
 * Copyright (c) 2021 Charlie Waters
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

#ifndef CONFIG_H
#define CONFIG_H

#define PRODUCT         Vortex POK3R RGB

#define DESCRIPTION     Vortex POK3R RGB with qmk_pok3r
#define NAME_SLUG       "vortex/pok3r_rgb"

// key matrix size
#define MATRIX_COLS 8
#define MATRIX_ROWS 10
// layer size: MATRIX_ROWS * MATRIX_COLS * sizeof(uint16_t) = 160 bytes

#define DIODE_DIRECTION COL2ROW

#define MATRIX_COL_PINS { C9, A5, A6, A7, C11, C13, C14, C15 }
#define MATRIX_ROW_PINS { C4, B6, D0, B11, A11, A0, A1, A2, A3, A4 }

#define MATRIX_IO_DELAY 10

#define DEBOUNCE 5

#define DIP_SWITCH_MATRIX_GRID { { 9, 4 }, { 9, 5 }, { 9, 6 }, { 9, 7 } }

#define GPIO_INPUT_PIN_DELAY 10

// SPI EERPOM
#define SPI_DRIVER          SPID1
#define SPI_SCK_PIN         B7
#define SPI_SCK_PAL_MODE    AFIO_SPI
#define SPI_MOSI_PIN        B8
#define SPI_MOST_PAL_MODE   AFIO_SPI
#define SPI_MISO_PIN        B9
#define SPI_MISO_PIN_MODE   ADIO_SPI

#define EXTERNAL_EEPROM_SPI_SLAVE_SELECT_PIN    B10

// Max available layers
//#define MAX_LAYERS 8
// keymaps size: layer size * MAX_LAYERS = 1280 bytes

//#define KEYMAP_60_ANSI
// Number of supported layouts
//#define NUM_LAYOUTS 4

//#include "layouts.h"

//#define RGBLED_NUM (61 + 1 + 2) // 61 keys, caps lock, 2 under spacebar

#define XXX KC_NO

// Matrix keymap

#define LAYOUT_all( \
    K01, K02, K03, K04, K05, K06, K07, K08, K36, K37, K38, K39, K41,  K40,  /* 14 keys */ \
    K09,  K10, K11, K12, K13, K14, K15, K16, K42, K43, K44, K45, K47,  K46, /* 14 keys */ \
     K17,  K18, K19, K20, K21, K22, K23, K24, K48, K49, K50, K51,    K52,   /* 13 keys */ \
     K25, K63, K26, K27, K28, K29, K30, K31, K32, K54, K55, K56,  K57, K58, /* 14 keys */ \
    K33,  K34,  K35,               K53,              K59,  K60,  K61,  K62, /* 8 keys */  \
                                                        S01, S02, S03, S04  /* 4 dip switches */ \
) { \
    /*            COL1   COL2   COL3   COL4   COL5   COL6   COL7   COL8   */ \
    /* ROW1  */ { K01,   K02,   K03,   K04,   K05,   K06,   K07,   K08,   }, \
    /* ROW2  */ { K09,   K10,   K11,   K12,   K13,   K14,   K15,   K16,   }, \
    /* ROW3  */ { K17,   K18,   K19,   K20,   K21,   K22,   K23,   K24,   }, \
    /* ROW4  */ { K25,   K26,   K27,   K28,   K29,   K30,   K31,   K32,   }, \
    /* ROW5  */ { K33,   K34,   K35,   K36,   K37,   K38,   K39,   K40,   }, \
    /* ROW6  */ { K63,   KC_NO, K41,   K42,   K43,   K44,   K45,   K46,   }, \
    /* ROW7  */ { KC_NO, KC_NO, K47,   K48,   K49,   K50,   K51,   K52,   }, \
    /* ROW8  */ { KC_NO, KC_NO, K53,   K54,   K55,   K56,   K57,   K58,   }, \
    /* ROW9  */ { KC_NO, KC_NO, KC_NO, K59,   K60,   K61,   KC_NO, K62,   }, \
    /* ROW10 */ { S01,   S02,   S03,   S04,   KC_NO, KC_NO, KC_NO, KC_NO, }  \
}

#define LAYOUT_60_ansi( \
    K01, K02, K03, K04, K05, K06, K07, K08, K36, K37, K38, K39, K41,  K40,  /* 14 keys */ \
    K09,  K10, K11, K12, K13, K14, K15, K16, K42, K43, K44, K45, K47,  K46, /* 14 keys */ \
     K17,  K18, K19, K20, K21, K22, K23, K24, K48, K49, K50, K51,    K52,   /* 13 keys */ \
      K25,  K26, K27, K28, K29, K30, K31, K32, K54, K55, K56,      K57,     /* 12 keys */ \
    K33,  K34,  K35,              K53,              K59,  K60,  K61,  K62   /* 8 keys */  \
) LAYOUT_all( \
    K01, K02, K03, K04, K05, K06, K07, K08, K36, K37, K38, K39, K41,  K40,  \
    K09,  K10, K11, K12, K13, K14, K15, K16, K42, K43, K44, K45, K47,  K46, \
     K17,  K18, K19, K20, K21, K22, K23, K24, K48, K49, K50, K51,    K52,   \
     K25, XXX, K26, K27, K28, K29, K30, K31, K32, K54, K55, K56,  K57, XXX, \
    K33,  K34,  K35,               K53,              K59,  K60,  K61,  K62, \
                                                        XXX, XXX, XXX, XXX  \
)

#define LAYOUT_60_ansi_split_lshift( \
    K01, K02, K03, K04, K05, K06, K07, K08, K36, K37, K38, K39, K41,  K40,  /* 14 keys */ \
    K09,  K10, K11, K12, K13, K14, K15, K16, K42, K43, K44, K45, K47,  K46, /* 14 keys */ \
     K17,  K18, K19, K20, K21, K22, K23, K24, K48, K49, K50, K51,    K52,   /* 13 keys */ \
    K25, K63, K26, K27, K28, K29, K30, K31, K32, K54, K55, K56,     K57,    /* 13 keys */ \
    K33,  K34,  K35,              K53,              K59,  K60,  K61,  K62   /* 8 keys */  \
) LAYOUT_all( \
    K01, K02, K03, K04, K05, K06, K07, K08, K36, K37, K38, K39, K41,  K40,  \
    K09,  K10, K11, K12, K13, K14, K15, K16, K42, K43, K44, K45, K47,  K46, \
     K17,  K18, K19, K20, K21, K22, K23, K24, K48, K49, K50, K51,    K52,   \
     K25, K63, K26, K27, K28, K29, K30, K31, K32, K54, K55, K56,  K57, XXX, \
    K33,  K34,  K35,               K53,              K59,  K60,  K61,  K62, \
                                                        XXX, XXX, XXX, XXX  \
)

#define LAYOUT_60_ansi_split_rshift( \
    K01, K02, K03, K04, K05, K06, K07, K08, K36, K37, K38, K39, K41,  K40,  /* 14 keys */ \
    K09,  K10, K11, K12, K13, K14, K15, K16, K42, K43, K44, K45, K47,  K46, /* 14 keys */ \
     K17,  K18, K19, K20, K21, K22, K23, K24, K48, K49, K50, K51,    K52,   /* 13 keys */ \
      K25,  K26, K27, K28, K29, K30, K31, K32, K54, K55, K56,    K57,  K58, /* 13 keys */ \
    K33,  K34,  K35,              K53,              K59,  K60,  K61,  K62   /* 8 keys */  \
) LAYOUT_all( \
    K01, K02, K03, K04, K05, K06, K07, K08, K36, K37, K38, K39, K41,  K40,  \
    K09,  K10, K11, K12, K13, K14, K15, K16, K42, K43, K44, K45, K47,  K46, \
     K17,  K18, K19, K20, K21, K22, K23, K24, K48, K49, K50, K51,    K52,   \
     K25, XXX, K26, K27, K28, K29, K30, K31, K32, K54, K55, K56,  K57, K58, \
    K33,  K34,  K35,               K53,              K59,  K60,  K61,  K62, \
                                                        XXX, XXX, XXX, XXX  \
)

#define LAYOUT_60_ansi_split_lshift_rshift( \
    K01, K02, K03, K04, K05, K06, K07, K08, K36, K37, K38, K39, K41,  K40,  /* 14 keys */ \
    K09,  K10, K11, K12, K13, K14, K15, K16, K42, K43, K44, K45, K47,  K46, /* 14 keys */ \
     K17,  K18, K19, K20, K21, K22, K23, K24, K48, K49, K50, K51,    K52,   /* 13 keys */ \
    K25, K63, K26, K27, K28, K29, K30, K31, K32, K54, K55, K56,  K57,  K58, /* 14 keys */ \
    K33,  K34,  K35,              K53,              K59,  K60,  K61,  K62   /* 8 keys */  \
) LAYOUT_all( \
    K01, K02, K03, K04, K05, K06, K07, K08, K36, K37, K38, K39, K41,  K40,  \
    K09,  K10, K11, K12, K13, K14, K15, K16, K42, K43, K44, K45, K47,  K46, \
     K17,  K18, K19, K20, K21, K22, K23, K24, K48, K49, K50, K51,    K52,   \
     K25, K63, K26, K27, K28, K29, K30, K31, K32, K54, K55, K56,  K57, K58, \
    K33,  K34,  K35,               K53,              K59,  K60,  K61,  K62, \
                                                        XXX, XXX, XXX, XXX  \
)

/*
 * Magic Key Options
 *
 * Magic keys are hotkey commands that allow control over firmware functions of
 * the keyboard. They are best used in combination with the HID Listen program,
 * found here: https://www.pjrc.com/teensy/hid_listen.html
 *
 * The options below allow the magic key functionality to be changed. This is
 * useful if your keyboard/keypad is missing keys and you want magic key support.
 *
 */

/* key combination for magic key command */
#define IS_COMMAND() ( \
    keyboard_report->mods == (MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT)) \
)

/* control how magic key switches layers */
//#define MAGIC_KEY_SWITCH_LAYER_WITH_FKEYS  true
//#define MAGIC_KEY_SWITCH_LAYER_WITH_NKEYS  true
//#define MAGIC_KEY_SWITCH_LAYER_WITH_CUSTOM false

/* override magic key keymap */
//#define MAGIC_KEY_SWITCH_LAYER_WITH_FKEYS
//#define MAGIC_KEY_SWITCH_LAYER_WITH_NKEYS
//#define MAGIC_KEY_SWITCH_LAYER_WITH_CUSTOM
//#define MAGIC_KEY_HELP1          H
//#define MAGIC_KEY_HELP2          SLASH
//#define MAGIC_KEY_DEBUG          D
//#define MAGIC_KEY_DEBUG_MATRIX   X
//#define MAGIC_KEY_DEBUG_KBD      K
//#define MAGIC_KEY_DEBUG_MOUSE    M
//#define MAGIC_KEY_VERSION        V
//#define MAGIC_KEY_STATUS         S
//#define MAGIC_KEY_CONSOLE        C
//#define MAGIC_KEY_LAYER0_ALT1    ESC
//#define MAGIC_KEY_LAYER0_ALT2    GRAVE
//#define MAGIC_KEY_LAYER0         0
//#define MAGIC_KEY_LAYER1         1
//#define MAGIC_KEY_LAYER2         2
//#define MAGIC_KEY_LAYER3         3
//#define MAGIC_KEY_LAYER4         4
//#define MAGIC_KEY_LAYER5         5
//#define MAGIC_KEY_LAYER6         6
//#define MAGIC_KEY_LAYER7         7
//#define MAGIC_KEY_LAYER8         8
//#define MAGIC_KEY_LAYER9         9
//#define MAGIC_KEY_BOOTLOADER     PAUSE
//#define MAGIC_KEY_LOCK           CAPS
//#define MAGIC_KEY_EEPROM         E
//#define MAGIC_KEY_NKRO           N
//#define MAGIC_KEY_SLEEP_LED      Z

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT
//#define NO_ACTION_MACRO
//#define NO_ACTION_FUNCTION

/*
 * MIDI options
 */

/* Prevent use of disabled MIDI features in the keymap */
//#define MIDI_ENABLE_STRICT 1

/* enable basic MIDI features:
   - MIDI notes can be sent when in Music mode is on
*/
//#define MIDI_BASIC

/* enable advanced MIDI features:
   - MIDI notes can be added to the keymap
   - Octave shift and transpose
   - Virtual sustain, portamento, and modulation wheel
   - etc.
*/
//#define MIDI_ADVANCED

/* override number of MIDI tone keycodes (each octave adds 12 keycodes and allocates 12 bytes) */
//#define MIDI_TONE_KEYCODE_OCTAVES 1

/* MBIA043 RGB Drivers */
#define MBIA043_NUM_CASCADE 2
/* MBIA pins */
#define MBIA043_GCLK_PIN C0
#define MBIA043_DCLK_PIN A14
#define MBIA043_SDI_PIN C2 /* data sent to first MBIA from MCU */
#define MBIA043_SDO_PIN B0 /* data sent to MCU from last MBIA */
#define MBIA043_LE_PIN A15
/* MBIA 3.3V power enable */
#define MBIA043_HAS_POWER_PIN
#define MBIA043_PWRCTRL_PIN C1

/* LED (Anode) Column GPIO Pins */
#define LED_COL_PINS \
    { C8, C7, B5, B4, B3, B2, C6, C5 }

#ifdef RGB_MATRIX_ENABLE
#    define DRIVER_LED_TOTAL 62
#    define RGB_MATRIX_KEYPRESSES
#    define RGB_MATRIX_FRAMEBUFFER_EFFECTS
#    define RGB_DISABLE_WHEN_USB_SUSPENDED
// RGB Matrix Animation modes. Explicitly enabled
// For full list of effects, see:
// https://docs.qmk.fm/#/feature_rgb_matrix?id=rgb-matrix-effects
#    define ENABLE_RGB_MATRIX_ALPHAS_MODS
#    define ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN
#    define ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
#    define ENABLE_RGB_MATRIX_BREATHING
#    define ENABLE_RGB_MATRIX_BAND_SAT
#    define ENABLE_RGB_MATRIX_BAND_VAL
#    define ENABLE_RGB_MATRIX_BAND_PINWHEEL_SAT
#    define ENABLE_RGB_MATRIX_BAND_PINWHEEL_VAL
#    define ENABLE_RGB_MATRIX_BAND_SPIRAL_SAT
#    define ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL
#    define ENABLE_RGB_MATRIX_CYCLE_ALL
#    define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
#    define ENABLE_RGB_MATRIX_CYCLE_UP_DOWN
#    define ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON
#    define ENABLE_RGB_MATRIX_CYCLE_OUT_IN
#    define ENABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL
#    define ENABLE_RGB_MATRIX_CYCLE_PINWHEEL
#    define ENABLE_RGB_MATRIX_CYCLE_SPIRAL
#    define ENABLE_RGB_MATRIX_DUAL_BEACON
#    define ENABLE_RGB_MATRIX_RAINBOW_BEACON
#    define ENABLE_RGB_MATRIX_RAINBOW_PINWHEELS
#    define ENABLE_RGB_MATRIX_RAINDROPS
#    define ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS
#    define ENABLE_RGB_MATRIX_HUE_BREATHING
#    define ENABLE_RGB_MATRIX_HUE_PENDULUM
#    define ENABLE_RGB_MATRIX_HUE_WAVE
#    define ENABLE_RGB_MATRIX_PIXEL_RAIN
#    define ENABLE_RGB_MATRIX_PIXEL_FLOW
#    define ENABLE_RGB_MATRIX_PIXEL_FRACTAL
// enabled only if RGB_MATRIX_FRAMEBUFFER_EFFECTS is defined
#    define ENABLE_RGB_MATRIX_TYPING_HEATMAP
#    define ENABLE_RGB_MATRIX_DIGITAL_RAIN
// enabled only of RGB_MATRIX_KEYPRESSES or RGB_MATRIX_KEYRELEASES is defined
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_WIDE
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_CROSS
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTICROSS
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_NEXUS
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS
#    define ENABLE_RGB_MATRIX_SPLASH
#    define ENABLE_RGB_MATRIX_MULTISPLASH
#    define ENABLE_RGB_MATRIX_SOLID_SPLASH
#    define ENABLE_RGB_MATRIX_SOLID_MULTISPLASH
#endif

#endif // CONFIG_H

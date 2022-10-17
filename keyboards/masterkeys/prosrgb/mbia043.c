// SDPX-License-Identifier: GPL-2.0-or-later
// Copyright 2022 (c) Hansem Ro

#include "hal.h"
#include "gpio.h"
#include "mbia043.h"
#include "quantum.h"

#ifdef MBIA043_DEBUG
#include "print.h"
#endif

#define RGB_LEVELS 10

#ifndef RGB_MATRIX_ENABLE
#define DRIVER_LED_TOTAL 87
typedef struct {
    uint8_t x;
    uint8_t y;
} led_point_t;

typedef struct {
    uint8_t matrix_co[MATRIX_ROWS][15];
    led_point_t point[DRIVER_LED_TOTAL];
    uint8_t flags[DRIVER_LED_TOTAL];
} led_config_t;
#endif

int BOUND = 87;
#ifdef RGB_MATRIX_ENABLE
#define NO_LED 255
led_config_t g_led_config = {
    { // Key Matrix to LED Index
{ 8,5,1,255,255,255,255,82,28,26,24,22,20,18,16 },
{ 9,6,2,72,255,255,85,57,41,39,37,35,33,31,29 },
{ 255,81,3,71,255,255,10,84,42,40,38,36,34,32,30 },
{ 255,255,255,73,255,255,11,77,56,54,52,50,48,46,44 },
{ 255,255,255,74,255,255,12,70,255,55,53,51,49,47,45 },
{ 255,255,13,75,255,255,86,80,78,68,66,64,62,60,58 },
{ 255,255,14,76,255,255,255,79,255,69,67,65,63,61,59 },
{ 7,4,0,255,255,255,83,43,27,25,23,21,19,17,15 },
    }, { // LED Index to Physical Position
        {  0,   0}, { 26,   0}, { 39,   0}, { 52,   0}, { 65,   0}, { 84,   0}, { 97,   0}, {110,   0}, {123,   0}, {143,   0}, {156,   0}, {169,   0}, {182,   0}, {198,   0}, {211,   0}, {224,   0},
        {  0,  17}, { 13,  17}, { 26,  17}, { 39,  17}, { 52,  17}, { 65,  17}, { 78,  17}, { 91,  17}, {104,  17}, {117,  17}, {130,  17}, {143,  17}, {156,  17}, {175,  17}, {198,  17}, {211,  17}, {224,  17},
        {  3,  29}, { 19,  29}, { 32,  29}, { 45,  29}, { 58,  29}, { 71,  29}, { 84,  29}, { 97,  29}, {110,  29}, {123,  29}, {136,  29}, {149,  29}, {162,  29}, {172,  29}, {198,  29}, {211,  29}, {224,  29},
        {  5,  41}, { 23,  41}, { 36,  41}, { 49,  41}, { 62,  41}, { 75,  41}, { 88,  41}, {101,  41}, {114,  41}, {127,  41}, {140,  41}, {153,  41}, {174,  41},
        {  8,  52}, { 29,  52}, { 42,  52}, { 55,  52}, { 68,  52}, { 81,  52}, { 94,  52}, {107,  52}, {120,  52}, {133,  52}, {146,  52}, {170,  52}, {211,  52},
        {  2,  64}, { 18,  64}, { 34,  64}, { 83,  64}, {131,  64}, {148,  64}, {164,  64}, {180,  64}, {198,  64}, {211,  64}, {224,  64},
    }, { // LED Index to Flag
        4,    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,    4,
        4,    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,    4,     4,
        4, 4, 4,          4,          4, 4, 4, 4,  4, 4, 4,
    }
};
#endif

typedef struct PACKED {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} mbia043_led_t;

#ifdef RGB_MATRIX_ENABLE
mbia043_led_t mbia043_leds[MATRIX_ROWS * MBIA043_NUM_CHANNELS];
#else
mbia043_led_t mbia043_leds[MATRIX_ROWS][MBIA043_NUM_CHANNELS];
#endif

static uint32_t LEDA_GPIO_ROW_PINS[MATRIX_ROWS] = {
    Q1_Ctrl,
    Q2_Ctrl,
    Q3_Ctrl,
    Q4_Ctrl,
    Q5_Ctrl,
    Q6_Ctrl,
    Q7_Ctrl,
    Q8_Ctrl,
};
static unsigned int LED_ROW_NUM = 0;

static void mbia043_reset_row_pins(void) {
    for (int i = 0; i < MATRIX_ROWS; i++) {
        setPinInput(LEDA_GPIO_ROW_PINS[i]);
    }
    return;
}

static inline void mbia043_write_color_row(int row) {
    writePinLow(MBIA043_LE_PIN);
#ifdef RGB_MATRIX_ENABLE
    for (int i = 0; i < MBIA043_NUM_CHANNELS - MATRIX_COLS; i++) {
        _mbia043_shift_data(0, 10);
        _mbia043_shift_data(0, 10);
        mbia043_shift_data_instr(0, 10, MBIA043_DATA_LATCH);
    }
    for (int i = 0; i < MATRIX_COLS; i++) {
        _mbia043_shift_data(mbia043_leds[g_led_config.matrix_co[row][i]].b << 8, 10);
        _mbia043_shift_data(mbia043_leds[g_led_config.matrix_co[row][i]].g << 8, 10);
        mbia043_shift_data_instr(mbia043_leds[g_led_config.matrix_co[row][i]].r << 8, 10, MBIA043_DATA_LATCH);
    }
#else
    for (int i = 0; i < MBIA043_NUM_CHANNELS; i++) {
        _mbia043_shift_data(mbia043_leds[row][i].b << 8, 10);
        _mbia043_shift_data(mbia043_leds[row][i].g << 8, 10);
        mbia043_shift_data_instr(mbia043_leds[row][i].r << 8, 10, MBIA043_DATA_LATCH);
    }
#endif
    writePinLow(MBIA043_SDI_PIN);
    writePinLow(MBIA043_DCLK_PIN);
    return;
}

#ifdef RGB_MATRIX_ENABLE
static void mbia043_set_color(int index, uint8_t r, uint8_t g, uint8_t b) {
    mbia043_leds[index].r = r;
    mbia043_leds[index].g = g;
    mbia043_leds[index].b = b;
    return;
}

static void mbia043_set_color_all(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < DRIVER_LED_TOTAL; i++) {
        mbia043_leds[i].r = r;
        mbia043_leds[i].g = g;
        mbia043_leds[i].b = b;
    }
    return;
}

static void mbia043_flush(void) { return; }

const rgb_matrix_driver_t rgb_matrix_driver = {
    .init = mbia043_init,
    .flush = mbia043_flush,
    .set_color = mbia043_set_color,
    .set_color_all = mbia043_set_color_all,
};
#endif

// BFTM1 callback routine
static void timer_callback(GPTDriver *gptp) {
    mbia043_reset_row_pins();
    wait_us(10);
    mbia043_send_instruction(MBIA043_GLOBAL_LATCH);
    setPinOutput(LEDA_GPIO_ROW_PINS[LED_ROW_NUM]);
    writePinLow(LEDA_GPIO_ROW_PINS[LED_ROW_NUM]);
    mbia043_write_color_row(LED_ROW_NUM);
    LED_ROW_NUM = (LED_ROW_NUM + 1) & 0x7;
    return;
}

static const GPTConfig BFTM1_config = {
    .frequency = HT32_CK_AHB_FREQUENCY,
    .callback = timer_callback,
};

static const PWMConfig GPTM1_config = {
    .frequency = HT32_CK_AHB_FREQUENCY,
    .period = (HT32_CK_AHB_FREQUENCY / 3600000) - 1,
    .callback = NULL,
    .channels = {
        [0] = {
            .mode = PWM_OUTPUT_ACTIVE_LOW,
            .callback = NULL,
        },
    },
};

void mbia043_init(void) {
#ifdef MBIA043_DEBUG
    printf("%s START\n", __func__);
#endif

    // configure MBIA pins
    setPinOutput(MBIA043_DCLK_PIN);
    setPinOutput(MBIA043_GCLK_PIN);
    setPinOutput(MBIA043_LE_PIN);
    setPinOutput(MBIA043_SDI_PIN);
    writePinHigh(MBIA043_DCLK_PIN);
    writePinHigh(MBIA043_LE_PIN);
    writePinHigh(MBIA043_SDI_PIN);
    setPinInput(MBIA043_SDO_PIN);

#ifdef MBIA043_HAS_POWER_PIN
    // power on MBIA
    setPinOutput(MBIA043_PWRCTRL_PIN);
    writePinHigh(MBIA043_PWRCTRL_PIN);
#endif

    // start/configure PWM (at GCLK pin)
    pwmStart(&PWMD_GPTM1, &GPTM1_config);
    writePinHigh(MBIA043_GCLK_PIN);
    palSetLineMode(MBIA043_GCLK_PIN, PAL_MODE_OUTPUT_PUSHPULL |
            PAL_MODE_HT32_AF(AFIO_TM));

    int len = 0;
    // wait until shift register becomes ready
    while (len != MBIA043_NUM_CASCADE * MBIA043_SHIFT_REGISTER_WIDTH) {
        len = mbia043_get_shift_register_length();
    }

    uint16_t mbia043_config[MBIA043_NUM_CASCADE] = { 0 };
    // read configuration
    mbia043_read_configuration(mbia043_config);
    // set configuration
    mbia043_config[0] = 0xc;
    mbia043_config[1] = 0xc;
    mbia043_config[2] = 0xc;
    mbia043_write_configuration(mbia043_config);
    // read configuration
    mbia043_read_configuration(mbia043_config);

    // TODO remove
    pwmEnableChannel(&PWMD_GPTM1, 0, PWM_FRACTION_TO_WIDTH(&PWMD_GPTM1, RGB_LEVELS, 5));
    gptStart(&GPTD_BFTM1, &BFTM1_config);
    if (GPTD_BFTM1.state == GPT_READY) {
        gptStartContinuous(&GPTD_BFTM1, 60000UL);
    }
#ifdef RGB_MATRIX_ENABLE
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < MATRIX_COLS; j++) {
            mbia043_leds[g_led_config.matrix_co[i][j]].r = 0;
            mbia043_leds[g_led_config.matrix_co[i][j]].g = 0;
            mbia043_leds[g_led_config.matrix_co[i][j]].b = 0;
        }
    }
    //for (int i = 0; i < MATRIX_COLS; i++) {
    //    mbia043_leds[g_led_config.matrix_co[0][i]].r = 0x2f;
    //    mbia043_leds[g_led_config.matrix_co[1][i]].g = 0x2f;
    //    mbia043_leds[g_led_config.matrix_co[2][i]].b = 0x0f;
    //    mbia043_leds[g_led_config.matrix_co[3][i]].r = 0x4f;
    //    mbia043_leds[g_led_config.matrix_co[4][i]].g = 0x4f;
    //    mbia043_leds[g_led_config.matrix_co[5][i]].b = 0x2f;
    //    mbia043_leds[g_led_config.matrix_co[6][i]].r = 0x6f;
    //    mbia043_leds[g_led_config.matrix_co[6][i]].b = 0x6f;
    //    mbia043_leds[g_led_config.matrix_co[7][i]].g = 0x6f;
    //    mbia043_leds[g_led_config.matrix_co[7][i]].b = 0x6f;
    //}
#else
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 16; j++) {
            mbia043_leds[i][j].r = 0;
            mbia043_leds[i][j].g = 0;
            mbia043_leds[i][j].b = 0;
        }
    }
    for (int i = 0; i < 16; i++) {
        mbia043_leds[0][i].r = 0x2f;
        mbia043_leds[1][i].g = 0x2f;
        mbia043_leds[2][i].b = 0x0f;
        mbia043_leds[3][i].r = 0x4f;
        mbia043_leds[4][i].g = 0x4f;
        mbia043_leds[5][i].b = 0x2f;
        mbia043_leds[6][i].r = 0x6f;
        mbia043_leds[6][i].b = 0x6f;
        mbia043_leds[7][i].g = 0x6f;
        mbia043_leds[7][i].b = 0x6f;
    }
#endif
#ifdef RGB_MATRIX_ENABLE
    for (int i = 0; i < BOUND/*DRIVER_LED_TOTAL;*/; i++) {
        mbia043_set_color(i, 0xff-(uint8_t)i, 0x5f, (uint8_t)i);
        wait_us(30000);
    }
    wait_us(30000);
    mbia043_set_color_all(0x3f, 0x00, 0x00);
#endif

#ifdef MBIA043_DEBUG
    printf("%s END\n", __func__);
#endif
    return;
}

// Send 'instr' number of DCLK pulses while LE is asserted high.
void inline _mbia043_send_instruction(int instr) {
    writePinLow(MBIA043_LE_PIN);
    __NOP();
    __NOP();
    __NOP();
    writePinHigh(MBIA043_LE_PIN);
    while (instr-- > 0) {
        __NOP();
        __NOP();
        __NOP();
        writePinLow(MBIA043_DCLK_PIN);
        __NOP();
        __NOP();
        __NOP();
        writePinHigh(MBIA043_DCLK_PIN);
    }
    writePinLow(MBIA043_LE_PIN);
    return;
}

// Send 'instr' number of DCLK pulses while LE is asserted high.
void mbia043_send_instruction(int instr) {
//#ifdef MBIA043_DEBUG
//    printf("%s(%d)\n", __func__, instr);
//#endif
    _mbia043_send_instruction(instr);
    return;
}

// Transmit data to shift-register with shift_amount number of DCLK pulses.
//
// Note: Transmission begins with MSB at data[15].
void inline _mbia043_shift_data(uint16_t data, int shift_amount) {
    while (shift_amount-- > 0) {
        __NOP();
        __NOP();
        __NOP();
        writePinLow(MBIA043_DCLK_PIN);
        // set SDI to data[15]
        writePin(MBIA043_SDI_PIN, data & 0x8000);
        __NOP();
        __NOP();
        __NOP();
        // clock in data
        writePinHigh(MBIA043_DCLK_PIN);
        data = (data & 0x7fff) << 1;
    }
    return;
}

// Transmit data to shift-register with shift_amount number of DCLK pulses.
//
// Note: Transmission begins with MSB at data[15].
void mbia043_shift_data(uint16_t data, int shift_amount) {
//#ifdef MBIA043_DEBUG
//    printf("%s(0x%04x, %d)\n", __func__, data, shift_amount);
//#endif
    _mbia043_shift_data(data, shift_amount);
    return;
}

// Transmit data to shift-register with shift_amount number of DCLK pulses,
// and assert LE for the last instr number of DCLK pulses.
//
// Note: Assumes instr is less than shift_amount.
// Note: Transmission begins with MSB at data[15].
void mbia043_shift_data_instr(uint16_t data, int shift_amount, int instr) {
#ifdef MBIA043_DEBUG
    printf("%s(0x%04x, %d, %d)\n", __func__, data, shift_amount, instr);
#endif
    if (instr < shift_amount) {
        writePinLow(MBIA043_LE_PIN);
        _mbia043_shift_data(data, shift_amount-instr);
        data = data << (shift_amount-instr);
        writePinHigh(MBIA043_LE_PIN);
        _mbia043_shift_data(data, instr);
        writePinLow(MBIA043_LE_PIN);
    }
    return;
}

// Transmit data to shift-register with shift_amount number of DCLK pulses,
// and read shift_amount bits of data from (last-in-cascade) shift-register.
//
// Note: Transmission begins with MSB at data[15].
uint16_t mbia043_shift_recv(uint16_t data, int shift_amount) {
#ifdef MBIA043_DEBUG
    printf("%s(0x%04x, %d)", __func__, data, shift_amount);
#endif
    uint16_t recv = 0;
    while (shift_amount-- > 0) {
        __NOP();
        __NOP();
        __NOP();
        writePinLow(MBIA043_DCLK_PIN);
        recv = (recv << 1) | readPin(MBIA043_SDO_PIN);
        // set SDI to data[15]
        writePin(MBIA043_SDI_PIN, data & 0x8000);
        __NOP();
        __NOP();
        __NOP();
        // clock in data
        writePinHigh(MBIA043_DCLK_PIN);
        data = (data & 0x7fff) << 1;
    }
#ifdef MBIA043_DEBUG
    printf(": %d\n", recv);
#endif
    return recv;
}

// Read configuration data from cascaded MBIA043s.
// Note: order of array follows order from SDO to SDI pins.
void mbia043_read_configuration(uint16_t *dst) {
#ifdef MBIA043_DEBUG
    printf("%s:\n", __func__);
#endif
    mbia043_send_instruction(MBIA043_READ_CONFIGURATION);
    for (int i = 0; i < MBIA043_NUM_CASCADE; i++) {
        dst[i] = mbia043_shift_recv(0, MBIA043_SHIFT_REGISTER_WIDTH) & 0x3ff;
#ifdef MBIA043_DEBUG
        printf("\tdst[%d]: 0x%04x\n", i, dst[i]);
#endif
    }
    return;
}

// Write configuration data to each MBIA043.
// Note: order of array follows order from SDO to SDI pins.
void mbia043_write_configuration(uint16_t *src) {
#ifdef MBIA043_DEBUG
    printf("%s:\n", __func__);
#endif
    mbia043_send_instruction(MBIA043_ENABLE_WRITE_CONFIGURATION);
    int i = 0;
    for (; i < MBIA043_NUM_CASCADE - 1; i++) {
        mbia043_shift_data(src[i] << 6, MBIA043_SHIFT_REGISTER_WIDTH);
#ifdef MBIA043_DEBUG
        printf("\tsrc[%d]: 0x%04x\n", i, src[i]);
#endif
    }
    if (i < MBIA043_NUM_CASCADE) {
        mbia043_shift_data_instr(src[i] << 6, MBIA043_SHIFT_REGISTER_WIDTH,
                MBIA043_WRITE_CONFIGURATION);
#ifdef MBIA043_DEBUG
        printf("\tsrc[%d]: 0x%04x\n", i, src[i]);
#endif
    }
    return;
}

#ifdef MBIA043_DEBUG
// Writes/read 10-bit data through shift-register(s)
void mbia043_test_shift_register(void) {
    printf("%s:\n", __func__);
    for (uint16_t i = 0; i < 0x400; i++) {
        uint16_t read = mbia043_shift_recv(i << 6, MBIA043_SHIFT_REGISTER_WIDTH);
        printf("\t%u: %u (%s)\n", i, read, i == read ? "Okay" : "Fail");
    }
    return;
}
#endif

// Find length of shift-register by clearing shift-register with 0s, writing
// with 1s, and checking how many DCLK pulses until a 1 is detected.
int mbia043_get_shift_register_length(void) {
#ifdef MBIA043_DEBUG
    printf("%s", __func__);
#endif
    int len = 0;
    writePinLow(MBIA043_LE_PIN);
    // clear shift register
    mbia043_shift_data(0, MBIA043_NUM_CASCADE * MBIA043_SHIFT_REGISTER_WIDTH);
    // write 1s until 1 appears on SDO
    int out = readPin(MBIA043_SDO_PIN);
    while (!out) {
        len++;
        _mbia043_shift_data(1U << 15, 1);
        out = readPin(MBIA043_SDO_PIN);
    }
#ifdef MBIA043_DEBUG
    printf(": %d\n", len);
#endif
    return len;
}

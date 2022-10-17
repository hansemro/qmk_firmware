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

static uint32_t LED_GPIO_PINS[8] = {
    Q1_Ctrl,
    Q2_Ctrl,
    Q3_Ctrl,
    Q4_Ctrl,
    Q5_Ctrl,
    Q6_Ctrl,
    Q7_Ctrl,
    Q8_Ctrl,
};
static unsigned int LED_PIN_NUM = 0;

static void led_gpio_pin_reset(void) {
    for (int i = 0; i < 8; i++) {
        setPinInput(LED_GPIO_PINS[i]);
    }
    return;
}

static inline void set_color_all(uint16_t red, uint16_t green, uint16_t blue) {
    writePinLow(MBIA043_LE_PIN);
    for (int i = 0; i < MBIA043_NUM_CHANNELS; i++) {
        _mbia043_shift_data(blue, 10);
        _mbia043_shift_data(green, 10);
        mbia043_shift_data_instr(red, 10, MBIA043_DATA_LATCH);
    }
    writePinLow(MBIA043_SDI_PIN);
    writePinLow(MBIA043_DCLK_PIN);
    return;
}

static int state = 0;
static int counter = 0;
static int c2 = 0;
static uint8_t _red = 0x00;
static uint8_t _green = 0x00;
static uint8_t _blue = 0x00;

// BFTM1 callback routine
static void timer_callback(GPTDriver *gptp) {
    led_gpio_pin_reset();
    wait_us(10);
    mbia043_send_instruction(MBIA043_GLOBAL_LATCH);
    setPinOutput(LED_GPIO_PINS[LED_PIN_NUM]);
    writePinLow(LED_GPIO_PINS[LED_PIN_NUM]);
    LED_PIN_NUM = (LED_PIN_NUM + 1) & 0x7;
    set_color_all(_red << 8, _green << 8, _blue << 8);
    if (counter >= 0x00000050) {
        switch (state) {
            case 0:
                if (_red < 0xff) {
                    _red += 0x1;
                } else {
                    _red = 0xff;
                    _green = 0x00;
                    _blue = 0x00;
                    state = 1;
                    printf("s1\n");
                }
                break;
            case 1:
                if (_red > 0x00) {
                    _red -= 0x1;
                } else {
                    _red = 0;
                    _green = 0;
                    _blue = 0;
                    state = 2;
                    printf("s2\n");
                }
                break;
            case 2:
                if (_green < 0xff) {
                    _green += 0x1;
                } else {
                    _red = 0;
                    _green = 0xff;
                    _blue = 0;
                    state = 3;
                    printf("s3\n");
                }
                break;
            case 3:
                if (_green > 0x00) {
                    _green -= 0x1;
                } else {
                    _red = 0;
                    _green = 0;
                    _blue = 0;
                    state = 4;
                    printf("s4\n");
                }
                break;
            case 4:
                if (_blue < 0x40) {
                    if (c2 <= 0x1) {
                        c2++;
                    } else {
                        _blue += 0x1;
                        //printf("b%d\n", _blue);
                        c2 = 0;
                    }
                } else {
                    _red = 0;
                    _green = 0;
                    _blue = 0x40;
                    state = 5;
                    printf("s5\n");
                }
                break;
            case 5:
                if (_blue > 0x00) {
                    if (c2 <= 0x0) {
                        c2++;
                    } else {
                        _blue -= 0x1;
                        //printf("b5%d\n", _blue);
                        c2 = 0;
                    }
                } else {
                    _red = 0;
                    _green = 0;
                    _blue = 0;
                    state = 0;
                    printf("s0\n");
                }
                break;
            default:
                _red = 0;
                _green = 0;
                _blue = 0;
                state = 0;
        }
        //_red -= 0x1800;
        //_green -= 0x2000;
        //_blue -= 0x1000;
        counter = 0;
    } else {
        counter++;
    }
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

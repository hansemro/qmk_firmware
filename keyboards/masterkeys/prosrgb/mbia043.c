// Copyright (c) 2022 Hansem Ro <hansemro@outlook.com
// SPDX-License-Identifier: GPL-2.0-or-later

#include "hal.h"
#include "mbia043.h"
#ifdef RGB_MATRIX_ENABLE
#include "rgb_matrix.h"

led_config_t g_led_config = {
    { // Key Matrix to LED Index
        { 15, 17, 19, 21, 23, 25, 27,     43, 83,     NO_LED, NO_LED, NO_LED, 0,      4,      7},
        { 16, 18, 20, 22, 24, 26, 28,     82, NO_LED, NO_LED, NO_LED, NO_LED, 1,      5,      8},
        { 29, 31, 33, 35, 37, 39, 41,     57, 85,     NO_LED, NO_LED, 72,     2,      6,      9},
        { 30, 32, 34, 36, 38, 40, 42,     84, 10,     NO_LED, NO_LED, 71,     3,      81,     NO_LED},
        { 44, 46, 48, 50, 52, 54, 56,     77, 11,     NO_LED, NO_LED, 73,     NO_LED, NO_LED, NO_LED},
        { 45, 47, 49, 51, 53, 55, NO_LED, 70, 12,     NO_LED, NO_LED, 74,     NO_LED, NO_LED, NO_LED},
        { 58, 60, 62, 64, 66, 68, 78,     80, 86,     NO_LED, NO_LED, 75,     13,     NO_LED, NO_LED},
        { 59, 61, 63, 65, 67, 69, NO_LED, 79, NO_LED, NO_LED, NO_LED, 76,     14,     NO_LED, NO_LED},
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
        1,    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,    1,     4,
        1, 1, 1,          4,          1, 1, 1, 1,  4, 4, 4,
    }
};

#endif

#include "print.h"

bool mbia043_initialized = false;

static void _delay(int n) {
    n = n * 20;
    while (n--) {
        __NOP();
    }
    return;
}

// GCLK @ channel 0 of GPTM1
// 3.6 MHz PWM signal
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

static int counter = 15;
static uint16_t number = 0x7ff;

// BFTM1 timer interrupt callback
static void timer_callback(GPTDriver *gptp) {
    //printf("%s START\n", __func__);
    // deactivate Qn pins
    //_mbia043_deactivate_row_pins();
    //_delay(10);
    // send overall latch instruction
    //mbia043_send_instruction(MBIA043_INSTR_OVERALL_LATCH);
    // set MBIA_LE pin high
    //palSetLine(MBIA_LE_PIN);
    // set mbia043_row pin direction to out
    // set mbia043_row pin low
    // increment mbia043_row (wrap to 0 when mbia043_row == MBIA_NUM_ROWS)
    //mbia043_row++;
    // shift RGB data to MBIA
    //mbia043_update_RGB_buffers();
    //printf("%s mbia043_row: %d\n", __func__, mbia043_row);

    return;
}

static const GPTConfig BFTM1_config = {
    .frequency = HT32_CK_AHB_FREQUENCY,
    .callback = timer_callback,
};

uint8_t mbia043_shift_register_length(void) {
    uint8_t len = 0;
    palClearLine(MBIA_LE_PIN);
    palSetLine(MBIA_SDI_PIN);
    palClearLine(MBIA_DCLK_PIN);
    _delay(10);
    palSetLine(MBIA_DCLK_PIN);
    _delay(10);
    uint32_t out = palReadLine(MBIA_SDO_PIN);
    while (!out) {
        len++;
        palClearLine(MBIA_DCLK_PIN);
        out = palReadLine(MBIA_SDO_PIN);
        _delay(10);
        palSetLine(MBIA_DCLK_PIN);
        _delay(10);
    }

    return len;
}

void gpio_reset(void) {
    GPIOB->DIRCR &= ~(0xf << 2);
    GPIOB->INER |= (0xf << 2);
    GPIOB->PUR &= ~(0xf << 2);
    GPIOB->PDR &= ~(0xf << 2);
    GPIOC->DIRCR &= ~(0xf << 5);
    GPIOC->INER |= (0xf << 5);
    GPIOC->PUR &= ~(0xf << 5);
    GPIOC->PDR &= ~(0xf << 5);
    return;
}

void mbia043_init(void) {
    printf("%s START\n", __func__);
    // enable AFIO clock
    CKCU->APBCCR0 |= CKCU_APBCCR0_AFIOEN;
    // enable clocks for GPIO A,B,C
    CKCU->AHBCCR |= 7 << 16;
    // setup MBIA GPIO pins
    GPIOA->INER &= ~(3 << 14); // A14/MBIA_DCLK & A15/MBIA_LE
    GPIOA->PUR &= ~(3 << 14); // A14/MBIA_DCLK & A15/MBIA_LE
    GPIOA->PDR &= ~(3 << 14); // A14/MBIA_DCLK & A15/MBIA_LE
    GPIOC->SRR |= 3 << 1; // C1/MBIA_PWRCTRL & C2/MBIA_SDI
    GPIOC->ODR &= ~(3 << 1); // C1/MBIA_PWRCTRL & C2/MBIA_SDI
    GPIOC->DIRCR |= 3 << 1; // C1/MBIA_PWRCTRL & C2/MBIA_SDI
    //GPIOA->SRR |= 3 << 14; // A14/MBIA_DCLK & A15/MBIA_LE
    GPIOA->ODR &= ~(3 << 14); // A14/MBIA_DCLK & A15/MBIA_LE
    GPIOA->DIRCR |= 3 << 14; // A14/MBIA_DCLK & A15/MBIA_LE
    GPIOB->INER |= 1 << 0; // B0/MBIA_SDO
    mbia043_initialized = true;

    // setup GPTM1...
    //  enable GPTM1 clock
    //  config GPTM1 PWM
    //   frequency = CK_AHB
    //   period = (CK_AHB/3,600,000) - 1
    //   active low (with PWM mode 1)
    // start/enable GPTM1
    pwmStart(&PWMD_GPTM1, &GPTM1_config);
    // no pull resistor on MBIA_GCLK
    GPIOC->PUR &= ~(1 << 0); // C0/MBIA_GCLK
    GPIOC->PDR &= ~(1 << 0); // C0/MBIA_GCLK
    // set MBIA_GCLK AFIO to AF4/GT_CH0
    AFIO->GPCCFGR[0] |= (AFIO_TM << 0);
    // enable channel 0 on GPTM1 with width = period / 2
    pwmEnableChannel(&PWMD_GPTM1, 0, GPTM1_config.period / 2);
    // setup BFTM1...
    //  enable BFTM1 clock
    //  set BFTM1 compare value to 60,000
    //  set BFTM1 counter to 0
    //  enable BFTM1
    //   frequency = CK_AHB
    //   period = 60,000 ticks
    //  enable BFTM1 compare match interrupt
    //  enable BFTM1 interrupt (IRQ42)
    gptStart(&GPTD_BFTM1, &BFTM1_config);
#ifdef RGB_MATRIX_ENABLE
    _mbia043_reset();
#else
    number = 0x7ff;
    palClearLine(MBIA_LE_PIN);
    _delay(1000);
    // start BFTM1 continous
    if (GPTD_BFTM1.state == GPT_READY) {
        // 60,000 ticks
        gptStartContinuous(&GPTD_BFTM1, 60000UL);
    }

    for (int i = 0; i < 1024; i++) {
        mbia043_shift_data(0, 10);
        mbia043_shift_data(0, 10);
        mbia043_shift_data(0, 10);
    }
    mbia043_send_instruction(0x12);
    mbia043_shift_data(0x300, 10);
    mbia043_shift_data(0x300, 10);
    mbia043_shift_data(0x0, 2);
    palClearLine(MBIA_DCLK_PIN);
    palSetLine(MBIA_LE_PIN);
    mbia043_shift_data(0xc00, 8);
    palClearLine(MBIA_SDI_PIN);
    palClearLine(MBIA_LE_PIN);
    _delay(10);
    // input direction
    // enable input
    // no pull resistor
    gpio_reset();


    while (true) {
        gpio_reset();
        GPIOC->DIRCR |= (1 << 5);
        GPIOC->RR |= (1 << 5);
        for (int i = 0; i < 16; i++) {
            mbia043_shift_data(0xaaaa, 10);
            mbia043_shift_data(0xaaaa, 10);
            mbia043_shift_data(0xaaaa, 10);
            _delay(100);
            mbia043_send_instruction(MBIA043_INSTR_OVERALL_LATCH);
            _delay(100);
        }
        _delay(100);

        gpio_reset();
        GPIOC->DIRCR |= (2 << 5);
        GPIOC->RR |= (2 << 5);
        for (int i = 0; i < 16; i++) {
            mbia043_shift_data(0xaaaa, 10);
            mbia043_shift_data(0xaaaa, 10);
            mbia043_shift_data(0xaaaa, 10);
            _delay(100);
            mbia043_send_instruction(MBIA043_INSTR_OVERALL_LATCH);
            _delay(100);
        }
        _delay(100);

        gpio_reset();
        GPIOC->DIRCR |= (4 << 5);
        GPIOC->RR |= (4 << 5);
        for (int i = 0; i < 16; i++) {
            mbia043_shift_data(0xaaaa, 10);
            mbia043_shift_data(0xaaaa, 10);
            mbia043_shift_data(0xaaaa, 10);
            _delay(100);
            mbia043_send_instruction(MBIA043_INSTR_OVERALL_LATCH);
            _delay(100);
        }
    }

    mbia043_shift_data(0, 10);
    mbia043_shift_data(0, 10);
    mbia043_shift_data(0, 10);
    counter = mbia043_shift_register_length();
    printf("%u\n", counter);
    mbia043_shift_data(0, 10);
    mbia043_shift_data(0, 10);
    mbia043_shift_data(0, 10);
    counter = mbia043_shift_register_length();
    printf("%u\n", counter);
    mbia043_shift_data(0, 10);
    mbia043_shift_data(0, 10);
    mbia043_shift_data(0, 10);
    counter = mbia043_shift_register_length();
    printf("%u\n", counter);

    //number = 0x7ff;
    //while (counter-- > 0) {
    //    uint16_t out = mbia043_shift_recv(number, 10);
    //    printf("%d: out: %u\n", counter, out);
    //    number = number >> 1;
    //}
#endif
    printf("%s END\n", __func__);

    return;
}

void mbia043_send_instruction(uint32_t instr) {
    if (mbia043_initialized) {
        palClearLine(MBIA_LE_PIN);
        _delay(10);
        // assert MBIA_LE high
        palSetLine(MBIA_LE_PIN);
        _delay(10);
        // instruction number of DCLK rising edges while MBIA_LE is asserted
        while (instr--) {
            palClearLine(MBIA_DCLK_PIN);
            _delay(10);
            palSetLine(MBIA_DCLK_PIN);
            _delay(10);
        }
        // deassert MBIA_LE
        palClearLine(MBIA_LE_PIN);
    }
    return;
}

void mbia043_shift_data(uint16_t value, uint16_t shift_amount) {
    if (mbia043_initialized) {
        while (shift_amount--) {
            __NOP();
            _delay(10);
            // deassert MBIA_DCLK
            palClearLine(MBIA_DCLK_PIN);
            _delay(10);
            // set MBIA_SDI to value_be[15] bit
            if ((value << 0x10) == 1) {
                palSetLine(MBIA_SDI_PIN);
            } else {
                palClearLine(MBIA_SDI_PIN);
            }
            __NOP();
            _delay(10);
            // shift bit to shift register
            palSetLine(MBIA_DCLK_PIN);
            // shift value_be
            value = (value & 0x7fff) << 1;
        }
    }
    return;
}

uint16_t mbia043_shift_recv(uint16_t value, uint16_t shift_amount) {
    uint16_t recv = 0;
    if (mbia043_initialized) {
        while (shift_amount--) {
            __NOP();
            _delay(10);
            palClearLine(MBIA_DCLK_PIN);
            printf("%ld\n", palReadLine(MBIA_SDO_PIN));
            //recv = (recv << 1) | palReadLine(MBIA_SDO_PIN);
            _delay(10);
            if (value & 0x200) {
                palSetLine(MBIA_SDI_PIN);
            } else {
                palClearLine(MBIA_SDI_PIN);
            }
            _delay(10);
            palSetLine(MBIA_DCLK_PIN);
            value = (value & 0x1ff) << 1;
        }
        __NOP();
        __NOP();
        _delay(10);
        //recv = ((recv << 1) | palReadLine(MBIA_SDO_PIN)) & 0xfff;
        //recv = palReadLine(MBIA_SDO_PIN);
        printf("%ld\n", palReadLine(MBIA_SDO_PIN));
    }
    return recv;
}

#ifdef RGB_MATRIX_ENABLE

mbia043_led_t mbia043_rgb_matrix[MATRIX_ROWS][MBIA_NUM_CHANNELS];
bool mbia043_needs_flush = true;
// current row number
uint8_t mbia043_row = 0;

void _mbia043_reset(void) {
    _mbia043_deactivate_row_pins();
    GPIOC->SRR |= 1 << 2; // C2/MBIA_SDI
    GPIOC->ODR &= ~(1 << 2); // C2/MBIA_SDI
    GPIOC->DIRCR |= 1 << 2; // C2/MBIA_SDI
    GPIOA->SRR |= 3 << 14; // A14/MBIA_DCLK & A15/MBIA_LE
    GPIOA->ODR &= ~(3 << 14); // A14/MBIA_DCLK & A15/MBIA_LE
    GPIOA->DIRCR |= 3 << 14; // A14/MBIA_DCLK & A15/MBIA_LE
    _delay(10000);
    mbia043_row = 0;
    for (int i = 0; i < MATRIX_ROWS; i++) {
        memset(mbia043_rgb_matrix[i], 0xffff, MATRIX_COLS * sizeof(mbia043_led_t));
    }
    _delay(100);
    mbia043_update_RGB_buffers();
    mbia043_send_instruction(MBIA043_INSTR_OVERALL_LATCH);
    GPIOA->SRR |= 1 << 15; // A15/MBIA_LE
    // start GPTM1 timer if not already
    if (PWMD_GPTM1.state != PWM_READY) {
        pwmStart(&PWMD_GPTM1, &GPTM1_config);
    }
    // start BFTM1 continous
    if (GPTD_BFTM1.state == GPT_READY) {
        // 60,000 ticks
        gptStartContinuous(&GPTD_BFTM1, 60000UL);
    }
    // no pull resistor on MBIA_GCLK
    GPIOC->PUR &= ~(1 << 0); // C0/MBIA_GCLK
    GPIOC->PDR &= ~(1 << 0); // C0/MBIA_GCLK
    // set MBIA_GCLK AFIO to AF4/GT_CH0
    AFIO->GPCCFGR[0] |= (AFIO_TM << 0);

    return;
}

void _mbia043_deactivate_row_pins(void) {
    //for (int row = 0; row < MATRIX_ROWS; row++) {
    //    // input direction
    //    // enable input
    //    // no pull resistor
    //}
    // Q1
    // Q2
    // Q3
    // Q4
    // Q5
    // Q6
    // Q7
    // Q8
    return;
}

void mbia043_update_RGB_buffers(void) {
    for (int i = 0; i < MBIA_NUM_CHANNELS; i++) {
        // Blue channel
        mbia043_shift_data(mbia043_rgb_matrix[mbia043_row][i].blue<<2, 10);
        // Green channel
        mbia043_shift_data(mbia043_rgb_matrix[mbia043_row][i].green<<2, 10);
        // Red channel
        mbia043_shift_data(mbia043_rgb_matrix[mbia043_row][i].red<<2, 10);
    }

    return;
}

void mbia043_flush(void) {
    if (!mbia043_needs_flush) {
        return;
    }

    mbia043_update_RGB_buffers();
    mbia043_needs_flush = false;
    return;
}

void mbia043_set_color(int index, uint8_t red, uint8_t green, uint8_t blue) {
    mbia043_needs_flush = true;
    // encode to 10-bit big endian
    return;
}

void mbia043_set_color_all(uint8_t red, uint8_t green, uint8_t blue) {
    return;
}

const rgb_matrix_driver_t rgb_matrix_driver = {
    .init = mbia043_init,
    .flush = mbia043_flush,
    .set_color = mbia043_set_color,
    .set_color_all = mbia043_set_color_all,
};

#endif

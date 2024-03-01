# LED Matrix Lighting :id=led-matrix-lighting

This feature allows you to use LED matrices driven by external drivers. It hooks into the backlight system so you can use the same keycodes as backlighting to control it.

If you want to use RGB LED's you should use the [RGB Matrix Subsystem](feature_rgb_matrix.md) instead.

## Driver configuration :id=driver-configuration
---
### IS31FL3731 :id=is31fl3731

There is basic support for addressable LED matrix lighting with the I2C IS31FL3731 LED controller. To enable it, add this to your `rules.mk`:

```make
LED_MATRIX_ENABLE = yes
LED_MATRIX_DRIVER = is31fl3731
```

You can use between 1 and 4 IS31FL3731 IC's. Do not specify `LED_DRIVER_ADDR_<N>` defines for IC's that are not present on your keyboard. You can define the following items in `config.h`:

| Variable | Description | Default |
|----------|-------------|---------|
| `IS31FL3731_I2C_TIMEOUT` | (Optional) How long to wait for i2c messages, in milliseconds | 100 |
| `IS31FL3731_I2C_PERSISTENCE` | (Optional) Retry failed messages this many times | 0 |
| `LED_MATRIX_LED_COUNT` | (Required) How many LED lights are present across all drivers | |
| `IS31FL3731_I2C_ADDRESS_1` | (Required) Address for the first LED driver | |
| `IS31FL3731_I2C_ADDRESS_2` | (Optional) Address for the second LED driver | |
| `IS31FL3731_I2C_ADDRESS_3` | (Optional) Address for the third LED driver | |
| `IS31FL3731_I2C_ADDRESS_4` | (Optional) Address for the fourth LED driver | |

Here is an example using 2 drivers.

```c
// This is a 7-bit address, that gets left-shifted and bit 0
// set to 0 for write, 1 for read (as per I2C protocol)
// The address will vary depending on your wiring:
// 00 AD <-> GND
// 01 AD <-> SCL
// 10 AD <-> SDA
// 11 AD <-> VCC
// ADDR represents A1:A0 of the 7-bit address.
// The result is: 0b11101(ADDR)
#define IS31FL3731_I2C_ADDRESS_1 IS31FL3731_I2C_ADDRESS_GND
#define IS31FL3731_I2C_ADDRESS_2 IS31FL3731_I2C_ADDRESS_SDA

#define LED_DRIVER_1_LED_TOTAL 25
#define LED_DRIVER_2_LED_TOTAL 24
#define LED_MATRIX_LED_COUNT (LED_DRIVER_1_LED_TOTAL + LED_DRIVER_2_LED_TOTAL)
```

!> Note the parentheses, this is so when `LED_MATRIX_LED_COUNT` is used in code and expanded, the values are added together before any additional math is applied to them. As an example, `rand() % (LED_DRIVER_1_LED_TOTAL + LED_DRIVER_2_LED_TOTAL)` will give very different results than `rand() % LED_DRIVER_1_LED_TOTAL + LED_DRIVER_2_LED_TOTAL`.

For split keyboards using `LED_MATRIX_SPLIT` with an LED driver, you can either have the same driver address or different driver addresses. If using different addresses, use `IS31FL3731_I2C_ADDRESS_1` for one and `IS31FL3731_I2C_ADDRESS_2` for the other one. Then, in `g_is31fl3731_leds`, fill out the correct driver index (0 or 1). If using one address, use `IS31FL3731_I2C_ADDRESS_1` for both, and use index 0 for `g_is31fl3731_leds`.

Define these arrays listing all the LEDs in your `<keyboard>.c`:

```c
const is31fl3731_led_t PROGMEM g_is31fl3731_leds[IS31FL3731_LED_COUNT] = {
/* Refer to IS31 manual for these locations
 *    driver
 *    |  LED address
 *    |  | */
    { 0, C1_1  },
    { 0, C1_15 },
    // ...
}
```

Where `Cx_y` is the location of the LED in the matrix defined by [the datasheet](https://www.issi.com/WW/pdf/31FL3731.pdf) and the header file `drivers/led/issi/is31fl3731-mono.h`. The `driver` is the index of the driver you defined in your `config.h` (`0`, `1`, `2`, or `3` ).

---
### IS31FLCOMMON :id=is31flcommon

There is basic support for addressable LED matrix lighting with a selection of I2C ISSI Lumissil LED controllers through a shared common driver. To enable it, add this to your `rules.mk`:

```makefile
LED_MATRIX_ENABLE = yes
LED_MATRIX_DRIVER = <driver name>
```

Where `<driver name>` is the applicable LED driver chip as below

| Driver Name | Data Sheet | Capability |
|-------------|------------|------------|
| `IS31FL3742A` | [datasheet](https://www.lumissil.com/assets/pdf/core/IS31FL3742A_DS.pdf) | 180 LED, 30x6 Matrix |
| `IS31FL3743A` | [datasheet](https://www.lumissil.com/assets/pdf/core/IS31FL3743A_DS.pdf) | 198 LED, 18x11 Matrix |
| `IS31FL3745` | [datasheet](https://www.lumissil.com/assets/pdf/core/IS31FL3745_DS.pdf) | 144 LED, 18x8 Matrix |
| `IS31FL3746A` | [datasheet](https://www.lumissil.com/assets/pdf/core/IS31FL3746A_DS.pdf) | 72 LED, 18x4 Matrix |

You can use between 1 and 4 IC's. Do not specify `DRIVER_ADDR_<N>` define for IC's if not present on your keyboard. The `DRIVER_ADDR_1` default assumes that all Address pins on the controller have been connected to GND. Drivers that have SYNC functionality have the default settings to disable if 1 driver. If more than 1 drivers then `DRIVER_ADDR_1` will be set to Master and the remaiing ones set to Slave.

Configure the hardware via your `config.h`:

| Variable | Description | Default |
|----------|-------------|---------|
| `ISSI_TIMEOUT` | (Optional) How long to wait for i2c messages, in milliseconds | 100 |
| `ISSI_PERSISTENCE` | (Optional) Retry failed messages this many times | 0 |
| `LED_MATRIX_LED_COUNT` | (Required) How many LED lights are present across all drivers | |
| `DRIVER_ADDR_1` | (Optional) Address for the first LED driver | |
| `DRIVER_ADDR_<N>` | (Required) Address for the additional LED drivers | |
| `ISSI_SSR_<N>` | (Optional) Configuration for the Spread Spectrum Register | |
| `ISSI_CONFIGURATION` | (Optional) Configuration for the Configuration Register | |
| `ISSI_GLOBALCURRENT` | (Optional) Configuration for the Global Current Register | 0xFF |
| `ISSI_PULLDOWNUP` | (Optional) Configuration for the Pull Up & Pull Down Register | |
| `ISSI_TEMP` | (Optional) Configuration for the Tempature Register | |
| `ISSI_PWM_ENABLE` | (Optional) Configuration for the PWM Enable Register | |
| `ISSI_PWM_SET` | (Optional) Configuration for the PWM Setting Register | |
| `ISSI_SCAL_LED ` | (Optional) Configuration for the LEDs Scaling Registers | 0xFF |
| `ISSI_MANUAL_SCALING` | (Optional) If you wish to configure the Scaling Registers manually | |


Defaults

| Variable | IS31FL3742A | IS31FL3743A | IS31FL3745 | IS31FL3746 |
|----------|-------------|-------------|------------|------------|
| `DRIVER_ADDR_1` | 0b0110000 | 0b0100000 | 0b0100000 | 0b1100000 |
| `ISSI_SSR_1` | 0x00 | 0x00 / 0x60 | 0x00 / 0xC0 | 0x00 |
| `ISSI_SSR_<2-4>` | 0x00 | 0x40 | 0x80 | 0x00 |
| `ISSI_CONFIGURATION` | 0x31 | 0x01 | 0x31 | 0x01 |
| `ISSI_PULLDOWNUP` | 0x55 | 0x33 | 0x33 | 0x33 |
| `ISSI_TEMP` | N/A | 0x00 | 0x00 | 0x00 |
| `ISSI_PWM_ENABLE` | N/A | N/A | N/A | 0x00 |
| `ISSI_PWM_SET` | 0x00 | N/A | N/A | 0x00 |

Here is an example using 2 drivers.

```c
#define DRIVER_ADDR_2 0b0100001

#define DRIVER_1_LED_TOTAL 66
#define DRIVER_2_LED_TOTAL 42
#define LED_MATRIX_LED_COUNT (DRIVER_1_LED_TOTAL + DRIVER_2_LED_TOTAL)
```
!> Note the parentheses, this is so when `LED_MATRIX_LED_COUNT` is used in code and expanded, the values are added together before any additional math is applied to them. As an example, `rand() % (DRIVER_1_LED_TOTAL + DRIVER_2_LED_TOTAL)` will give very different results than `rand() % DRIVER_1_LED_TOTAL + DRIVER_2_LED_TOTAL`.

Currently only 4 drivers are supported, but it would be trivial to support for more. Note that using a combination of different drivers is not supported. All drivers must be of the same model.

Define these arrays listing all the LEDs in your `<keyboard>.c`:

```c
const is31_led PROGMEM g_is31_leds[LED_MATRIX_LED_COUNT] = {
/* Refer to IS31 manual for these locations
 *    driver
 *    |  LED address
 *    |  | */
    { 0, SW1_CS1 },
    { 0, SW1_CS2 },
    // ...
}
```

Where `CSx_SWx` is the location of the LED in the matrix defined by the datasheet. The `driver` is the index of the driver you defined in your `config.h` (`0`, `1`, `2`, or `3` for now).

`ISSI_MANUAL_SCALING` is used to override the Scaling for individual LED's. By default they will be set as per `ISSI_SCAL_LED`. In `config.h` set how many LED's you want to manually set scaling for.
Eg `#define ISSI_MANUAL_SCALING 3`

Then Define the array listing all the LEDs you want to override in your `<keyboard>.c`:

```c
const is31_led PROGMEM g_is31_scaling[ISSI_MANUAL_SCALING] = {
/*   LED Index
 *   |  Scaling
 *   |  | */
    {5, 120},
    {9, 120},
    ....
}
```

Where LED Index is the position of the LED in the `g_is31_leds` array. The `scaling` value between 0 and 255 to be written to the Scaling Register.

---
### MBI :id=mbi


There is basic support for monochromatic LED matrix lighting with the Macroblock MBIA04X/MBI504X LED Driver with proprietary SPI-like control interface. To enable it, add this to your `rules.mk`:

```make
LED_MATRIX_ENABLE = yes
LED_MATRIX_DRIVER = mbi
```

You can use 1 or more chained MBI IC's. You can define the following items in `config.h`:

| Variable | Description | Default |
|----------|-------------|---------|
| `MBI_NUM_CHANNELS` | (Required) Number of output channels per MBI | |
| `MBI_SHIFT_REG_WIDTH` | (Required) Length of shift register per MBI, in bits | |
| `MBI_DATA_LATCH` | (Required) Number of DCLK rising edges with LE asserted to transfer data from shift register to buffers. | |
| `MBI_GLOBAL_LATCH` | (Required) Number of DCLK rising edges with LE asserted to transfer data from buffers to comparators. | |
| `MBI_CONFIGURATION` | (Optional) Value to set each MBI configuration register. | |
| `MBI_WRITE_CONFIGURATION` | (Optional) Number of DCLK rising edges with LE asserted to transfer data from shift register to configuration register (if writing to configuration register is enabled). | |
| `MBI_ENABLE_WRITE_CONFIGURATION` | (Optional) Number of DCLK rising edges with LE asserted to enable writing to configuration register. | |
| `MBI_NOPS` | (Required) Number of no-op delays (multiplied by 3) to ensure MBI timing requirements. Not required to be defined for some platforms. | |
| `MBI_NUM_DRIVER` | (Required) Number of chained MBI drivers | |
| `MBI_LED_GPIO_PINS` | (Required) Array of MCU-managed LED GPIO pins in order of row/column index | |
| `MBI_LED_GPIO_OUTPUT_MODE` | (Optional) GPIO pad mode for LED GPIO pins | `PAL_MODE_OUTPUT_PUSHPULL` |
| `MBI_LED_DIRECTION` | (Required) Direction from MCU-managed LED pin to output channel of MBI. Accepted values: `ROW2COL` or `COL2ROW` | |
| `MBI_LED_GPIO_ACTIVE_STATE` | Enable MCU-managed LED pin on high/low signal. Accepted values: `HIGH` or `LOW` | |
| `MBI_NUM_LED_GPIO_PINS` | (Required) Number of LED GPIO pins | |
| `MBI_PWM_DRIVER` | (Required) PWM driver to use for generating GCLK clock signal | |
| `MBI_PWM_CHANNEL` | (Required) PWM channel to use | |
| `MBI_PWM_OUTPUT_MODE` | (Optional) PWM driver output mode | `PWM_OUTPUT_ACTIVE_LOW` |
| `MBI_PWM_COUNTER_FREQUENCY` | (Optional) PWM counter frequency, in Hz | 8000000 |
| `MBI_PWM_PERIOD` | (Optional) PWM period in ticks. Output frequency = `MBI_PWM_COUNTER_FREQUENCY` / `MBI_PWM_PERIOD` | 2 |
| `MBI_TIMER_DRIVER` | (Required) GPT timer driver to use for continuous flushing of a row/column color data | |
| `MBI_TIMER_COUNTER_FREQUENCY` | (Optional) Timer counter frequency, in Hz | 2000 |
| `MBI_TIMER_PERIOD` | (Optional) Timer period in ticks. LED refresh rate = `MBI_TIMER_COUNTER_FREQUENCY` / (`MBI_TIMER_PERIOD` * `MBI_NUM_LED_GPIO_PINS`) | 2 |
| `MBI_LE_PIN` | (Required) MCU-managed LE pin | |
| `MBI_LE_OUTPUT_MODE` | (Optional) GPIO pad mode for LE pin | `PAL_MODE_OUTPUT_PUSHPULL` |
| `MBI_SDI_PIN` | (Required) MCU-managed SDI pin | |
| `MBI_SDI_OUTPUT_MODE` | (Optional) GPIO pad mode for SDI pin | `PAL_MODE_OUTPUT_PUSHPULL` |
| `MBI_DCLK_PIN` | (Required) MCU-managed DCLK pin | |
| `MBI_DCLK_OUTPUT_MODE` | (Optional) GPIO pad mode for DCLK pin | `PAL_MODE_OUTPUT_PUSHPULL` |
| `MBI_GCLK_PIN` | (Required) MCU-managed GCLK pin | |
| `MBI_GCLK_OUTPUT_MODE` | (Optional) GPIO pad mode for GCLK pin | `PAL_MODE_OUTPUT_PUSHPULL` |
| `MBI_POWER_ENABLE_PIN` | (Optional) MCU-managed pin for enabling power to MBI(s) | |
| `MBI_POWER_OUTPUT_MODE` | GPIO pad mode for power enable pin | `PAL_MODE_OUTPUT_PUSHPULL` |
| `MBI_POWER_ACTIVE_STATE` | Enable power on high/low signal. Accepted values: `HIGH` or `LOW` | |

Here is an example using a single MBI5042 driver.

```c
// MBI5042 (at 5V)
#define MBI_NUM_CHANNELS 16
#define MBI_SHIFT_REG_WIDTH 10
#define MBI_DATA_LATCH 1
#define MBI_GLOBAL_LATCH 3
#define MBI_WRITE_CONFIGURATION 11
#define MBI_ENABLE_WRITE_CONFIGURATION 15

// MBI configuration
#define MBI_CONFIGURATION 0x12c

#define MBI_NUM_DRIVER 1
// Insert no-ops to meet timing requirements
#define MBI_NOPS 2

// PWM to generate GCLK clock signal
// Desired output/GCLK frequency =  3.7 MHz
#define MBI_PWM_DRIVER PWMD_GPTM1
#define MBI_PWM_CHANNEL 0
#define MBI_PWM_OUTPUT_MODE PWM_OUTPUT_ACTIVE_LOW
#define MBI_PWM_PERIOD 2UL
#define MBI_PWM_COUNTER_FREQUENCY (3700000UL * MBI_PWM_PERIOD)

// MBI timer for flushing color data for a single column
#define MBI_TIMER_DRIVER GPTD_BFTM0
// Default: 120 Hz LED refresh rate
// #define MBI_TIMER_PERIOD 2UL
// #define MBI_TIMER_COUNTER_FREQUENCY (120UL * MBI_NUM_LED_GPIO_PINS * MBI_TIMER_PERIOD)

// MCU manages row pins; MBI's manage column pins
#define MBI_LED_DIRECTION ROW2COL
// MCU-managed LED row pins
#define MBI_LED_GPIO_PINS \
    { C15, C14, B5, B4, B3, B2, D3, D2 }
#define MBI_NUM_LED_GPIO_PINS 8
#define MBI_LED_GPIO_OUTPUT_MODE (PAL_MODE_OUTPUT_OPENDRAIN | PAL_MODE_HT32_AF(AFIO_GPIO))
#define MBI_LED_GPIO_ACTIVE_STATE LOW

// MCU-managed MBI pins
// These pins connect to pull-up resistor to 5V, so use open-drain.
#define MBI_LE_PIN A15
#define MBI_LE_OUTPUT_MODE (PAL_MODE_OUTPUT_OPENDRAIN | PAL_MODE_HT32_AF(AFIO_GPIO))
#define MBI_SDI_PIN C12
#define MBI_SDI_OUTPUT_MODE (PAL_MODE_OUTPUT_OPENDRAIN | PAL_MODE_HT32_AF(AFIO_GPIO))
#define MBI_DCLK_PIN A14
#define MBI_DCLK_OUTPUT_MODE (PAL_MODE_OUTPUT_OPENDRAIN | PAL_MODE_HT32_AF(AFIO_GPIO))
#define MBI_GCLK_PIN C10
#define MBI_GCLK_OUTPUT_MODE (PAL_MODE_OUTPUT_OPENDRAIN | PAL_MODE_HT32_AF(AFIO_TM))

// MCU-managed MBI 5V power supply enable (active-low)
#define MBI_POWER_ENABLE_PIN C11
#define MBI_POWER_ACTIVE_STATE LOW
#define MBI_POWER_OUTPUT_MODE (PAL_MODE_OUTPUT_OPENDRAIN | PAL_MODE_HT32_AF(AFIO_GPIO))
```

!> Note that this driver requires an LED matrix to LED index table in addition to the standard Key matrix to LED index table. This is to account for Key coordinate mapping differences or additional LEDs.

!> Note that the LED entries in `rgb_matrix.layout` in `info.json` are specified with Key matrix coordinates instead of LED matrix coordinates.

?> Acknowledge that the LED index for a key allows the LED matrix to map to the Key matrix, but the value itself is arbitrary. By extension, the order of LED entries in `rgb_matrix.layout` in `info.json` is also arbitrary. However, make sure that the entry index (which is also the LED index) remains consistent with the index in the LED matrix to LED index table.

Define channel and LED matrix configuration in your `<keyboard>.c`:

```c
#include "mbi.h"

/*
 * Channel Setup:
 *         ┌───────┐
 *    SDI->│       │->SDO
 *   COL0<-│       │->unused
 *   COL1<-│       │->unused
 *   COL2<-│       │->COL13
 *   COL3<-│MBI5042│->COL12
 *   COL4<-│   A   │->COL11
 *   COL5<-│       │->COL10
 *   COL6<-│       │->COL9
 *   COL7<-│       │->COL8
 *         └───────┘
 */
// clang-format off
const mbi_channel_t g_mbi_channels[MBI_NUM_DRIVER][MBI_NUM_CHANNELS] = {
    /* MBI A */
    {
    /*    Color channel (either MONO or UNUSED)
     *    |
     *    |            MBI-managed row/column index
     *    |            | */
        { MBI_MONO_CH, 0 }, /* MBI output channel 0 */
        { MBI_MONO_CH, 1 }, /* MBI output channel 1 */
        { MBI_MONO_CH, 2 }, /* ... */
        { MBI_MONO_CH, 3 },
        { MBI_MONO_CH, 4 },
        { MBI_MONO_CH, 5 },
        { MBI_MONO_CH, 6 },
        { MBI_MONO_CH, 7 },
        { MBI_MONO_CH, 8 },
        { MBI_MONO_CH, 9 },
        { MBI_MONO_CH, 10 },
        { MBI_MONO_CH, 11 },
        { MBI_MONO_CH, 12 },
        { MBI_MONO_CH, 13 },
        { MBI_UNUSED_CH, 0 },
        { MBI_UNUSED_CH, 0 }, /* MBI output channel 15 */
    },
};
// clang-format on

#ifndef NO_LED
#    define NLD 255
#else
#    define NLD NO_LED
#endif

// LED Matrix to LED Index
// Since ROW2COL, MCU manages rows while MBI manages columns
// clang-format off
const uint8_t g_mbi_led_matrix_co[MBI_NUM_LED_GPIO_PINS][MBI_NUM_CHANNELS] = {
    /*          Col0 Col1 Col2 Col3 Col4 Col5 Col6 Col7 Col8 Col9 Col10 Col11 Col12 Col13 Col14 Col15 */
    /*Row0*/ {  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,   11,   12,   26,   NLD,  NLD },
    /*Row1*/ {  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,   24,   25,   NLD,  NLD,  NLD },
    /*Row2*/ {  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,   42,   43,   44,   NLD,  NLD },
    /*Row3*/ {  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,   60,   NLD,  61,   NLD,  NLD },
    /*Row4*/ {  65,  NLD, 66,  67,  68,  69,  70,  71,  72,  73,  74,   75,   NLD,  76,   NLD,  NLD },
    /*Row5*/ {  81,  82,  83,  NLD, NLD, NLD, 84,  NLD, NLD, 85,  86,   87,   88,   27,   NLD,  NLD },
    /*Row6*/ {  89,  90,  77,  78,  62,  63,  45,  46,  28,  NLD, NLD,  NLD,  NLD,  NLD,  NLD,  NLD },
    /*Row7*/ {  91,  80,  79,  64,  NLD, 48,  47,  29,  30,  NLD, NLD,  92,   93,   94,   NLD,  NLD },
};
// clang-format on
```

Initialization functions (`mbi_init_pins`, `mbi_init_config`, `mbi_init_timers`, and `mbi_init`) may be overriden in your `<keyboard>.c`:

```c
void mbi_init_pins(void) {
    // initialize pins
    ...
}

void mbi_init_config(void) {
    // set MBI configuration register
    ...
}

void mbi_init_timers(void) {
    // setup and start GCLK signal
    ...
    // setup timer with callback to mbi_flush_isr()
    ...
}
```

---

## Common Configuration :id=common-configuration

From this point forward the configuration is the same for all the drivers. The `led_config_t` struct provides a key electrical matrix to led index lookup table, what the physical position of each LED is on the board, and what type of key or usage the LED if the LED represents. Here is a brief example:

```c
led_config_t g_led_config = { {
  // Key Matrix to LED Index
  {   5, NO_LED, NO_LED,   0 },
  { NO_LED, NO_LED, NO_LED, NO_LED },
  {   4, NO_LED, NO_LED,   1 },
  {   3, NO_LED, NO_LED,   2 }
}, {
  // LED Index to Physical Position
  { 188,  16 }, { 187,  48 }, { 149,  64 }, { 112,  64 }, {  37,  48 }, {  38,  16 }
}, {
  // LED Index to Flag
  1, 4, 4, 4, 4, 1
} };
```

The first part, `// Key Matrix to LED Index`, tells the system what key this LED represents by using the key's electrical matrix row & col. The second part, `// LED Index to Physical Position` represents the LED's physical `{ x, y }` position on the keyboard. The default expected range of values for `{ x, y }` is the inclusive range `{ 0..224, 0..64 }`. This default expected range is due to effects that calculate the center of the keyboard for their animations. The easiest way to calculate these positions is imagine your keyboard is a grid, and the top left of the keyboard represents `{ x, y }` coordinate `{ 0, 0 }` and the bottom right of your keyboard represents `{ 224, 64 }`. Using this as a basis, you can use the following formula to calculate the physical position:

```c
x = 224 / (NUMBER_OF_COLS - 1) * COL_POSITION
y =  64 / (NUMBER_OF_ROWS - 1) * ROW_POSITION
```

Where NUMBER_OF_COLS, NUMBER_OF_ROWS, COL_POSITION, & ROW_POSITION are all based on the physical layout of your keyboard, not the electrical layout.

As mentioned earlier, the center of the keyboard by default is expected to be `{ 112, 32 }`, but this can be changed if you want to more accurately calculate the LED's physical `{ x, y }` positions. Keyboard designers can implement `#define LED_MATRIX_CENTER { 112, 32 }` in their config.h file with the new center point of the keyboard, or where they want it to be allowing more possibilities for the `{ x, y }` values. Do note that the maximum value for x or y is 255, and the recommended maximum is 224 as this gives animations runoff room before they reset.

`// LED Index to Flag` is a bitmask, whether or not a certain LEDs is of a certain type. It is recommended that LEDs are set to only 1 type.

## Flags :id=flags

|Define                      |Value |Description                                      |
|----------------------------|------|-------------------------------------------------|
|`HAS_FLAGS(bits, flags)`    |*n/a* |Evaluates to `true` if `bits` has all `flags` set|
|`HAS_ANY_FLAGS(bits, flags)`|*n/a* |Evaluates to `true` if `bits` has any `flags` set|
|`LED_FLAG_NONE`             |`0x00`|If this LED has no flags                         |
|`LED_FLAG_ALL`              |`0xFF`|If this LED has all flags                        |
|`LED_FLAG_MODIFIER`         |`0x01`|If the LED is on a modifier key                  |
|`LED_FLAG_KEYLIGHT`         |`0x04`|If the LED is for key backlight                  |
|`LED_FLAG_INDICATOR`        |`0x08`|If the LED is for keyboard state indication      |

## Keycodes :id=keycodes

|Key                            |Aliases  |Description                        |
|-------------------------------|---------|-----------------------------------|
|`QK_LED_MATRIX_ON`             |`LM_ON`  |Turn on LED Matrix                 |
|`QK_LED_MATRIX_OFF`            |`LM_OFF` |Turn off LED Matrix                |
|`QK_LED_MATRIX_TOGGLE`         |`LM_TOGG`|Toggle LED Matrix on or off        |
|`QK_LED_MATRIX_MODE_NEXT`      |`LM_NEXT`|Cycle through animations           |
|`QK_LED_MATRIX_MODE_PREVIOUS`  |`LM_PREV`|Cycle through animations in reverse|
|`QK_LED_MATRIX_BRIGHTNESS_UP`  |`LM_BRIU`|Increase the brightness level      |
|`QK_LED_MATRIX_BRIGHTNESS_DOWN`|`LM_BRID`|Decrease the brightness level      |
|`QK_LED_MATRIX_SPEED_UP`       |`LM_SPDU`|Increase the animation speed       |
|`QK_LED_MATRIX_SPEED_DOWN`     |`LM_SPDD`|Decrease the animation speed       |

## LED Matrix Effects :id=led-matrix-effects

These are the effects that are currently available:

```c
enum led_matrix_effects {
    LED_MATRIX_NONE = 0,
    LED_MATRIX_SOLID = 1,           // Static single val, no speed support
    LED_MATRIX_ALPHAS_MODS,         // Static dual val, speed is val for LEDs marked as modifiers
    LED_MATRIX_BREATHING,           // Cycling brightness animation
    LED_MATRIX_BAND,                // Band fading brightness scrolling left to right
    LED_MATRIX_BAND_PINWHEEL,       // 3 blade spinning pinwheel fades brightness
    LED_MATRIX_BAND_SPIRAL,         // Spinning spiral fades brightness
    LED_MATRIX_CYCLE_LEFT_RIGHT,    // Full gradient scrolling left to right
    LED_MATRIX_CYCLE_UP_DOWN,       // Full gradient scrolling top to bottom
    LED_MATRIX_CYCLE_OUT_IN,        // Full gradient scrolling out to in
    LED_MATRIX_DUAL_BEACON,         // Full gradient spinning around center of keyboard
    LED_MATRIX_SOLID_REACTIVE_SIMPLE,   // Pulses keys hit then fades out
    LED_MATRIX_SOLID_REACTIVE_WIDE,       // Value pulses near a single key hit then fades out
    LED_MATRIX_SOLID_REACTIVE_MULTIWIDE,  // Value pulses near multiple key hits then fades out
    LED_MATRIX_SOLID_REACTIVE_CROSS,      // Value pulses the same column and row of a single key hit then fades out
    LED_MATRIX_SOLID_REACTIVE_MULTICROSS, // Value pulses the same column and row of multiple key hits then fades out
    LED_MATRIX_SOLID_REACTIVE_NEXUS,      // Value pulses away on the same column and row of a single key hit then fades out
    LED_MATRIX_SOLID_REACTIVE_MULTINEXUS, // Value pulses away on the same column and row of multiple key hits then fades out
    LED_MATRIX_SOLID_SPLASH,             // Value pulses away from a single key hit then fades out
    LED_MATRIX_SOLID_MULTISPLASH,        // Value pulses away from multiple key hits then fades out
    LED_MATRIX_WAVE_LEFT_RIGHT,           // Sine wave scrolling from left to right
    LED_MATRIX_WAVE_UP_DOWN,              // Sine wave scrolling from up to down
    LED_MATRIX_EFFECT_MAX
};
```

You can enable a single effect by defining `ENABLE_[EFFECT_NAME]` in your `config.h`:


|Define                                                 |Description                                   |
|-------------------------------------------------------|----------------------------------------------|
|`#define ENABLE_LED_MATRIX_ALPHAS_MODS`                |Enables `LED_MATRIX_ALPHAS_MODS`              |
|`#define ENABLE_LED_MATRIX_BREATHING`                  |Enables `LED_MATRIX_BREATHING`                |
|`#define ENABLE_LED_MATRIX_BAND`                       |Enables `LED_MATRIX_BAND`                     |
|`#define ENABLE_LED_MATRIX_BAND_PINWHEEL`              |Enables `LED_MATRIX_BAND_PINWHEEL`            |
|`#define ENABLE_LED_MATRIX_BAND_SPIRAL`                |Enables `LED_MATRIX_BAND_SPIRAL`              |
|`#define ENABLE_LED_MATRIX_CYCLE_LEFT_RIGHT`           |Enables `LED_MATRIX_CYCLE_LEFT_RIGHT`         |
|`#define ENABLE_LED_MATRIX_CYCLE_UP_DOWN`              |Enables `LED_MATRIX_CYCLE_UP_DOWN`            |
|`#define ENABLE_LED_MATRIX_CYCLE_OUT_IN`               |Enables `LED_MATRIX_CYCLE_OUT_IN`             |
|`#define ENABLE_LED_MATRIX_DUAL_BEACON`                |Enables `LED_MATRIX_DUAL_BEACON`              |
|`#define ENABLE_LED_MATRIX_WAVE_LEFT_RIGHT`            |Enables `LED_MATRIX_WAVE_LEFT_RIGHT`          |
|`#define ENABLE_LED_MATRIX_WAVE_UP_DOWN`               |Enables `LED_MATRIX_WAVE_UP_DOWN`             |

|Reactive Defines                                       |Description                                   |
|-------------------------------------------------------|----------------------------------------------|
|`#define ENABLE_LED_MATRIX_SOLID_REACTIVE_SIMPLE`      |Enables `LED_MATRIX_SOLID_REACTIVE_SIMPLE`    |
|`#define ENABLE_LED_MATRIX_SOLID_REACTIVE_WIDE`        |Enables `LED_MATRIX_SOLID_REACTIVE_WIDE`      |
|`#define ENABLE_LED_MATRIX_SOLID_REACTIVE_MULTIWIDE`   |Enables `LED_MATRIX_SOLID_REACTIVE_MULTIWIDE` |
|`#define ENABLE_LED_MATRIX_SOLID_REACTIVE_CROSS`       |Enables `LED_MATRIX_SOLID_REACTIVE_CROSS`     |
|`#define ENABLE_LED_MATRIX_SOLID_REACTIVE_MULTICROSS`  |Enables `LED_MATRIX_SOLID_REACTIVE_MULTICROSS`|
|`#define ENABLE_LED_MATRIX_SOLID_REACTIVE_NEXUS`       |Enables `LED_MATRIX_SOLID_REACTIVE_NEXUS`     |
|`#define ENABLE_LED_MATRIX_SOLID_REACTIVE_MULTINEXUS`  |Enables `LED_MATRIX_SOLID_REACTIVE_MULTINEXUS`|
|`#define ENABLE_LED_MATRIX_SOLID_SPLASH`               |Enables `LED_MATRIX_SOLID_SPLASH`             |
|`#define ENABLE_LED_MATRIX_SOLID_MULTISPLASH`          |Enables `LED_MATRIX_SOLID_MULTISPLASH`        |

?> These modes introduce additional logic that can increase firmware size.

## Custom LED Matrix Effects :id=custom-led-matrix-effects

By setting `LED_MATRIX_CUSTOM_USER` (and/or `LED_MATRIX_CUSTOM_KB`) in `rules.mk`, new effects can be defined directly from userspace, without having to edit any QMK core files.

To declare new effects, create a new `led_matrix_user/kb.inc` that looks something like this:

`led_matrix_user.inc` should go in the root of the keymap directory.
`led_matrix_kb.inc` should go in the root of the keyboard directory.

To use custom effects in your code, simply prepend `LED_MATRIX_CUSTOM_` to the effect name specified in `LED_MATRIX_EFFECT()`. For example, an effect declared as `LED_MATRIX_EFFECT(my_cool_effect)` would be referenced with:

```c
led_matrix_mode(led_MATRIX_CUSTOM_my_cool_effect);
```

```c
// !!! DO NOT ADD #pragma once !!! //

// Step 1.
// Declare custom effects using the LED_MATRIX_EFFECT macro
// (note the lack of semicolon after the macro!)
LED_MATRIX_EFFECT(my_cool_effect)
LED_MATRIX_EFFECT(my_cool_effect2)

// Step 2.
// Define effects inside the `LED_MATRIX_CUSTOM_EFFECT_IMPLS` ifdef block
#ifdef LED_MATRIX_CUSTOM_EFFECT_IMPLS

// e.g: A simple effect, self-contained within a single method
static bool my_cool_effect(effect_params_t* params) {
  LED_MATRIX_USE_LIMITS(led_min, led_max);
  for (uint8_t i = led_min; i < led_max; i++) {
    led_matrix_set_value(i, 0xFF);
  }
  return led_matrix_check_finished_leds(led_max);
}

// e.g: A more complex effect, relying on external methods and state, with
// dedicated init and run methods
static uint8_t some_global_state;
static void my_cool_effect2_complex_init(effect_params_t* params) {
  some_global_state = 1;
}
static bool my_cool_effect2_complex_run(effect_params_t* params) {
  LED_MATRIX_USE_LIMITS(led_min, led_max);
  for (uint8_t i = led_min; i < led_max; i++) {
    led_matrix_set_value(i, some_global_state++);
  }
  return led_matrix_check_finished_leds(led_max);
}
static bool my_cool_effect2(effect_params_t* params) {
  if (params->init) my_cool_effect2_complex_init(params);
  return my_cool_effect2_complex_run(params);
}

#endif // LED_MATRIX_CUSTOM_EFFECT_IMPLS
```

For inspiration and examples, check out the built-in effects under `quantum/led_matrix/animations/`.


## Additional `config.h` Options :id=additional-configh-options

```c
#define LED_MATRIX_KEYRELEASES // reactive effects respond to keyreleases (instead of keypresses)
#define LED_MATRIX_TIMEOUT 0 // number of milliseconds to wait until led automatically turns off
#define LED_MATRIX_SLEEP // turn off effects when suspended
#define LED_MATRIX_LED_PROCESS_LIMIT (LED_MATRIX_LED_COUNT + 4) / 5 // limits the number of LEDs to process in an animation per task run (increases keyboard responsiveness)
#define LED_MATRIX_LED_FLUSH_LIMIT 16 // limits in milliseconds how frequently an animation will update the LEDs. 16 (16ms) is equivalent to limiting to 60fps (increases keyboard responsiveness)
#define LED_MATRIX_MAXIMUM_BRIGHTNESS 255 // limits maximum brightness of LEDs
#define LED_MATRIX_DEFAULT_ON true // Sets the default enabled state, if none has been set
#define LED_MATRIX_DEFAULT_MODE LED_MATRIX_SOLID // Sets the default mode, if none has been set
#define LED_MATRIX_DEFAULT_VAL LED_MATRIX_MAXIMUM_BRIGHTNESS // Sets the default brightness value, if none has been set
#define LED_MATRIX_DEFAULT_SPD 127 // Sets the default animation speed, if none has been set
#define LED_MATRIX_DEFAULT_FLAGS LED_FLAG_ALL // Sets the default LED flags, if none has been set
#define LED_MATRIX_SPLIT { X, Y }   // (Optional) For split keyboards, the number of LEDs connected on each half. X = left, Y = Right.
                                    // If reactive effects are enabled, you also will want to enable SPLIT_TRANSPORT_MIRROR
```

## EEPROM storage :id=eeprom-storage

The EEPROM for it is currently shared with the RGB Matrix system (it's generally assumed only one feature would be used at a time).

### Direct Operation :id=direct-operation
|Function                                    |Description  |
|--------------------------------------------|-------------|
|`led_matrix_set_value_all(v)`         |Set all of the LEDs to the given value, where `v` is between 0 and 255 (not written to EEPROM) |
|`led_matrix_set_value(index, v)`      |Set a single LED to the given value, where `v` is between 0 and 255, and `index` is between 0 and `LED_MATRIX_LED_COUNT` (not written to EEPROM) |

### Disable/Enable Effects :id=disable-enable-effects
|Function                                    |Description  |
|--------------------------------------------|-------------|
|`led_matrix_toggle()`                       |Toggle effect range LEDs between on and off |
|`led_matrix_toggle_noeeprom()`              |Toggle effect range LEDs between on and off (not written to EEPROM) |
|`led_matrix_enable()`                       |Turn effect range LEDs on, based on their previous state |
|`led_matrix_enable_noeeprom()`              |Turn effect range LEDs on, based on their previous state (not written to EEPROM) |
|`led_matrix_disable()`                      |Turn effect range LEDs off, based on their previous state |
|`led_matrix_disable_noeeprom()`             |Turn effect range LEDs off, based on their previous state (not written to EEPROM) |

### Change Effect Mode :id=change-effect-mode
|Function                                    |Description  |
|--------------------------------------------|-------------|
|`led_matrix_mode(mode)`                     |Set the mode, if LED animations are enabled |
|`led_matrix_mode_noeeprom(mode)`            |Set the mode, if LED animations are enabled (not written to EEPROM) |
|`led_matrix_step()`                         |Change the mode to the next LED animation in the list of enabled LED animations |
|`led_matrix_step_noeeprom()`                |Change the mode to the next LED animation in the list of enabled LED animations (not written to EEPROM) |
|`led_matrix_step_reverse()`                 |Change the mode to the previous LED animation in the list of enabled LED animations |
|`led_matrix_step_reverse_noeeprom()`        |Change the mode to the previous LED animation in the list of enabled LED animations (not written to EEPROM) |
|`led_matrix_increase_speed()`               |Increase the speed of the animations |
|`led_matrix_increase_speed_noeeprom()`      |Increase the speed of the animations (not written to EEPROM) |
|`led_matrix_decrease_speed()`               |Decrease the speed of the animations |
|`led_matrix_decrease_speed_noeeprom()`      |Decrease the speed of the animations (not written to EEPROM) |
|`led_matrix_set_speed(speed)`               |Set the speed of the animations to the given value where `speed` is between 0 and 255 |
|`led_matrix_set_speed_noeeprom(speed)`      |Set the speed of the animations to the given value where `speed` is between 0 and 255 (not written to EEPROM) |

### Change Value :id=change-value
|Function                                    |Description  |
|--------------------------------------------|-------------|
|`led_matrix_increase_val()`                 |Increase the value for effect range LEDs. This wraps around at maximum value |
|`led_matrix_increase_val_noeeprom()`        |Increase the value for effect range LEDs. This wraps around at maximum value (not written to EEPROM) |
|`led_matrix_decrease_val()`                 |Decrease the value for effect range LEDs. This wraps around at minimum value |
|`led_matrix_decrease_val_noeeprom()`        |Decrease the value for effect range LEDs. This wraps around at minimum value (not written to EEPROM) |

### Query Current Status :id=query-current-status
|Function                         |Description                |
|---------------------------------|---------------------------|
|`led_matrix_is_enabled()`        |Gets current on/off status |
|`led_matrix_get_mode()`          |Gets current mode          |
|`led_matrix_get_val()`           |Gets current val           |
|`led_matrix_get_speed()`         |Gets current speed         |
|`led_matrix_get_suspend_state()` |Gets current suspend state |

## Callbacks :id=callbacks

### Indicators :id=indicators

If you want to set custom indicators, such as an LED for Caps Lock, or layer indication, then you can use the `led_matrix_indicators_kb` function on the keyboard level source file, or `led_matrix_indicators_user` function in the user `keymap.c`.
```c
bool led_matrix_indicators_kb(void) {
    if (!led_matrix_indicators_user()) {
        return false;
    }
    led_matrix_set_value(index, value);
    return true;
}
```

In addition, there are the advanced indicator functions.  These are aimed at those with heavily customized displays, where rendering every LED per cycle is expensive.  This includes a special macro to help make this easier to use: `LED_MATRIX_INDICATOR_SET_VALUE(i, v)`.

```c
void led_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    LED_MATRIX_INDICATOR_SET_VALUE(index, value);
    return false;
}
```

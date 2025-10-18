//
// Created by marko on 10/18/25.
//

#ifndef PIED_PIPER_DISPLAY_H
#define PIED_PIPER_DISPLAY_H

#include "hardware/i2c.h"

enum class TM1650_BRIGHTNESS : uint8_t
{
    BRIGHTNESS_8 = 0,
    BRIGHTNESS_1 = 1,
    BRIGHTNESS_2 = 2,
    BRIGHTNESS_3 = 3,
    BRIGHTNESS_4 = 4,
    BRIGHTNESS_5 = 5,
    BRIGHTNESS_6 = 6,
    BRIGHTNESS_7 = 7
};

enum class TM1650_POS : uint8_t
{
    POS_0 = 0,
    POS_1 = 1,
    POS_2 = 2,
    POS_3 = 3,
};

enum class TM1650_SYMBOL : uint8_t
{
    SYM_0  = 0x3F,
    SYM_1  = 0x06,
    SYM_2  = 0x5B,
    SYM_3  = 0x4F,
    SYM_4  = 0x66,
    SYM_5  = 0x6D,
    SYM_6  = 0x7D,
    SYM_7  = 0x07,
    SYM_8  = 0x7F,
    SYM_9  = 0x6F,
    SYM_A  = 0x77,
    SYM_B  = 0x7C,
    SYM_C  = 0x39,
    SYM_D  = 0x5E,
    SYM_E  = 0x79,
    SYM_F  = 0x71,
    SYM_R = ((1 << 4) | (1 << 6)),
    SYM_H = 0x76,
    SYM_L = 0x38,
};

void tm1650_set_brightness(i2c_inst * i2c, TM1650_BRIGHTNESS brightness);
void tm1650_write_raw(i2c_inst * i2c, TM1650_POS pos, uint8_t data);
void tm1650_reset(i2c_inst * i2c);
void tm1650_write_sym(i2c_inst * i2c, TM1650_POS pos, TM1650_SYMBOL sym, bool dot = false);
TM1650_SYMBOL tm1650_number_to_symbol(uint8_t num);
void tm1650_write_float(i2c_inst * i2c, float num, uint8_t precision);

#endif //PIED_PIPER_DISPLAY_H
//
// Created by marko on 8/12/25.
//
#include "display.h"
#include "hardware/i2c.h"
#include "math.h"

#define TM1650_CONF_REG (0x48 >> 1)
#define TM1650_DATA_REG (0x68 >> 1)


void tm1650_set_brightness(i2c_inst * i2c, TM1650_BRIGHTNESS brightness)
{
    auto data = static_cast<uint8_t>(static_cast<uint8_t>(brightness) << 4 | 1);
    i2c_write_blocking(i2c, TM1650_CONF_REG, &data, 1, false);
}

void tm1650_reset(i2c_inst * i2c)
{
    for (int i=0; i <= 3; i++)
    {
        tm1650_write_raw(i2c, static_cast<TM1650_POS>(i), 0);
    }
}

void tm1650_write_raw(i2c_inst * i2c, TM1650_POS pos, uint8_t data)
{
    uint8_t reg = TM1650_DATA_REG + static_cast<uint8_t>(pos);
    i2c_write_blocking(i2c, reg, &data, 1, false);
}

inline uint8_t with_dp(TM1650_SYMBOL s)
{
    constexpr uint8_t TM1650_DP_MASK = 0x80;
    return static_cast<uint8_t>(s) | TM1650_DP_MASK;
}

void tm1650_write_sym(i2c_inst * i2c, TM1650_POS pos, TM1650_SYMBOL sym, bool dot)
{
    uint8_t sym_data = (!dot) ? static_cast<uint8_t>(sym) : with_dp(sym);
    tm1650_write_raw(i2c, pos, sym_data);
}

TM1650_SYMBOL tm1650_number_to_symbol(uint8_t num)
{
    static constexpr TM1650_SYMBOL symbols[16] = {
        TM1650_SYMBOL::SYM_0,
        TM1650_SYMBOL::SYM_1,
        TM1650_SYMBOL::SYM_2,
        TM1650_SYMBOL::SYM_3,
        TM1650_SYMBOL::SYM_4,
        TM1650_SYMBOL::SYM_5,
        TM1650_SYMBOL::SYM_6,
        TM1650_SYMBOL::SYM_7,
        TM1650_SYMBOL::SYM_8,
        TM1650_SYMBOL::SYM_9,
        TM1650_SYMBOL::SYM_A,
        TM1650_SYMBOL::SYM_B,
        TM1650_SYMBOL::SYM_C,
        TM1650_SYMBOL::SYM_D,
        TM1650_SYMBOL::SYM_E,
        TM1650_SYMBOL::SYM_F
    };

    if (num > 0x0F) {
        // Handle out-of-range input — default to blank or zero
        return TM1650_SYMBOL::SYM_0;
    }

    TM1650_SYMBOL sym = symbols[num];

    return sym;
}

void tm1650_write_float(i2c_inst * i2c, float num, uint8_t precision)
{
    auto val = static_cast<uint32_t>(num * (pow(10, precision)));

    for (int8_t i = 3; i >= 0; i--)
    {
        uint8_t val_to_write = val % 10;
        val = val / 10;
        bool dot = (precision == (3 - i)) ? true : false;
        tm1650_write_sym(i2c, static_cast<TM1650_POS>(i), tm1650_number_to_symbol(val_to_write), dot);
    }
}

void tm1650_write_int(i2c_inst * i2c, uint16_t num)
{
    tm1650_reset(i2c);
    if (num > 9999)
    {
        num = 9999;
    }
    for (int8_t i = 3; i >= 0; i--)
    {
        uint8_t val_to_write = num % 10;
        num = num / 10;
        tm1650_write_sym(i2c, static_cast<TM1650_POS>(i), tm1650_number_to_symbol(val_to_write));
        if (num == 0)
        {
            break;
        }
    }
}
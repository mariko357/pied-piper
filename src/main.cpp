#include "main.h"
#include "pico/stdlib.h"
#include <cstdio>
#include <hardware/i2c.h>
#include "buttons.h"
#include "display.h"

void btn0_press_callback()
{
    printf("btn0_press_callback\n");
}
void btn0_long_press_callback()
{
    printf("btn0_long_press_callback\n");
}
void btn1_press_callback()
{
    printf("btn1_press_callback\n");
}
void btn1_long_press_callback()
{
    printf("btn1_long_press_callback\n");
}

void display_i2c_init()
{
    gpio_set_function(DISPLAY_SDA, GPIO_FUNC_I2C);
    gpio_set_function(DISPLAY_SCL, GPIO_FUNC_I2C);
    i2c_init(i2c0, 100000);
}

void send_ominous_message()
{
    tm1650_reset(i2c0);
    tm1650_set_brightness(i2c0, TM1650_BRIGHTNESS::BRIGHTNESS_8);
    tm1650_write_sym(i2c0, TM1650_POS::POS_0, TM1650_SYMBOL::SYM_H);
    tm1650_write_sym(i2c0, TM1650_POS::POS_1, TM1650_SYMBOL::SYM_E);
    tm1650_write_sym(i2c0, TM1650_POS::POS_2, TM1650_SYMBOL::SYM_L);
    tm1650_write_sym(i2c0, TM1650_POS::POS_3, TM1650_SYMBOL::SYM_L);
    sleep_ms(1000);
    tm1650_reset(i2c0);
    tm1650_write_sym(i2c0, TM1650_POS::POS_0, TM1650_SYMBOL::SYM_0);
}

void write_dash(i2c_inst *i2c)
{
    for (uint8_t i =0; i < 4; i++)
    {
        tm1650_write_raw(i2c, static_cast<TM1650_POS>(i), 1 << 6);
    }
}

int main(void)
{
    timer_hw->dbgpause = 0;
    stdio_init_all();

    gpio_set_dir(S0_PIN, GPIO_OUT);
    gpio_set_dir(S1_PIN, GPIO_OUT);
    gpio_set_dir(S2_PIN, GPIO_OUT);
    gpio_set_dir(S3_PIN, GPIO_OUT);
    gpio_set_drive_strength(S0_PIN, GPIO_DRIVE_STRENGTH_12MA);
    gpio_set_drive_strength(S1_PIN, GPIO_DRIVE_STRENGTH_12MA);
    gpio_set_drive_strength(S2_PIN, GPIO_DRIVE_STRENGTH_12MA);
    gpio_set_drive_strength(S3_PIN, GPIO_DRIVE_STRENGTH_12MA);

    gpio_set_dir(BTN0_PIN, GPIO_IN);
    gpio_set_dir(BTN1_PIN, GPIO_IN);
    gpio_pull_up(BTN0_PIN);
    gpio_pull_up(BTN1_PIN);

    display_i2c_init();
    send_ominous_message();
    sleep_ms(500);


    button btn0{.inverted = true, .pin = BTN0_PIN, .press_callback = btn0_press_callback, .long_press_callback = btn0_long_press_callback};
    button btn1{.inverted = true, .pin = BTN1_PIN, .press_callback = btn1_press_callback, .long_press_callback = btn1_long_press_callback};

    while (true)
    {
        handle_button(&btn0);
        handle_button(&btn1);
    }
    return 0;
}
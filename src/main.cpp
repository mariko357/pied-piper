#include "main.h"
#include "pico/stdlib.h"
#include <cstdio>
#include <hardware/i2c.h>
#include "buttons.h"
#include "display.h"
#include "pico/util/queue.h"
#include "core1.h"
#include "pico/multicore.h"

queue_t player_queue;
mutex_t player_mutex;

struct btn_flags
{
    bool press = false;
    bool long_press = false;
} btn0_flags, btn1_flags;

void btn0_press_callback()
{
    btn0_flags.press = true;
}

void btn0_long_press_callback()
{
    btn0_flags.long_press = true;
}

void btn1_press_callback()
{
    btn1_flags.press = true;
}

void btn1_long_press_callback()
{
    btn1_flags.long_press = true;
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
    tm1650_write_sym(i2c0, TM1650_POS::POS_1, TM1650_SYMBOL::SYM_0);
}

void write_dash(i2c_inst* i2c)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        tm1650_write_raw(i2c, static_cast<TM1650_POS>(i), 1 << 6);
    }
}

int main(void)
{
    timer_hw->dbgpause = 0;
    stdio_init_all();

    queue_init(&player_queue, sizeof(uint8_t), 4);
    mutex_init(&player_mutex);

    gpio_init(S0_PIN);
    gpio_init(S1_PIN);
    gpio_init(S2_PIN);
    gpio_init(S3_PIN);
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
    tm1650_reset(i2c0);

    button btn0{.inverted = true, .pin = BTN0_PIN, .press_callback = btn0_press_callback, .long_press_callback = btn0_long_press_callback};
    button btn1{.inverted = true, .pin = BTN1_PIN, .press_callback = btn1_press_callback, .long_press_callback = btn1_long_press_callback};

    uint8_t data_to_encode = 69;
    uint8_t old_data_to_encode = !data_to_encode;
    uint64_t btn0_long_press_expiry = 0, btn1_long_press_expiry = 0;

    multicore_reset_core1();
    multicore_launch_core1(core1_entry);

    while (true)
    {
        mutex_enter_blocking(&player_mutex);

        handle_button(&btn0);
        handle_button(&btn1);

        if (btn0_flags.press)
        {
            btn0_flags.press = false;
            if (data_to_encode != 0xFF)
            {
                data_to_encode++;
            }
        }

        if (btn1_flags.press)
        {
            btn1_flags.press = false;
            if (data_to_encode != 0x00)
            {
                data_to_encode--;
            }
        }

        if (btn0.state == press_type::BTN_LONG_TOUT && btn1.state == press_type::BTN_LONG_TOUT)
        {
            mutex_exit(&player_mutex);
            write_dash(i2c0);
            queue_try_add(&player_queue, &data_to_encode);
            old_data_to_encode = !data_to_encode;
            sleep_ms(1000);
            mutex_enter_blocking(&player_mutex);
            handle_button(&btn0);
            handle_button(&btn1);
        }


        if (btn0.state == press_type::BTN_LONG_TOUT && btn1.state == press_type::BTN_IDLE)
        {
            if (btn0_long_press_expiry < time_us_64())
            {
                btn0_long_press_expiry = time_us_64() + 500000;
                btn0_flags.long_press = false;
                if (data_to_encode <= 235)
                {
                    data_to_encode += 20;
                }
                else
                {
                    data_to_encode = 255;
                }
            }
        }

        if (btn1.state == press_type::BTN_LONG_TOUT && btn0.state == press_type::BTN_IDLE)
        {
            if (btn1_long_press_expiry < time_us_64())
            {
                btn1_long_press_expiry = time_us_64() + 500000;
                btn1_flags.long_press = false;
                if (data_to_encode >= 20)
                {
                    data_to_encode -= 20;
                }
                else
                {
                    data_to_encode = 0;
                }
            }
        }

        if (old_data_to_encode != data_to_encode)
        {
            old_data_to_encode = data_to_encode;
            tm1650_write_int(i2c0, data_to_encode);
        }

        mutex_exit(&player_mutex); // Non recursive mutex
    }
    return 0;
}

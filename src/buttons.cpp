//
// Created by marko on 10/18/25.
//

#include "buttons.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

inline constexpr uint32_t BUTTON_PRESS_THRESH = 200 * 1000;
inline constexpr uint32_t BUTTON_LONG_PRESS_THRESH = 1000 * 1000;

void handle_button(button* butt)
{
    uint64_t now = time_us_64();
    bool pressed = gpio_get(butt->pin);
    if (butt->inverted)
    {
        pressed = !pressed;
    }

    switch (butt->state)
    {
    case press_type::BTN_IDLE:
        if (pressed)
        {
            butt->state = press_type::BTN_DEBOUNCE;
            butt->press_time = now;
        }
        break;
    case press_type::BTN_DEBOUNCE:
        if (pressed)
        {
            if (butt->press_time + BUTTON_PRESS_THRESH > now)
            {
                butt->state = press_type::BTN_PRESSED;
                butt->press_time = now;
            }
        }
        else
        {
            butt->state = press_type::BTN_IDLE;
        }
        break;
    case press_type::BTN_PRESSED:
        if (!pressed)
        {
            uint64_t dt = now - butt->press_time;
            if (dt >= BUTTON_LONG_PRESS_THRESH)
            {
                butt->long_press_callback();
            }
            else
            {
                butt->press_callback();
            }
            butt->state = press_type::BTN_IDLE;
        }
        else if (now - butt->press_time >= BUTTON_LONG_PRESS_THRESH && butt->state != press_type::BTN_LONG_TOUT)
        {
            butt->state = press_type::BTN_LONG_TOUT;
            butt->long_press_callback();
        }
        break;
    case press_type::BTN_LONG_TOUT:
        if (!pressed)
        {
            butt->state = press_type::BTN_IDLE;
        }
        break;
    }
}

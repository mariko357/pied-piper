//
// Created by marko on 10/18/25.
//

#ifndef PIED_PIPER_BUTTONS_H
#define PIED_PIPER_BUTTONS_H

#include "stdint-gcc.h"


enum class press_type
{
    BTN_IDLE,
    BTN_DEBOUNCE,
    BTN_PRESSED,
    BTN_LONG_TOUT,
};

struct button
{
    bool inverted;
    uint8_t pin;
    uint64_t press_time;
    press_type state;
    void (*press_callback)(void);
    void (*long_press_callback)(void);
};

void handle_button(button * butt);
#endif //PIED_PIPER_BUTTONS_H

//
// Created by marko on 10/18/25.
//
#include "player.h"
#include "hardware/gpio.h"
#include "main.h"

uint8_t whistle_to_pin(uint8_t whistles)
{
    uint8_t pin;
    switch (whistles)
    {
        case WHISTLE_0:
            pin = S0_PIN;
            break;
        case WHISTLE_1:
            pin = S1_PIN;
            break;
        case WHISTLE_2:
            pin = S2_PIN;
            break;
        case WHISTLE_3:
            pin = S3_PIN;
            break;
        default:
            pin = 0;
    }
    return pin;
}

void play_transmission_bit(uint8_t whistles)
{
    uint8_t pin = whistle_to_pin(whistles);
    if (pin != 0)
    {
        gpio_put(pin, true);
    }
    else
    {
        gpio_put(pin, false);
    }
}


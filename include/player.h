//
// Created by marko on 10/18/25.
//

#ifndef PIED_PIPER_PLAYER_H
#define PIED_PIPER_PLAYER_H
#include "stdint-gcc.h"

enum whistle_number
{
    WHISTLE_0 = 1 << 0,
    WHISTLE_1 = 1 << 1,
    WHISTLE_2 = 1 << 2,
    WHISTLE_3 = 1 << 3,
};
struct player_transmission
{
    uint16_t period_ms;
    uint16_t len;
    uint8_t data[256];
};

void play_transmission_bit(uint8_t whistles);
uint8_t whistle_to_pin(uint8_t whistles);

#endif //PIED_PIPER_PLAYER_H
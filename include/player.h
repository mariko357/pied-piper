//
// Created by marko on 10/18/25.
//

#ifndef PIED_PIPER_PLAYER_H
#define PIED_PIPER_PLAYER_H
#include "stdint-gcc.h"
#include "cstdlib"

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
    uint8_t data[28];
};

void play_transmission_bit(uint8_t whistle, uint8_t bit);
uint8_t whistle_to_pin(uint8_t whistles);
void build_frame(uint8_t payload_byte, uint8_t *frame);
uint8_t crc8(uint8_t *data, size_t len);

#endif //PIED_PIPER_PLAYER_H
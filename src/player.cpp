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

void play_transmission_bit(uint8_t whistle, uint8_t bit)
{
    uint8_t pin = whistle_to_pin(whistle);
    if (bit)
    {
        gpio_put(pin, true);
    }
    else
    {
        gpio_put(pin, false);
    }
}

uint8_t crc8(uint8_t *data, size_t len) {
    uint8_t crc = 0;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = ((crc << 1) ^ 0x07) & 0xFF;
            } else {
                crc = (crc << 1) & 0xFF;
            }
        }
    }
    return crc;
}

void build_frame(uint8_t payload_byte, uint8_t *frame) {
    int pos = 0;

    // 4-bit preamble: [1, 0, 1, 0]
    frame[pos++] = 1;
    frame[pos++] = 0;
    frame[pos++] = 1;
    frame[pos++] = 0;

    // 8-bit sync: 0xA9 = [1, 0, 1, 0, 1, 0, 0, 1]
    frame[pos++] = 1;
    frame[pos++] = 0;
    frame[pos++] = 1;
    frame[pos++] = 0;
    frame[pos++] = 1;
    frame[pos++] = 0;
    frame[pos++] = 0;
    frame[pos++] = 1;

    for (int i = 7; i >= 0; i--) {
        frame[pos++] = (payload_byte >> i) & 1;
    }

    uint8_t crc = crc8(&payload_byte, 1);
    for (int i = 7; i >= 0; i--) {
        frame[pos++] = (crc >> i) & 1;
    }
}
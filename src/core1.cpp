//
// Created by marko on 10/18/25.
//

#include "core1.h"

#include <hardware/gpio.h>
#include <pico/mutex.h>

#include "cstdio"
#include "player.h"
#include "pico/util/queue.h"
#include "main.h"

extern queue_t player_queue;
extern mutex_t player_mutex;

void core1_entry(void)
{
    while (1)
    {
        uint8_t character_to_encode;
        queue_remove_blocking(&player_queue, &character_to_encode);
        mutex_enter_blocking(&player_mutex);

        player_transmission transmission;
        transmission.period_ms = 250;
        build_frame(character_to_encode, transmission.data);
        for (uint8_t i = 0; i < 28; i++)
        {
            gpio_put(S0_PIN, transmission.data[i]);
            sleep_ms(transmission.period_ms);
        }
        gpio_put(S0_PIN, false);

        mutex_exit(&player_mutex);
    }
}
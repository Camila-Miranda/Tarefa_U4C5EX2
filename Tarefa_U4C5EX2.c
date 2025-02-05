#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define LED_BLUE 11
#define LED_RED 12
#define LED_GREEN 13
#define BUTTON 5

volatile bool leds_active = false;

// Callback para desligar LEDs progressivamente
int64_t turn_off_callback(alarm_id_t id, void *user_data) {
    static int step = 0;
    if (step == 0) {
        gpio_put(LED_BLUE, 0);
    } else if (step == 1) {
        gpio_put(LED_RED, 0);
    } else if (step == 2) {
        gpio_put(LED_GREEN, 0);
        leds_active = false;
        step = -1;
    }
    step++;
    if (step < 3) {
        add_alarm_in_ms(3000, turn_off_callback, NULL, false);
    }
    return 0;
}

// Callback de interrupção do botão
void button_callback(uint gpio, uint32_t events) {
 if (gpio_get(LED_BLUE) == 0 && gpio_get(LED_RED) == 0 && gpio_get(LED_GREEN) == 0) {
        gpio_put(LED_BLUE, 1);
        gpio_put(LED_RED, 1);
        gpio_put(LED_GREEN, 1);
        add_alarm_in_ms(3000, turn_off_callback, NULL, false);
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_BLUE);
    gpio_init(LED_RED);
    gpio_init(LED_GREEN);
    gpio_init(BUTTON);
    
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_set_dir(BUTTON, GPIO_IN);
    gpio_pull_up(BUTTON);
     
    gpio_set_irq_enabled_with_callback(BUTTON, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    
    while (true) {
        printf("Sistema aguardando acionamento do botão...\n");
        sleep_ms(1000);
    }
}

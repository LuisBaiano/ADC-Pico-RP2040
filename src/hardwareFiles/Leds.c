#include "Leds.h"
#define LED_GREEN  11    // LED RGB: componente verde

void Led_init(){

    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_put(LED_GREEN, 0);
}

void toggle_led(uint pin) {
    bool current_state = gpio_get(pin);
    gpio_put(pin, !current_state);
}

void set_led(uint pin, bool state) {
    gpio_put(pin, state);
}

bool get_led_state(uint pin) {
    return gpio_get(pin);
}

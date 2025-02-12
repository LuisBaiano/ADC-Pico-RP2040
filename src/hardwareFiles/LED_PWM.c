#include "LED_PWM.h"

/**
 * @brief Inicializa o PWM para um pino específico, configurando uma resolução de 8 bits.
 * @param gpio_pin Pino a ser configurado para PWM.
 */
void init_pwm_led(uint gpio_pin) {
    gpio_set_function(gpio_pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio_pin);
    // Configura o divisor de clock para que o PWM opere com resolução de 8 bits
    pwm_set_clkdiv(slice_num, 1.0f);  // Utiliza o clock padrão; ajuste se necessário
    pwm_set_wrap(slice_num, LED_PWM_WRAP);
    pwm_set_enabled(slice_num, true);
}

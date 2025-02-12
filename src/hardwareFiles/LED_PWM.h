#ifndef PWM_H
#define PWM_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"

/** @brief Resolução de 8 bits para controle de brilho via PWM. */
#define LED_PWM_WRAP 255

/**
 * @brief Inicializa o PWM para um pino específico, configurando uma resolução de 8 bits.
 * @param gpio_pin Pino a ser configurado para PWM.
 */
void init_pwm_led(uint gpio_pin);

#endif // PWM_H
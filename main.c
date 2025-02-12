#include "src/debouncer.h"
#include "src/hardwareFiles/Leds.h"
#include "src/hardwareFiles/buttons.h"
#include "hardware/i2c.h"
#include "src/inc/ssd1306.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "src/LED_PWM.h"

/** @brief Configurações de I2C, pinos de LED, botões e outros periféricos. */
ssd1306_t ssd; ///< Estrutura do display
uint8_t border_style = 0; ///< Estilo da borda do display (alterna com o botão do joystick)
bool pwm_active = true; ///< Ativação dos LEDs PWM controlados pelo botão A

#define ADC_CENTER     2048
#define ADC_THRESHOLD  512   ///< Limiar para ligar o LED (ajuste conforme necessário)

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define LED_PWM_WRAP    255 ///< Resolução de 8 bits para controle de brilho

#define LED_RED   13    ///< LED RGB: componente vermelho
#define LED_BLUE  12    ///< LED RGB: componente azul
#define LED_GREEN  11   ///< LED RGB: componente verde

#define BUTTON_A    5     ///< Botão A

/// @brief Pinos do joystick
#define JOYSTICK_ADC_X  26   ///< ADC canal 0: eixo X
#define JOYSTICK_ADC_Y  27   ///< ADC canal 1: eixo Y
#define JOYSTICK_BTN    22   ///< Botão do joystick

// Definições para o display
#define DISPLAY_WIDTH   128
#define DISPLAY_HEIGHT  64
#define SQUARE_SIZE     8   ///< Tamanho do quadrado que representa a posição do joystick

#define DEBOUNCE_TIME 200000  ///< Tempo de debounce em microsegundos (200ms)

// Variáveis para armazenar o tempo da última interrupção de cada botão
static uint32_t last_interrupt_time_JOYSTICK = 0;
static uint32_t last_interrupt_time_A = 0;

/** @brief Inicializa o display OLED via I2C. */
void init_Display(){
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init(&ssd, DISPLAY_WIDTH, DISPLAY_HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

/**
 * @brief Callback de interrupção para tratamento dos botões.
 *
 * Alterna o estado dos LEDs correspondentes aos botões pressionados,
 * utilizando debounce para evitar múltiplos acionamentos.
 *
 * @param gpio Pino que gerou a interrupção.
 * @param events Tipo de evento ocorrido.
 */
void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == JOYSTICK_BTN) {
        if (check_debounce(&last_interrupt_time_JOYSTICK, DEBOUNCE_TIME)) {
            toggle_led(LED_GREEN);
            bool state = get_led_state(LED_GREEN);
            printf("LED Verde %s\n", state ? "Ligado" : "Desligado");
            border_style = (border_style + 1) % 2;
            printf("Estilo da borda alterado para: %s\n", border_style == 0 ? "Estilo continuo" : "Estilo pontilhado");
        }
    } else if (gpio == BUTTON_A) {
        if (check_debounce(&last_interrupt_time_A, DEBOUNCE_TIME)) {
            pwm_active = !pwm_active;
            printf("PWM %s\n", pwm_active ? "Ligado" : "Desligado");
        }
    }
}

/** @brief Inicializa o sistema ADC e configura os pinos para entrada analógica. */
void init_adc_system(void) {
    adc_init();
    adc_gpio_init(JOYSTICK_ADC_X);
    adc_gpio_init(JOYSTICK_ADC_Y);
}

/**
 * @brief Realiza a leitura do ADC para o canal especificado.
 * @param adc_channel Número do canal ADC (0 para GPIO26, 1 para GPIO27).
 * @return Valor lido (0 a 4095).
 */
uint16_t read_adc(uint adc_channel) {
    adc_select_input(adc_channel);
    sleep_us(5);
    return adc_read();
}

/**
 * @brief Atualiza a intensidade dos LEDs vermelho e azul com base nos valores ADC.
 * @param adc_x Valor ADC do eixo X.
 * @param adc_y Valor ADC do eixo Y.
 */
void update_leds(uint16_t adc_x, uint16_t adc_y) {
    const uint16_t deadzone = 100;  // Tolerância em torno do centro para a deadzone
    int16_t diff_x = (int16_t)adc_x - ADC_CENTER;
    int16_t diff_y = (int16_t)adc_y - ADC_CENTER;
    uint pwm_val_red = 0;
    uint pwm_val_blue = 0;

    // Calcula o valor PWM para o LED vermelho se a deflexão for maior que a deadzone
    if (abs(diff_x) > 140) {
        pwm_val_red = ((abs(diff_x) - 140) * LED_PWM_WRAP) / (2048 - 140);
    }
    // Calcula o valor PWM para o LED azul se a deflexão for maior que a deadzone
    if (abs(diff_y) > deadzone) {
        pwm_val_blue = ((abs(diff_y) - deadzone) * LED_PWM_WRAP) / (2048 - deadzone);
    }
    
    pwm_set_gpio_level(LED_RED, pwm_val_red);
    pwm_set_gpio_level(LED_BLUE, pwm_val_blue);
}
/**
 * @brief Atualiza o display OLED com a posição do joystick e o estilo da borda.
 * @param adc_x Valor ADC do eixo X.
 * @param adc_y Valor ADC do eixo Y.
 */
void update_display(uint16_t adc_x, uint16_t adc_y) {
    // Mapeia o ADC para a faixa do display sem offsets adicionais
    int pos_x = ((4095 - adc_x)* (DISPLAY_HEIGHT - SQUARE_SIZE)) / 4095;
    int pos_y = (adc_y * (DISPLAY_WIDTH - SQUARE_SIZE)) / 4095;
    
    // Limita as posições para que o quadrado não ultrapasse os limites do display
    if (pos_x < 0) pos_x = 0;
    if (pos_x > DISPLAY_HEIGHT - SQUARE_SIZE) pos_x = DISPLAY_HEIGHT - SQUARE_SIZE;
    if (pos_y < 0) pos_y = 0;
    if (pos_y > DISPLAY_WIDTH - SQUARE_SIZE) pos_y = DISPLAY_WIDTH - SQUARE_SIZE;
    
    // Limpa o display
    ssd1306_fill(&ssd, 0);

    // Desenha a borda conforme o estilo selecionado
    if (border_style == 0) {
        ssd1306_rect(&ssd, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 1, 0);
    } else {
        // Desenha uma borda traçada (dashed border)
        // Desenha linhas horizontais tracejadas (topo e base)
        for (int x = 0; x < DISPLAY_WIDTH; x += 4) {
            ssd1306_pixel(&ssd, x, 0, 1);                   // Linha superior
            ssd1306_pixel(&ssd, x, DISPLAY_HEIGHT - 1, 1);    // Linha inferior
        }
        // Desenha linhas verticais tracejadas (lado esquerdo e direito)
        for (int y = 0; y < DISPLAY_HEIGHT; y += 4) {
            ssd1306_pixel(&ssd, 0, y, 1);                   // Lado esquerdo
            ssd1306_pixel(&ssd, DISPLAY_WIDTH - 1, y, 1);     // Lado direito
        }
    }

    // Desenha o quadrado na posição calculada
    ssd1306_rect(&ssd, pos_x, pos_y, SQUARE_SIZE, SQUARE_SIZE, 1, 1);

    // Atualiza o display com os dados desenhados
    ssd1306_send_data(&ssd);
}

int main(){
    Led_init();
    init_pwm_led(LED_RED);
    init_pwm_led(LED_BLUE);
    stdio_init_all();
    init_buttons();
    init_Display();
    init_adc_system();

    // Configura as interrupções para os botões, acionando na borda de descida (FALLING)
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(JOYSTICK_BTN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    while (true)
    {  
        // Realiza a leitura dos valores ADC a cada iteração
        uint16_t adc_val_x = read_adc(0);  // Leitura do canal ADC para o eixo X
        uint16_t adc_val_y = read_adc(1);  // Leitura do canal ADC para o eixo Y

        // Atualiza a intensidade dos LEDs com base nos valores lidos
        if (pwm_active) {
            update_leds(adc_val_x, adc_val_y);
        } 
        else {
            pwm_set_gpio_level(LED_RED, 0);
            pwm_set_gpio_level(LED_BLUE, 0);
        }

        update_display(adc_val_x, adc_val_y);
        sleep_ms(25);
    }
        
        return 0;
    }
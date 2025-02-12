# Conversor Analógico-Digital (ADC) na Raspberry Pi Pico W RP2040

## Índice

1. [Objetivos](#objetivos)
2. [Descrição do Projeto](#descricao-do-projeto)
3. [Funcionalidades](#funcionalidades)
4. [Requisitos do Projeto](#requisitos-do-projeto)
5. [Como Executar](#como-executar)
6. [Estrutura do Código](#estrutura-do-codigo)
7. [Referências](#referencias)
8. [Demonstrativo em Vídeo](#demonstrativo-em-video)

## Objetivos

* Compreender o funcionamento do conversor analógico-digital (ADC) no RP2040.
* Utilizar PWM para controlar a intensidade de dois LEDs RGB com base nos valores do joystick.
* Representar a posição do joystick no display SSD1306 por meio de um quadrado móvel.
* Aplicar o protocolo I2C na comunicação com o display SSD1306.

## Descrição do Projeto

O joystick fornece valores analógicos correspondentes aos eixos X e Y, que serão utilizados para:

### Controle da Intensidade dos LEDs RGB

* **LED Azul** : Brilho ajustado conforme o valor do eixo Y.
* Posição central (2048) → LED apagado.
* Movimento para cima ou para baixo → Brilho aumenta gradualmente até o máximo nos extremos (0 e 4095).
* **LED Vermelho** : Brilho ajustado conforme o eixo X.
* Posição central (2048) → LED apagado.
* Movimento para esquerda ou direita → Brilho aumenta gradualmente até o máximo nos extremos (0 e 4095).
* Os LEDs são controlados via **PWM** para permitir variação suave da intensidade luminosa.

### Exibição no Display SSD1306

* Um **quadrado de 8x8 pixels** representa a posição do joystick e se movimenta proporcionalmente aos valores capturados.

### Funcionalidades dos Botões

* **Botão do Joystick (GPIO 22)** :
* Alterna o estado do **LED Verde** a cada acionamento.
* Modifica a borda do display para indicar o pressionamento, alternando entre diferentes estilos de borda.
* **Botão A (GPIO 5)** :
* Ativa ou desativa os LEDs PWM a cada acionamento.

## Funcionalidades

✅ Controle da intensidade dos LEDs RGB via joystick.
✅ Representação gráfica do movimento no display SSD1306.
✅ Alternância da borda do display ao pressionar o joystick.
✅ Controle de ativação/desativação dos LEDs PWM via botão A.
✅ Implementação de **interrupções** para os botões.
✅ Tratamento de **debounce** para evitar múltiplos acionamentos indesejados.
✅ Comunicação via **I2C** com o display SSD1306.

## Requisitos do Projeto

1. **Uso de interrupções** : Todas as funcionalidades dos botões devem ser implementadas utilizando rotinas de interrupção (IRQ).
2. **Debouncing** : O bouncing dos botões deve ser tratado via software.
3. **Utilização do Display 128x64 SSD1306** : Deve demonstrar o entendimento do protocolo I2C.
4. **Código bem estruturado e comentado** para facilitar o entendimento.

## Como Executar

### Requisitos

* Placa **Raspberry Pi Pico (RP2040)**
* Placa **BitDogLab**
* **Joysticks e Botões**
* **LEDs RGB**
* **Display OLED SSD1306 (128x64 pixels) com I2C**

### Passos

1. Clone o repositório e compile o código.
2. Carregue o firmware na  **Raspberry Pi Pico** .
3. Conecte o hardware conforme os pinos descritos.
4. Execute o programa e teste as funcionalidades do joystick e display.

## Estrutura do Código

```
📂 src/
 ├── hardwareFiles/
 │   ├── Leds.h       # Controle dos LEDs
 |   ├── LED_PWM.h    # Controle PWM dos LEDs
 │   ├── buttons.h    # Configuração dos botões
 ├── inc/
 │   ├── ssd1306.h    # Biblioteca do display SSD1306
 ├── debouncer.h      # Implementação de debounce
├── main.c            # Código principal do projeto
```

## Referências

* [Documentação do RP2040](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
* [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)
* [BitDogLab - Informações da Placa](https://www.bitdoglab.com/)

## Demonstrativo em Vídeo

[📹 Assista no YouTube](#)

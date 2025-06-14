#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "FreeRTOS.h"
#include "task.h"

// Pinos dos LEDs RGB
#define RED_LED_PIN   13
#define GREEN_LED_PIN 11
#define BLUE_LED_PIN  12

// Pino do Buzzer
#define BUZZER_PIN 21
#define BUZZER_FREQUENCY 100

// Pinos dos botões
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6

// Handles das tarefas
TaskHandle_t rgbTaskHandle = NULL;
TaskHandle_t buzzerTaskHandle = NULL;

// ---------- Inicializa PWM do buzzer ----------
void pwm_init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096));
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(pin, 0);
}

// ---------- Beep com FreeRTOS ----------
void beep_freertos(uint pin, uint duration_ms) {
    pwm_set_gpio_level(pin, 2048); // 50% duty
    vTaskDelay(pdMS_TO_TICKS(duration_ms));
    pwm_set_gpio_level(pin, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
}

// ---------- Tarefa do LED RGB ----------
void rgb_task(void *params) {
    gpio_init(RED_LED_PIN);
    gpio_init(GREEN_LED_PIN);
    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);

    int estado = 0;

    while (1) {
        // Alterna entre vermelho, verde e azul
        gpio_put(RED_LED_PIN,   estado == 0);
        gpio_put(GREEN_LED_PIN, estado == 1);
        gpio_put(BLUE_LED_PIN,  estado == 2);

        estado = (estado + 1) % 3;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// ---------- Tarefa do buzzer ----------
void buzzer_task(void *params) {
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    pwm_init_buzzer(BUZZER_PIN);

    while (1) {
        beep_freertos(BUZZER_PIN, 1000);  // Beep de 1 segundo
        vTaskDelay(pdMS_TO_TICKS(1000));  // Espera 1 segundo → total 2s entre beeps
    }
}

// ---------- Tarefa dos botões ----------
void button_task(void *params) {
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    bool rgb_suspended = false;
    bool buzzer_suspended = false;

    while (1) {
        if (gpio_get(BUTTON_A_PIN) == 0) {
            if (rgb_suspended) {
                vTaskResume(rgbTaskHandle);
            } else {
                vTaskSuspend(rgbTaskHandle);
            }
            rgb_suspended = !rgb_suspended;
            vTaskDelay(pdMS_TO_TICKS(50)); // debounce
        }

        if (gpio_get(BUTTON_B_PIN) == 0) {
            if (buzzer_suspended) {
                vTaskResume(buzzerTaskHandle);
            } else {
                vTaskSuspend(buzzerTaskHandle);
            }
            buzzer_suspended = !buzzer_suspended;
            vTaskDelay(pdMS_TO_TICKS(50)); // debounce
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // polling geral
    }
}

// ---------- Função principal ----------
int main() {
    stdio_init_all();

    xTaskCreate(rgb_task, "RGB", 256, NULL, 1, &rgbTaskHandle);
    xTaskCreate(buzzer_task, "Buzzer", 256, NULL, 1, &buzzerTaskHandle);
    xTaskCreate(button_task, "Buttons", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true) {}
}
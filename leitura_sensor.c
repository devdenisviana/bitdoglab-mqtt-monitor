#include "leitura_sensor.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"

// Variáveis para debounce do botão
static uint32_t ultimo_tempo_botao = 0;
static bool ultimo_estado_botao = false;
static bool estado_botao_debounced = false;
#define DEBOUNCE_DELAY_MS 50  // 50ms para debounce

void inicializar_pinos()
{
    adc_init();                          // Inicializa o ADC
    adc_set_temp_sensor_enabled(true);    // Habilita o sensor de temperatura interno
    
    // Inicializa o pino do botão
    gpio_init(5);                        // GPIO 5 para o botão
    gpio_set_dir(5, GPIO_IN);            // Configura como entrada
    gpio_pull_up(5);                     // Ativa pull-up interno
}

bool ler_botao_com_debounce()
{
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
    bool estado_atual = !gpio_get(5);    // Inverte porque pull-up (HIGH quando não pressionado)
    
    // Verifica se passou tempo suficiente desde a última mudança
    if (tempo_atual - ultimo_tempo_botao > DEBOUNCE_DELAY_MS) {
        if (estado_atual != ultimo_estado_botao) {
            estado_botao_debounced = estado_atual;
            ultimo_estado_botao = estado_atual;
            ultimo_tempo_botao = tempo_atual;
        }
    }
    
    return estado_botao_debounced;
}

float ler_temperatura()
{
    const int amostras = 10;
    uint32_t soma = 0;

    adc_select_input(4); // Seleciona o canal do sensor interno

    for (int i = 0; i < amostras; i++) {
        sleep_ms(5);
        soma += adc_read();
    }

    float media = soma / (float)amostras;
    const float conversao = 3.3f / (1 << 12); // 3.3V e ADC de 12 bits
    float tensao = media * conversao;

    // Fórmula oficial do datasheet do RP2040
    float temperatura = 27.0f - (tensao - 0.706f) / 0.001721f;

    printf("DEBUG_TEMP: ADC=%.2f | V=%.3f V | TEMP=%.2f C\n", media, tensao, temperatura);

    return temperatura;
}

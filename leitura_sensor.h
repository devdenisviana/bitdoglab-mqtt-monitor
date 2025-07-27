#ifndef LEITURA_SENSOR_H
#define LEITURA_SENSOR_H

#include "pico/stdlib.h"
#include "hardware/adc.h"

void inicializar_pinos();
float ler_temperatura();
bool ler_botao_com_debounce();

#endif

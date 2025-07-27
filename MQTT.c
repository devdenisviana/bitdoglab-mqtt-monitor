#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "lwip/netif.h"
#include "lwip/altcp_tcp.h"
#include "lwip/dns.h"
#include "lwip/apps/lwiperf.h"
#include "pico/cyw43_arch.h"
#include "pico/time.h"
#include "mqtt_configuracao.h"
#include "leitura_sensor.h"

// --- DEFINIÇÕES DA REDE WIFI ---
#define SSID_WIFI "SUAREDE" // Nome da rede wifi
#define SENHA_WIFI "SUASENHA" // Senha da rede wifi

// Tópicos MQTT
#define TOPICO_BOTAO_MQTT "bitdoglab/botao_status" // Tópico para status do botão
#define TOPICO_TEMP_MQTT "bitdoglab/temperature" // Tópico para temperatura

// --- VARIÁVEIS GLOBAIS ---
bool ultimo_estado_botao_enviado = false; // Para detectar mudanças no botão

void callback_mqtt(char *topico, char *mensagem) { 
    printf("Monitor: Callback MQTT recebido - Topico: '%s', Mensagem: '%s'\n", topico, mensagem);
    // Por enquanto, apenas log das mensagens recebidas
}

//----FUNCAO PRINCIPAL---
int main() {
    stdio_init_all(); // Inicializa o sistema

    // --- Conexao WIFI ----
    printf("Monitor: Iniciando Wi-Fi...\n");
    if (cyw43_arch_init()) {
        printf("Monitor: Falha na inicializacao do Wi-Fi CYW43.\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();

    printf("Monitor: Conectando ao Wi-Fi %s...\n", SSID_WIFI);
    int tentativas = 0; 
    while (cyw43_arch_wifi_connect_timeout_ms(SSID_WIFI, SENHA_WIFI, CYW43_AUTH_WPA2_AES_PSK, 30000) != 0) {
        printf("Monitor: Falha na conexao Wi-Fi. Tentando novamente...\n");
        tentativas++;
        if (tentativas > 5) {
            printf("Monitor: Nao foi possivel conectar ao Wi-Fi. Verifique as credenciais.\n");
            cyw43_arch_deinit();
            return 1;
        }
        sleep_ms(2000);
    }
    printf("Monitor: Wi-Fi conectado com sucesso!\n");

    //---- Inicializa sensor e botão ----
    inicializar_pinos(); // Habilita leitura do sensor (ADC) e configura botão

    //---- Conexao com MQTT ----
    printf("Monitor: Iniciando conexao MQTT...\n");
    inicializar_mqtt(); 
    definir_callback_usuario_mqtt(callback_mqtt); 

    // Aguarda conexão MQTT
    uint32_t tempo_inicio = to_ms_since_boot(get_absolute_time()); 
    while (!mqtt_esta_conectado && (to_ms_since_boot(get_absolute_time()) - tempo_inicio < 5000)) {
        printf("Monitor: Aguardando conexao MQTT... (%lums de 5000ms)\n", (to_ms_since_boot(get_absolute_time()) - tempo_inicio));
        sleep_ms(1000);
    }

    if (mqtt_esta_conectado) {
        printf("Monitor: MQTT conectado com sucesso!\n");
    } else {
        printf("Monitor: Falha na conexao MQTT apos timeout. Continuando sem MQTT.\n");
    }

    printf("Monitor: Entrando no loop principal...\n");

    //---- LOOP PRINCIPAL ----
    uint32_t contador_loop = 0;
    uint32_t ultimo_tempo_temp = 0;
    uint32_t ultimo_tempo_botao = 0;
    
    while (true) {
        uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
        
        // Verifica botão a cada 1 segundo
        if (tempo_atual - ultimo_tempo_botao >= 1000) {
            bool estado_botao = ler_botao_com_debounce();
            
            // Só envia se o estado mudou ou é a primeira leitura
            if (estado_botao != ultimo_estado_botao_enviado || contador_loop == 0) {
                char status_botao[10];
                sprintf(status_botao, "%s", estado_botao ? "PRESSED" : "RELEASED");
                
                printf("BOTAO: Status=%s (GPIO 5)\n", status_botao);
                publicar_mensagem(TOPICO_BOTAO_MQTT, status_botao);
                
                ultimo_estado_botao_enviado = estado_botao;
            }
            
            ultimo_tempo_botao = tempo_atual;
        }
        
        // Verifica temperatura a cada 5 segundos
        if (tempo_atual - ultimo_tempo_temp >= 5000) {
            float temperatura_atual = ler_temperatura();
            char temp_str[20];
            sprintf(temp_str, "%.2f", temperatura_atual);
            
            printf("TEMP: %.2f C\n", temperatura_atual);
            publicar_mensagem(TOPICO_TEMP_MQTT, temp_str);
            
            ultimo_tempo_temp = tempo_atual;
        }
        
        contador_loop++;
        sleep_ms(100); // Loop principal a cada 100ms
    }

    cyw43_arch_deinit();
    return 0;
}

# Monitor BitDogLab MQTT PICO W - Botão e Temperatura

## Visão Geral
Este projeto transforma um Raspberry Pi Pico W em um nó IoT capaz de:
- Monitorar o status de um botão conectado na GPIO 5 (com proteção contra debounce)
- Monitorar a temperatura interna do microcontrolador
- Publicar os dados em tempo real via MQTT para o broker HiveMQ
- Exibir informações no Serial Monitor

## Como Funciona
1. **Conexão Wi-Fi:**
   - O Pico W conecta-se à rede Wi-Fi definida no código.
2. **Conexão MQTT:**
   - O dispositivo conecta-se ao broker MQTT público HiveMQ.
3. **Monitoramento do Botão:**
   - A cada 1 segundo, verifica o status do botão na GPIO 5
   - Implementa proteção contra debounce (50ms)
   - Publica mudanças de estado no tópico: `bitdoglab/botao_status`
   - Mensagens: `"PRESSED"` ou `"RELEASED"`
4. **Monitoramento de Temperatura:**
   - A cada 5 segundos, lê a temperatura interna
   - Publica no tópico: `bitdoglab/temperature`
   - Valor em graus Celsius com 2 casas decimais

## Configuração do Hardware
- **Botão:** Conectado na GPIO 5
  - Uma perna do botão conectada ao GPIO 5
  - Outra perna conectada ao GND
  - Pull-up interno ativado no código
- **Temperatura:** Sensor interno do RP2040 (não requer hardware adicional)

## Como Usar com MQTTX
1. **Conecte-se ao broker HiveMQ:**
   - Host: `broker.hivemq.com`
   - Porta: `1883`
   - Username/Password: deixe em branco
2. **Para visualizar o status do botão:**
   - Adicione uma subscription ao tópico `bitdoglab/botao_status`
3. **Para visualizar a temperatura:**
   - Adicione uma subscription ao tópico `bitdoglab/temperature`

## Tópicos Utilizados
- **Publicação:**
  - `bitdoglab/botao_status` (status do botão: "PRESSED" ou "RELEASED")
  - `bitdoglab/temperature` (temperatura em graus Celsius)

## Exemplo de Saída no Serial Monitor
```
Monitor: Wi-Fi conectado com sucesso!
Monitor: MQTT conectado com sucesso!
Monitor: Entrando no loop principal...
BOTAO: Status=RELEASED (GPIO 5)
TEMP: 25.34 C
BOTAO: Status=PRESSED (GPIO 5)
BOTAO: Status=RELEASED (GPIO 5)
TEMP: 25.36 C
```

## Características Técnicas
- **Debounce:** 50ms para eliminar ruídos do botão
- **Frequência de amostragem:**
  - Botão: 1 segundo
  - Temperatura: 5 segundos
- **Precisão da temperatura:** 2 casas decimais
- **Amostras de temperatura:** 10 leituras com média

## Requisitos
- Raspberry Pi Pico W
- Botão push-button
- Broker MQTT público (HiveMQ)
- Cliente MQTT (ex: MQTTX)

## Configuração
1. Edite as credenciais Wi-Fi no arquivo `MQTT.c`:
   ```c
   #define SSID_WIFI "SUAREDE"
   #define SENHA_WIFI "SUASENHA"
   ```
2. Compile e faça upload para o Pico W
3. Monitore o Serial Monitor para ver as mensagens de debug

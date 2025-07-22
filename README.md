# Monitor BitDogLab MQTT PICO W

## Visão Geral
Este projeto transforma um Raspberry Pi Pico W em um nó IoT capaz de:
- Monitorar a temperatura interna do microcontrolador
- Publicar os dados de temperatura em tempo real via MQTT
- Receber comandos remotos para ligar/desligar um LED
- Exibir informações em um display OLED

## Como Funciona
1. **Conexão Wi-Fi:**
   - O Pico W conecta-se à rede Wi-Fi definida no código.
2. **Conexão MQTT:**
   - O dispositivo conecta-se ao broker MQTT público HiveMQ (IP fixo configurado no código).
3. **Publicação de Temperatura:**
   - A cada 5 segundos, a temperatura é lida e publicada no tópico:
     - `bitdoglab/temperature`
4. **Controle Remoto do LED:**
   - O dispositivo subscreve ao tópico:
     - `pico_led_controle`
   - Ao receber a mensagem `on`, o LED é ligado.
   - Ao receber a mensagem `off`, o LED é desligado.
5. **Display OLED:**
   - Mostra o status da conexão, última mensagem recebida e a temperatura atual.
6. **Serial Debug:**
   - Informações de debug são exibidas no terminal/Serial Monitor do VSCode.

## Como Usar com MQTTX
1. **Conecte-se ao broker HiveMQ:**
   - Host: `3.76.111.254` (ou outro IP válido do broker)
   - Porta: `1883`
   - Username/Password: deixe em branco
2. **Para visualizar a temperatura:**
   - Adicione uma nova subscription ao tópico `bitdoglab/temperature`.
3. **Para controlar o LED:**
   - Publique no tópico `pico_led_controle` com o payload `on` (liga) ou `off` (desliga).

## Tópicos Utilizados
- **Publicação:**
  - `bitdoglab/temperature` (temperatura em tempo real)
- **Subscrição:**
  - `pico_led_controle` (controle do LED)

## Exemplo de Fluxo
1. O Pico W conecta ao Wi-Fi e ao broker MQTT.
2. Publica a temperatura periodicamente.
3. Você pode ver as mensagens no MQTTX.
4. Para ligar/desligar o LED, publique `on` ou `off` no tópico `pico_led_controle` pelo MQTTX.
5. O status e a última mensagem recebida aparecem no display OLED.

## Requisitos
- Raspberry Pi Pico W
- Display OLED SSD1306
- Broker MQTT público (HiveMQ)
- Cliente MQTT (ex: MQTTX)

## Observações
- O IP do broker pode mudar. Se não conseguir conectar, faça um novo `nslookup broker.hivemq.com` e atualize o IP no código.
- O uso do nome do host (DNS) é o ideal, mas o código está configurado para IP fixo por simplicidade.

# AudioMonitorBT_ESP32

Receptor de áudio Bluetooth A2DP com controle físico de:

**Play / Pause**

**Próxima faixa**

**Volume via potenciômetro**

Desenvolvido para ESP32 utilizando:

**ESP32-A2DP** – Bluetooth A2DP Sink para ESP32
https://github.com/pschatzmann/ESP32-A2DP

**AudioTools** – Framework de áudio para ESP32
https://github.com/pschatzmann/arduino-audio-tools

## Saída de áudio

Este projeto terá duas versões de saída de áudio:

**Versão DAC Interno**
Utiliza o DAC interno do ESP32.

**Versão I2S**
Utiliza saída digital I2S com DAC externo para melhor qualidade de áudio.

## Funcionalidades

Conexão Bluetooth A2DP Sink

Áudio Estéreo ou Mono (Downmix)

Controle de volume por ADC

Controle de mídia por botões físicos

Debounce por software

Nome Bluetooth configurável

## Hardware Utilizado

ESP32

2 botões (Play/Pause e Next)

1 potenciômetro (10k recomendado)

Saída de áudio analógica

## Pinagem

| Função        | Pino ESP32 |
|---------------|------------|
| Play/Pause    |    GPIO 32 |
| Próxima faixa |    GPIO 33 |
| Potenciômetro |    GPIO 35 |

## Controle de Volume

Leitura ADC: 0–4095

Mapeamento: 0–127

## Fluxo de Funcionamento

**Setup**

ESP32 inicia o Bluetooth

Recebe áudio via A2DP

Reproduz na saída analógica (ou envia para DAC externo)

**Loop principal**

Lê botões

Aplica debounce

Controla mídia

Atualiza volume

## Arquitetura e Uso dos Cores

O ESP32 possui dois núcleos (dual-core).

**Neste projeto:**

O A2DP (Bluetooth + processamento de áudio) roda em uma task própria, gerenciada internamente pela biblioteca.

O loop() principal roda separadamente, cuidando apenas da interação física (botões e potenciômetro).

**Isso permite que:**

O áudio continue fluindo independentemente do código no loop().

O uso de delay curto não interrompa a reprodução.

A interface física funcione de forma estável sem comprometer o streaming.

Essa separação garante fluidez no áudio e simplicidade na lógica de controle.
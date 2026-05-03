# Sistema de Coordenação de Atenção - ESP32 MQTT

Este projeto consiste em um sistema de monitoramento centralizado para múltiplos laboratórios, utilizando um **ESP32** como coordenador. O dispositivo atua como um "dashboard físico", processando dados de atenção de alunos enviados via protocolo **MQTT** e sinalizando visualmente o estado de cada sala através de LEDs.

## 🚀 Funcionalidades

- **Conectividade Dual**: Integração com Wi-Fi (Wokwi-GUEST) e Broker MQTT (Shiftr.io).
- **Monitoramento Multi-Lab**: Gerenciamento simultâneo de 5 laboratórios distintos.
- **Processamento de Dados**: Cálculo em tempo real da média de atenção por laboratório (baseado em até 10 alunos por sala).
- **Sinalização de Alerta**: Controle de 5 LEDs independentes que indicam quando a atenção média de um laboratório cai abaixo do limiar crítico (0.5).
- **Log Inteligente**: Monitoramento detalhado via Serial, exibindo médias, contagem de amostras e status de cada LED.

## 🛠️ Arquitetura do Sistema

O coordenador assina os tópicos globais de cada laboratório (`Lab1/#` até `Lab5/#`) e processa mensagens no formato:
`LabX/AlunoYY` → *Payload: Valor de atenção (0.0 a 1.0)*

### Mapeamento de Hardware

| Laboratório | GPIO (ESP32) | Indicador LED |
| :--- | :---: | :--- |
| **Laboratório 1** | 2 | Vermelho |
| **Laboratório 2** | 4 | Amarelo |
| **Laboratório 3** | 5 | Verde |
| **Laboratório 4** | 18 | Azul |
| **Laboratório 5** | 19 | Branco |

## 💻 Tecnologias Utilizadas

- **Linguagem**: C++ (Arduino Framework)
- **Hardware**: ESP32 DevKit V1
- **Protocolo**: MQTT (Message Queuing Telemetry Transport)
- **Bibliotecas**: 
  - `WiFi.h` (Conectividade nativa)
  - `PubSubClient.h` (Cliente MQTT leve)
- **Ambiente de Simulação**: [Wokwi](https://wokwi.com)

## ⚙️ Configuração e Instalação

1. **Hardware/Simulação**:
   - Conecte os LEDs nos GPIOs indicados na tabela de mapeamento, utilizando resistores de 220Ω em série.
2. **Software**:
   - Clone este repositório ou copie o conteúdo de `sketch.ino`.
   - Certifique-se de ter a biblioteca **PubSubClient** instalada na sua IDE Arduino ou declarada no seu `wokwi.toml`.
3. **Credenciais MQTT**:
   - O código já está pré-configurado com as credenciais do broker `escola-mesquita`. Caso deseje utilizar outro, altere as constantes `MQTT_HOST`, `MQTT_USER` e `MQTT_PASS`.

## 📊 Lógica de Operação

O sistema utiliza uma matriz bidimensional para armazenar o estado mais recente de cada aluno. A média é calculada de forma dinâmica:
- **Atenção Média >= 0.5**: LED Apagado (Status OK).
- **Atenção Média < 0.5**: LED Aceso (Alerta de baixa atenção na sala).

---
*Desenvolvido para fins educacionais de monitoramento IoT em sala de aula.*

# 🚦 Projeto de Semáforo Inteligente

Este projeto implementa um sistema de semáforo inteligente utilizando dois ESP32, sensores de luz (LDR), LEDs e a plataforma HiveMQ para comunicação. O sistema é projetado para detectar veículos, adaptar-se ao modo noturno e coordenar os sinais entre um semáforo de rua e um de avenida, dando prioridade para o fluxo na avenida.

## 🌐 Visão Geral

O semáforo inteligente foi desenvolvido com os seguintes objetivos:
- **Detecção de veículos**: Utilizando sensores de luminosidade (LDR) para identificar a presença de veículos e alterar o sinal para otimizar o fluxo.
- **Modo noturno**: Quando a luminosidade ambiente é baixa, o semáforo entra em "modo noturno", piscando em amarelo.
- **Prioridade da Avenida**: Como a avenida é uma via principal, ela possui maior prioridade e deve sempre que possível, permanecer com o sinal verde.
- **Comunicação entre semáforos**: Os semáforos (um na rua e outro na avenida), cada um conectado a um ESP32, comunicam-se através do protocolo MQTT e da plataforma HiveMQ para coordenar o fluxo de veículos.

## 🛠️ Estrutura do Projeto

- `rua.ino`: Código responsável pelo controle do semáforo na rua.
- `avenida.ino`: Código responsável pelo controle do semáforo na avenida.
- **HiveMQ**: Plataforma MQTT utilizada para gerenciar os dados do LDR e permitir a comunicação entre os ESP32 dos semáforos.

## 📋 Componentes Utilizados

- **2 ESP32**: Cada semáforo (rua e avenida) é controlado por um ESP32, permitindo comunicação e controle independentes.
- **LDR (Light Dependent Resistor)**: Sensor de luminosidade utilizado para detectar a presença de veículos e identificar o ambiente noturno.
- **LEDs**: Indicadores visuais para os sinais de tráfego (verde, amarelo e vermelho).
- **HiveMQ**: Plataforma MQTT usada para monitorar e controlar o estado dos semáforos remotamente.

## 🔧 Montagem Física

### Conexões
- **LEDs**: Conectados aos pinos 23 (verde), 22 (amarelo) e 21 (vermelho) em ambos os ESP32.
- **LDRs**: Conectados aos pinos analógicos 36 (rua) e 39 (calçada) para detecção de luminosidade na rua.

## 🖥️ Programação e Lógica de Funcionamento

### Código para o Semáforo da Rua (`rua.ino`)

1. **Conexão Wi-Fi e MQTT**: Configura a conexão do ESP32 com a rede Wi-Fi e o servidor HiveMQ para comunicação entre os semáforos.
2. **Detecção de Veículos**: O LDR monitora a luminosidade para identificar a presença de um veículo. Quando um veículo é detectado, o semáforo da avenida é solicitado a mudar para vermelho, permitindo que o semáforo da rua mude para verde temporariamente.
3. **Modo Noturno**: Se o LDR detectar baixa luminosidade ambiente, o semáforo entra em modo noturno, piscando o LED amarelo.
4. **Publicação de Status**: Publica o status do semáforo no tópico MQTT para que a avenida ajuste seu comportamento conforme necessário.

### Código para o Semáforo da Avenida (`avenida.ino`)

1. **Conexão Wi-Fi e MQTT**: Configura a conexão do ESP32 com a rede Wi-Fi e o servidor HiveMQ.
2. **Recepção de Comandos**: Recebe comandos de controle do semáforo da rua para alterar seu estado (verde, vermelho, ou modo piscar amarelo). A avenida, como via principal, é programada para estar verde por padrão e só altera para vermelho quando necessário.
3. **Modo Noturno**: Implementa o modo de piscar amarelo conforme os comandos recebidos ou em caso de baixa luminosidade.

### 🌍 Comunicação via HiveMQ

A integração com o HiveMQ permite:
- **Monitoramento Remoto**: A interface do HiveMQ exibe o status dos LDRs e dos semáforos, permitindo visualizar os dados captados em tempo real.
- **Ajustes Remotos**: Possibilidade de ativar o modo noturno manualmente e ajustar parâmetros conforme necessário.
- **Coordenação de Tráfego**: Através de tópicos MQTT, os semáforos comunicam-se para sincronizar o fluxo de veículos, alternando o sinal entre a rua e a avenida. **A avenida, por ser a via principal, permanece verde sempre que possível, garantindo fluidez no tráfego principal.**

## ⚙️ Configuração da Plataforma HiveMQ

1. **Criação de Tópicos**: Configure tópicos no HiveMQ para monitorar o status dos semáforos e as leituras dos sensores de luminosidade.
2. **Integração MQTT**: Use as configurações MQTT fornecidas pela HiveMQ para conectar os ESP32 à plataforma.

## 🧪 Testes Realizados

### 1. Teste de Detecção de Veículos
- **Pré-condição**: Luminosidade normal na área do semáforo da rua.
- **Procedimento**: O LDR detecta a passagem de um veículo simulando uma variação na luz.
- **Resultado Esperado**: O semáforo da rua fica verde temporariamente, e a avenida alterna para vermelho para permitir a passagem do veículo.

### 2. Teste do Modo Noturno
- **Pré-condição**: Ambiente com baixa luminosidade.
- **Procedimento**: Reduza a iluminação para simular o ambiente noturno.
- **Resultado Esperado**: Ambos os semáforos alternam para o modo piscar amarelo.

### 3. Teste de Comunicação entre Semáforos
- **Pré-condição**: Semáforos em estado inicial (rua vermelha e avenida verde).
- **Procedimento**: Simule a detecção de um veículo na rua.
- **Resultado Esperado**: O semáforo da avenida recebe o comando para alternar para vermelho, enquanto o semáforo da rua fica verde temporariamente.

## 🎥 Vídeo de Demonstração

https://github.com/user-attachments/assets/4c32f587-c92b-4020-8550-c41f3f8d64b2

## 📌 Conclusão

Este projeto de semáforo inteligente utiliza sensores e comunicação via MQTT para adaptar o fluxo de veículos e as condições de luminosidade em uma via principal (avenida) e uma via secundária (rua). A avenida é priorizada para manter o tráfego fluindo, e o modo noturno garante visibilidade adequada com o piscar amarelo quando a luminosidade é baixa. A integração com o HiveMQ permite monitoramento remoto e sincronização entre os semáforos, tornando o sistema ideal para uma gestão de tráfego mais eficiente.

## 👥 Criadores do Projeto

|                                                                                             |                                                                                             |                                                                                             |                                                                                             |
|---------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------|
| <img src="assets/Isabelle_Messias_Dantas_Pereira.jpg" alt="Isabelle" width="150">           | <img src="assets/Nataly_de_Souza_Cunha.jpg" alt="Nataly" width="150">                       | <img src="assets/Heitor_de_Faria_Candido.jpg" alt="Heitor" width="150">                     | <img src="assets/Ian_Pereira_Simão.jpg" alt="Ian" width="150">                             |
| **Isabelle Pereira**                                                        | **Nataly Cunha**                                                                   | **Heitor Candido**                                                                 | **Ian Simão**                                                                       |

|                                                                                             |                                                                                             |                                                                                             |
|---------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------|
| <img src="assets/Rodrigo_Hu_Tchie_Lee.jpg" alt="Rodrigo" width="150">                       | <img src="assets/Lucas_Paiva_Brasil.jpg" alt="Lucas" width="150">                           | <img src="assets/João_Victor_de_Souza_Campos.jpg" alt="João" width="150">                  |
| **Rodrigo Lee**                                                                   | **Lucas Brasil**                                                                      | **João Campos**                                                             |






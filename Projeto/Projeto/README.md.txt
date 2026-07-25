# Projeto de Monitoramento Ambiental – Franzininho WiFi + InfluxDB Cloud

Este projeto implementa um sistema IoT que coleta dados de temperatura, umidade e luminosidade de um ESP32, exibe em display OLED e envia para o InfluxDB Cloud, com um dashboard para visualização remota.

##  Estrutura do Repositório

- `Firmware/` – Código do ESP32 (Arduino)
- `Backend/` – (opcional) Scripts de apoio ou configuração do InfluxDB
- `Frontend/` – Dashboard web para visualização dos dados
- `docs/` – Documentação complementar (imagens, esquemas, etc.)
- `video/` – Vídeo de apresentação do projeto
- `README.md` – Este arquivo

##  Funcionalidades

- Leitura de temperatura, umidade (DHT11) e luminosidade (LDR)
- Exibição local em display OLED 128x64 (I2C)
- Envio HTTPS para InfluxDB Cloud a cada 3 segundos
- Feedback visual (LED RGB) e sonoro (buzzer) de sucesso/erro
- Dashboard web para visualização dos dados em tempo real (Frontend)

##  Tecnologias Utilizadas

- ESP32 (Franzininho WiFi Lab01)
- Arduino framework
- InfluxDB Cloud (backend como serviço)
- HTML, CSS, JavaScript (dashboard)
- API de escrita do InfluxDB v2

## Como executar

### Firmware
1. Instale as bibliotecas: DHT sensor library, Adafruit SSD1306, Adafruit GFX.
2. Abra `Firmware/ambiente-iot-esp32.ino` no Arduino IDE.
3. Configure as credenciais Wi-Fi e os dados do InfluxDB (URL, token, org, bucket).
4. Compile e faça upload para o ESP32.

### Backend (caso exista script local)
- ...

### Dashboard (Frontend)

Site InfluxDB com a criação de uma conta, bucket e API key

##  Uso de Inteligência Artificial

### Ferramentas utilizadas
- ChatGPT (OpenAI) e DeepSeek – auxiliou na geração do código do firmware, explicação da API do InfluxDB e criação da documentação.

### Prompts importantes
influxdb local para influxdb Cloud; Roteiro para o vídeo do projeto; 

1. *"Crie um código ESP32 que leia DHT11, LDR, mostre em OLED e envie via HTTPS para InfluxDB Cloud. Inclua LED e buzzer para feedback."*
2. *"Explique como funciona o Line Protocol do InfluxDB e monte uma linha de exemplo para temperatura, umidade e luminosidade."*
3. *"Como estruturar um README no GitHub que documente o uso de IA, com seções de ferramentas, prompts, dificuldades e validação?"*

### Dificuldades encontradas
- Entender a sintaxe exata do Line Protocol para múltiplos fields e tags.
- Configurar a requisição HTTPS no ESP32 sem verificação de certificado (apenas para testes).
- Ajustar o controle não bloqueante do buzzer usando `millis()` – a IA sugeriu uma máquina de estados simples que funcionou bem.
- A IA inicialmente gerou um código com `delay()` para o buzzer; precisei pedir para refatorar para abordagem não bloqueante.

### Como validei as respostas da IA
- Testei o código no hardware real (ESP32) e verifiquei cada funcionalidade separadamente (WiFi, leitura de sensores, envio HTTP, display).
- Confrontei as sugestões com a documentação oficial do InfluxDB e do ESP32.
- Ajustei parâmetros manualmente quando a resposta não funcionou de primeira (ex: a resolução do ADC para 13 bits).

### Reflexão crítica sobre o uso da IA
O uso da IA acelerou significativamente o desenvolvimento, especialmente na parte de protocolo e na estrutura inicial do código. No entanto, foi necessário um bom entendimento dos conceitos para filtrar sugestões incorretas ou incompletas (como o uso de `delay` em um sistema que precisa ser responsivo). A IA se mostrou uma excelente ferramenta de aprendizado, mas não substitui a validação prática e a leitura da documentação oficial. A documentação do uso da IA também se mostrou um exercício valioso para refletir sobre o processo de desenvolvimento.

##  Vídeo de apresentação

O vídeo demonstrativo está disponível no diretório `video/` (ou no Drive: https://drive.google.com/file/d/1thFSmx-BkKNNvX7ngyCz06wlS_jq2Y6I/view?usp=sharing).

##  Autor

Gabriel Felix Milen de Albuquerque
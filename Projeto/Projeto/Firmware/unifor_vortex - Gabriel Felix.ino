#include <WiFi.h>
#include <WiFiClientSecure.h>   // Adicionado para HTTPS
#include <HTTPClient.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Configurações de Wi-Fi ---
const char* ssid = "S22 Ultra";
const char* password = "uthh8288";

// --- Configurações do InfluxDB Cloud ---
// IMPORTANTE: Substitua pela URL da sua instância (sem barra no final)
const char* influxUrl = "https://us-east-1-1.aws.cloud2.influxdata.com";  // Exemplo
const char* influxOrg = "Dev team";
const char* influxBucket = "Ambiente";
const char* influxToken = "XOz6bUxxTzBgzcQGNkufn9MF91Bqn6Y_xGJ2RPpEkDi2Q0gVwVZoTYYmjZypvZlA_3e0nwyWIWgpWVhoEmEr7Q==";

// --- Sensor DHT11 onboard ---
#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- LDR onboard ---
#define LDR_PIN 1

// --- Buzzer onboard ---
#define BUZZER_PIN 17

// --- LED RGB onboard ---
#define LED_BLUE  12
#define LED_GREEN 13
#define LED_RED   14

// --- Display OLED onboard ---
#define SDA_PIN 8
#define SCL_PIN 9
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Temporização ---
unsigned long ultimoEnvio = 0;
const unsigned long intervaloEnvio = 3000;

String ultimoStatus = "Aguardando...";

// Controle do buzzer para feedback sonoro
bool buzzerAtivo = false;
int etapaBuzzer = 0;
const int totalBips = 3;
unsigned long ultimaMudancaBuzzer = 0;
const unsigned long duracaoBip = 120;

// Função para codificar URL (necessária para org/bucket com espaços)
String urlEncode(const String& str) {
  String encoded = "";
  char c;
  for (unsigned int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      encoded += c;
    } else {
      encoded += '%';
      encoded += String(c, HEX);  // simplificado; funciona para caracteres básicos
    }
  }
  return encoded;
}

// Converte valor bruto do LDR (0-8191) para percentual de luminosidade
float converterLuminosidadeParaPercentual(int valorBruto) {
  return (valorBruto / 8191.0) * 100.0;
}

// Controle de LEDs
void apagarLeds() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
}

void acenderVerde() {
  apagarLeds();
  digitalWrite(LED_GREEN, HIGH);
}

void acenderVermelho() {
  apagarLeds();
  digitalWrite(LED_RED, HIGH);
}

// Inicia sequência de bipes de sucesso
void iniciarBipsSucesso() {
  buzzerAtivo = true;
  etapaBuzzer = 0;
  ultimaMudancaBuzzer = millis();
  digitalWrite(BUZZER_PIN, HIGH);
}

// Máquina de estados para alternar o buzzer
void atualizarBuzzer() {
  if (!buzzerAtivo) return;

  if (millis() - ultimaMudancaBuzzer >= duracaoBip) {
    ultimaMudancaBuzzer = millis();
    bool estadoAtual = digitalRead(BUZZER_PIN);
    digitalWrite(BUZZER_PIN, !estadoAtual);

    if (estadoAtual == HIGH) {
      etapaBuzzer++;
      if (etapaBuzzer >= totalBips) {
        buzzerAtivo = false;
        digitalWrite(BUZZER_PIN, LOW);
      }
    }
  }
}

// Atualiza o display OLED com as leituras e status
void atualizarDisplay(float temperatura, float umidade, float luminosidade, String status) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("Gabriel Unifor_VORTEX");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  display.setCursor(0, 16);
  display.setTextSize(2);
  display.print(temperatura, 1);
  display.println(" C");


  display.setTextSize(1);
  display.setCursor(0, 34);
  display.print("Umidade: ");
  display.print(umidade, 1);
  display.println(" %");

  display.setCursor(0, 44);
  display.print("Luz: ");
  display.print(luminosidade, 1);
  display.println(" %");

  display.setCursor(0, 56);
  display.print(status);

  display.display();
}

// Configuração inicial
void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  apagarLeds();

  analogReadResolution(13);   // LDR com resolução de 13 bits (0-8191)

  Wire.begin(SDA_PIN, SCL_PIN);

  // Inicializa display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Erro ao iniciar o display OLED!");
  } else {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Conectando ao WiFi...");
    display.display();
  }

  // Conexão Wi-Fi
  Serial.print("Conectando ao Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi conectado!");
  display.println(WiFi.localIP());
  display.display();
  delay(1500);
}

void loop() {
  atualizarBuzzer();   // mantém a sequência de bipes, se ativa

  if (millis() - ultimoEnvio >= intervaloEnvio) {
    ultimoEnvio = millis();

    // Leitura dos sensores
    float temperatura = dht.readTemperature();
    float umidade = dht.readHumidity();
    int luminosidadeBruta = analogRead(LDR_PIN);
    float luminosidade = converterLuminosidadeParaPercentual(luminosidadeBruta);

    // Verifica falha no DHT
    if (isnan(temperatura) || isnan(umidade)) {
      Serial.println("Erro: falha na leitura do DHT11!");
      ultimoStatus = "Erro no sensor!";
      acenderVermelho();
      atualizarDisplay(0, 0, luminosidade, ultimoStatus);
      return;
    }

    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print(" °C | Umidade: ");
    Serial.print(umidade);
    Serial.print(" % | Luminosidade: ");
    Serial.print(luminosidade);
    Serial.println(" %");

    // Envia dados para o InfluxDB Cloud
    enviarParaInfluxDB(temperatura, umidade, luminosidade);

    atualizarDisplay(temperatura, umidade, luminosidade, ultimoStatus);
  }
}

// Função que envia dados via HTTPS para o InfluxDB Cloud
void enviarParaInfluxDB(float temperatura, float umidade, float luminosidade) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi desconectado, pulando envio.");
    ultimoStatus = "WiFi desconectado";
    acenderVermelho();
    return;
  }

  // Cria cliente seguro (HTTPS)
  WiFiClientSecure client;
  client.setInsecure();   // Ignora verificação do certificado (apenas para testes!)

  HTTPClient http;

  // Monta URL completa para escrita no InfluxDB Cloud
  String url = String(influxUrl) + "/api/v2/write?org=" + urlEncode(influxOrg) +
               "&bucket=" + urlEncode(influxBucket) + "&precision=s";

  http.begin(client, url);   // usa o cliente seguro
  http.setTimeout(5000);
  http.addHeader("Authorization", "Token " + String(influxToken));
  http.addHeader("Content-Type", "text/plain; charset=utf-8");

  // Linha de protocolo do InfluxDB
  String linhaProtocolo = "sensor_ambiente,device=franzininho_lab01,local=maquina1 temperatura="
                          + String(temperatura, 1) + ",umidade=" + String(umidade, 1)
                          + ",luminosidade=" + String(luminosidade, 1);

  int httpCode = http.POST(linhaProtocolo);

  if (httpCode == 204) {
    Serial.println("✅ Dado enviado com sucesso!");
    ultimoStatus = "Enviado com sucesso";
    iniciarBipsSucesso();
    acenderVerde();
  } else if (httpCode > 0) {
    Serial.print("❌ Erro ao enviar. Código HTTP: ");
    Serial.println(httpCode);
    Serial.println(http.getString());
    ultimoStatus = "Erro HTTP " + String(httpCode);
    acenderVermelho();
  } else {
    Serial.print("❌ Falha de conexão: ");
    Serial.println(http.errorToString(httpCode));
    ultimoStatus = "Falha de conexao";
    acenderVermelho();
  }

  http.end();
}
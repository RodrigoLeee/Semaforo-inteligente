#include <WiFi.h>
#include <PubSubClient.h>

// Configurações Wi-Fi
const char* ssid = "Sla";
const char* password = "bananadepijama30";

// Configurações MQTT
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic_control = "traffic/control";
const char* mqtt_topic_status = "traffic/status";

WiFiClient espClient;
PubSubClient client(espClient);

// Pinos dos LEDs
const int ledVerde = 23;
const int ledAmarelo = 22;
const int ledVermelho = 21;

// Variáveis de estado
bool modoPiscarAmarelo = false;
unsigned long ultimoPiscar = 0;
const unsigned long intervaloPiscar = 500; // Intervalo de piscar em milissegundos
bool estadoAmarelo = false;

void setup() {
  Serial.begin(115200);

  // Configura os pinos dos LEDs como saída
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);

  // Conecta ao Wi-Fi
  setup_wifi();

  // Configura o cliente MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)message[i];
  }

  Serial.print("Mensagem recebida em [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);

  if (String(topic) == mqtt_topic_control) {
    if (msg == "rua_verde") {
      // Muda o semáforo da avenida para vermelho
      modoPiscarAmarelo = false; // Certifique-se de que não está em modo piscar amarelo
      digitalWrite(ledVerde, LOW);
      digitalWrite(ledAmarelo, LOW);
      digitalWrite(ledVermelho, HIGH);

      // Envia confirmação de status
      client.publish(mqtt_topic_status, "avenida_vermelho");
    } else if (msg == "avenida_verde") {
      // Muda o semáforo da avenida para verde
      modoPiscarAmarelo = false;
      digitalWrite(ledVerde, HIGH);
      digitalWrite(ledAmarelo, LOW);
      digitalWrite(ledVermelho, LOW);

      // Envia confirmação de status
      client.publish(mqtt_topic_status, "avenida_verde");
    } else if (msg == "piscar_amarelo") {
      // Inicia piscar amarelo
      modoPiscarAmarelo = true;
    } else if (msg == "modo_normal") {
      // Retorna ao modo normal
      modoPiscarAmarelo = false;
      // Define o semáforo para o estado padrão (verde)
      digitalWrite(ledVerde, HIGH);
      digitalWrite(ledAmarelo, LOW);
      digitalWrite(ledVermelho, LOW);
    }
  }
}

void reconnect() {
  // Loop até reconectar
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (client.connect("ESP32_Avenida")) {
      Serial.println("Conectado");
      client.subscribe(mqtt_topic_control);
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (modoPiscarAmarelo) {
    unsigned long agora = millis();
    if (agora - ultimoPiscar >= intervaloPiscar) {
      ultimoPiscar = agora;
      estadoAmarelo = !estadoAmarelo;
      digitalWrite(ledVerde, LOW);
      digitalWrite(ledVermelho, LOW);
      digitalWrite(ledAmarelo, estadoAmarelo ? HIGH : LOW);
    }
  }
}

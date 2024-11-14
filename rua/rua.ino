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

// Pinos dos LEDs e LDRs
const int ledVerde = 23;
const int ledAmarelo = 22;
const int ledVermelho = 21;
const int ldrRua = 36;
const int ldrCalcada = 39;

// Limiar para detecção
const int limiarCarro = 2500;    // Ajuste conforme necessário
const int limiarDiaNoite = 200;  // Ajuste conforme necessário

// Variáveis de estado
bool modoPiscarAmarelo = false;
unsigned long ultimoPiscar = 0;
const unsigned long intervaloPiscar = 500;  // Intervalo de piscar em milissegundos
bool estadoAmarelo = false;

String msg_at = "";
void setup() {
  Serial.begin(115200);

  // Configura os pinos dos LEDs como saída
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);

  // Configura os pinos dos LDRs como entrada
  pinMode(ldrRua, INPUT);
  pinMode(ldrCalcada, INPUT);

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



  if (String(topic) == mqtt_topic_status) {
    if (msg == "avenida_vermelho" && msg != msg_at) {
      // Avenida está vermelha, rua pode ficar verde
      modoPiscarAmarelo = false;
      digitalWrite(ledVermelho, LOW);
      digitalWrite(ledAmarelo, LOW);
      digitalWrite(ledVerde, HIGH);
      Serial.println("verdeee");

    } else if (msg == "avenida_verde") {
      // Avenida voltou a verde, rua fica vermelha
      modoPiscarAmarelo = false;
      digitalWrite(ledVerde, LOW);
      digitalWrite(ledAmarelo, LOW);
      digitalWrite(ledVermelho, HIGH);
    }
    msg_at = msg;
  }
}

void reconnect() {
  // Loop até reconectar
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (client.connect("ESP32_Rua")) {
      Serial.println("Conectado");
      client.subscribe(mqtt_topic_status);
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

  int valorLDRRua = analogRead(ldrRua);
  int valorLDRCalcada = analogRead(ldrCalcada);

  Serial.print("LDR Rua: ");
  Serial.println(valorLDRRua);
  Serial.print("LDR Calçada: ");
  Serial.println(valorLDRCalcada);

  // Verifica se é noite
  if (valorLDRCalcada < limiarDiaNoite) {
    if (!modoPiscarAmarelo) {
      modoPiscarAmarelo = true;
      // Envia comando para piscar amarelo
      client.publish(mqtt_topic_control, "piscar_amarelo");
    }

    // Piscar amarelo
    unsigned long agora = millis();
    if (agora - ultimoPiscar >= intervaloPiscar) {
      ultimoPiscar = agora;
      estadoAmarelo = !estadoAmarelo;
      digitalWrite(ledVerde, LOW);
      digitalWrite(ledVermelho, LOW);
      digitalWrite(ledAmarelo, estadoAmarelo ? HIGH : LOW);
    }
  } else {
    if (modoPiscarAmarelo) {
      modoPiscarAmarelo = false;
      // Envia comando para voltar ao modo normal
      client.publish(mqtt_topic_control, "modo_normal");
      // Configura semáforo da rua para vermelho
      digitalWrite(ledVerde, LOW);
      digitalWrite(ledAmarelo, LOW);
      digitalWrite(ledVermelho, HIGH);
      Serial.println("Vermelho");
    }

    if (!modoPiscarAmarelo) {
      // Verifica se há carro na rua
      if (valorLDRRua < limiarCarro) {
        // Envia comando para fechar avenida e abrir rua
        client.publish(mqtt_topic_control, "rua_verde");
        // Espera confirmação e mantém o estado atual
      } else if (valorLDRCalcada > limiarDiaNoite) {
        // Envia comando para abrir avenida e fechar rua
        client.publish(mqtt_topic_control, "avenida_verde");
        // Mantém o semáforo da rua vermelho
        digitalWrite(ledVerde, LOW);
        digitalWrite(ledAmarelo, LOW);
        digitalWrite(ledVermelho, HIGH);
      } else {
        digitalWrite(ledVermelho, LOW);
      }
    }
  }

  delay(100);  // Ajuste o tempo conforme necessário
}

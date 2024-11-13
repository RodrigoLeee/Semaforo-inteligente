#include "UbidotsEsp32Mqtt.h"

/****************************************
 * Define Constants
 ****************************************/
const char *UBIDOTS_TOKEN = "BBUS-dxyl1ZC7D6TrGedjnwYUND2n789lJA"; //pegue o token no menu vertical esquerdo de quando vc criou o "black device"
const char *WIFI_SSID = "Inteli.Iot";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "@Intelix10T#";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "testeGodoi";   // pegue o nome do device que você criou do "blank device"
const char *VARIABLE_LABEL1 = "potenciometro"; // Put here your Variable label to which data  will be published
const char *VARIABLE_LABEL2 = "botao"; // Put here your Variable label to which data  will be published

const int PUBLISH_FREQUENCY = 6000; // Update rate in milliseconds

unsigned long timer;
uint8_t pinPotenciometro = 34;
uint8_t pinBotao = 33;
uint8_t pinLED = 2;
bool statusLED = false;

Ubidots ubidots(UBIDOTS_TOKEN);

/****************************************
 * Auxiliar Functions
 ****************************************/

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/****************************************
 * Main Functions
 ****************************************/

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  ubidots.setDebug(true);  // uncomment this to make debug messages available
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  pinMode(pinBotao, INPUT_PULLUP);
  pinMode(pinPotenciometro, INPUT);
  pinMode(pinLED, OUTPUT);

  timer = millis();
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }
  if (millis() - timer > PUBLISH_FREQUENCY) // triggers the routine every X seconds
  {
    float value1 = 12;
    bool value2 = true;
    Serial.println(value1);
    Serial.println(value2);
    
    ubidots.add(VARIABLE_LABEL1, value1); // Insert your variable Labels and the value to be sent
    ubidots.publish(DEVICE_LABEL);
    ubidots.add(VARIABLE_LABEL2, value2); // Insert your variable Labels and the value to be sent
    ubidots.publish(DEVICE_LABEL);
    timer = millis();
    //statusLED = !statusLED;
    //digitalWrite(pinLED, statusLED);
    
  }
  ubidots.loop();
}
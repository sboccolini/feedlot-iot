// nodo_energia.ino — Control electrificador (RF1)
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid        = "TU_WIFI";
const char* password    = "TU_PASSWORD";
const char* mqtt_server = "IP_DE_TU_PC";

#define PIN_SENSOR_RED    34
#define PIN_RELE_BAT      26
#define PIN_RELE_CARGADOR 27
#define PIN_ADC_BAT       35
#define ADC_FACTOR  (3.3f / 4095.0f * 4.0f)

WiFiClient   espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];

  if (String(topic) == "feedlot/energia/rele_bateria/set") {
    digitalWrite(PIN_RELE_BAT, msg == "ON" ? HIGH : LOW);
    client.publish("feedlot/energia/rele_bateria/estado", msg == "ON" ? "ON" : "OFF", true);
  }
  if (String(topic) == "feedlot/energia/rele_cargador/set") {
    digitalWrite(PIN_RELE_CARGADOR, msg == "ON" ? HIGH : LOW);
    client.publish("feedlot/energia/rele_cargador/estado", msg == "ON" ? "ON" : "OFF", true);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP_Energia_Feedlot")) {
      client.subscribe("feedlot/energia/rele_bateria/set");
      client.subscribe("feedlot/energia/rele_cargador/set");
    } else {
      delay(3000);
    }
  }
}

void setup() {
  pinMode(PIN_SENSOR_RED,    INPUT);
  pinMode(PIN_RELE_BAT,      OUTPUT); digitalWrite(PIN_RELE_BAT,      LOW);
  pinMode(PIN_RELE_CARGADOR, OUTPUT); digitalWrite(PIN_RELE_CARGADOR, LOW);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    bool redOk = digitalRead(PIN_SENSOR_RED);
    client.publish("feedlot/energia/red_220v", redOk ? "OK" : "CORTE", true);
    int raw = analogRead(PIN_ADC_BAT);
    float voltaje = raw * ADC_FACTOR;
    client.publish("feedlot/energia/bateria_v", String(voltaje, 2).c_str());
  }
}

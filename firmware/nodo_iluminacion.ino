// nodo_iluminacion.ino — Control iluminación manga (RF4)
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid        = "TU_WIFI";
const char* password    = "TU_PASSWORD";
const char* mqtt_server = "IP_DE_TU_PC";

#define PIN_RELE_LUCES 26

WiFiClient   espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  digitalWrite(PIN_RELE_LUCES, msg == "ON" ? HIGH : LOW);
  client.publish("feedlot/iluminacion/manga/estado", msg == "ON" ? "ON" : "OFF", true);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP_Iluminacion_Feedlot"))
      client.subscribe("feedlot/iluminacion/manga/set");
    else delay(3000);
  }
}

void setup() {
  pinMode(PIN_RELE_LUCES, OUTPUT);
  digitalWrite(PIN_RELE_LUCES, LOW);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}

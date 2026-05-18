// nodo_tanque.ino — Control tanque de agua (RF2)
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid        = "TU_WIFI";
const char* password    = "TU_PASSWORD";
const char* mqtt_server = "IP_DE_TU_PC";

WiFiClient   espClient;
PubSubClient client(espClient);
bool bombaEncendida = false;
unsigned long lastMsg = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  if (String(topic) == "feedlot/tanque/bomba/set") {
    bombaEncendida = (msg == "ON");
    client.publish("feedlot/tanque/bomba/estado", bombaEncendida ? "ON" : "OFF");
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP_Tanque_Feedlot"))
      client.subscribe("feedlot/tanque/bomba/set");
    else delay(3000);
  }
}

void setup() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    // Reemplazar con lectura real del sensor de nivel
    int nivel = analogRead(34) * 100 / 4095;
    client.publish("feedlot/tanque/nivel", String(nivel).c_str());
  }
}

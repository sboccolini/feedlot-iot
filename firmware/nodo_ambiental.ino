// nodo_ambiental.ino — Temperatura, humedad y bebederos (RF3)
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

const char* ssid        = "TU_WIFI";
const char* password    = "TU_PASSWORD";
const char* mqtt_server = "IP_DE_TU_PC";

#define DHT_PIN  4
#define DHT_TYPE DHT22

const int   PINES_BEB[]  = {32, 33, 25};
const char* CORRALES[]   = {"corral_1", "corral_2", "corral_3"};
const int   N_BEB        = 3;

WiFiClient   espClient;
PubSubClient client(espClient);
DHT dht(DHT_PIN, DHT_TYPE);
unsigned long lastMsg = 0;

void reconnect() {
  while (!client.connected())
    if (!client.connect("ESP_Ambiental_Feedlot")) delay(3000);
}

void setup() {
  dht.begin();
  for (int i = 0; i < N_BEB; i++) pinMode(PINES_BEB[i], INPUT_PULLUP);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
  unsigned long now = millis();
  if (now - lastMsg > 15000) {
    lastMsg = now;
    float t = dht.readTemperature(), h = dht.readHumidity();
    if (!isnan(t) && !isnan(h)) {
      client.publish("feedlot/ambiental/temperatura", String(t, 1).c_str());
      client.publish("feedlot/ambiental/humedad",     String(h, 1).c_str());
    }
    for (int i = 0; i < N_BEB; i++) {
      char topic[40];
      snprintf(topic, sizeof(topic), "feedlot/bebederos/%s", CORRALES[i]);
      client.publish(topic, digitalRead(PINES_BEB[i]) ? "OK" : "VACIO");
    }
  }
}

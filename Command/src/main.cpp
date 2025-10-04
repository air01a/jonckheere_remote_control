#include <Arduino.h>
#include <WiFi.h>
#include "Network.h"

const char* ssid = "JONCKHEEREMOUNT";
const char* password = "12345678";
int i=0;
// IP du serveur à modifier
Network network("192.168.1.1", 4000);

void setup() {
    Serial.begin(115200);

    WiFi.begin(ssid, password);
    Serial.print("Connexion WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connecté");

    network.begin(8000); // démarre UDP sur port 8000 local
    JsonDocument doc;   // plus besoin de StaticJsonDocument<256>
    doc["type"] = "register";
    doc["client_id"] = "ESP32_001";

    network.sendJson(doc);

}

void loop() {


  if (i%1000==0) {
    JsonDocument doc; 
    doc["type"] = "ping";
    doc["client_id"] = "ESP32_001";

    if (network.sendJson(doc)) {
      Serial.println("Ping envoyé !");
    }
  }

  i+=1;
  
  NetworkMessage msg = network.readMessage();
    if (msg.valid) {
        Serial.println("📩 Notification reçue :");
        Serial.println("  Type      : " + msg.type);
        Serial.println("  Message   : " + msg.message);
        Serial.println("  Parameters: " + msg.parameters);
    } else {
        Serial.print(".");
    }

  delay(10);
}

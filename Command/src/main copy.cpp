#include <Arduino.h>
#include <WiFi.h>
#include "Network.h"
#define PIN_DATA   4   // Q7 (serial output du dernier 74HC165)
#define PIN_CLOCK  19   // Horloge pour décaler les bits
#define PIN_LATCH   5   // /PL (load) pour capturer les valeurs


const char* ssid = "JONCKHEEREMOUNT";
const char* password = "12345678";
int i=0;
// IP du serveur à modifier
//Network network("192.168.1.1", 4000);
uint16_t lastValue = 0xFFFF;  // Valeur initiale (assume tout à 1, à ajuster selon ton câblage)

void setup() {
    Serial.begin(115200);
  /*
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

*/
    Serial.println("Lecture des interrupteurs 74HC165");

    pinMode(PIN_DATA, INPUT);
    pinMode(PIN_CLOCK, OUTPUT);
    pinMode(PIN_LATCH, OUTPUT);

    digitalWrite(PIN_CLOCK, LOW);
    digitalWrite(PIN_LATCH, HIGH);
    Serial.println("Lecture des interrupteurs 74HC165");
}

uint16_t readSwitches() {
  uint16_t switches = 0;

  // Capture les états des interrupteurs
  digitalWrite(PIN_LATCH, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_LATCH, HIGH);

  // Lecture des 16 bits en série
  for (int i = 0; i < 16; i++) {
    // Lire le bit courant
    int bitValue = digitalRead(PIN_DATA);

    // Insérer dans le 16 bits (le premier lu est le bit le plus significatif)
    switches |= (bitValue << (15 - i));

    // Impulsion clock pour obtenir le bit suivant
    digitalWrite(PIN_CLOCK, HIGH);
    delayMicroseconds(2);
    digitalWrite(PIN_CLOCK, LOW);
    delayMicroseconds(2);
  }

  return switches;
}

void loop() {
/*

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
*/
    uint16_t current = readSwitches();

    // Détection des changements bit par bit
    uint16_t diff = current ^ lastValue;

    if (diff != 0) {
      Serial.println("Changements détectés :");
      for (int i = 15; i >= 0; i--) {
      Serial.print((current & (1 << i)) ? '1' : '0');
    }
    Serial.println();

    for (int i = 0; i < 16; i++) {
      if (diff & (1 << i)) {
        int newValue = (current & (1 << i)) ? 1 : 0;
        Serial.print("Bit ");
        Serial.print(i);
        Serial.print(" → ");
        Serial.println(newValue);
      }
    }

    Serial.println();  
  }

  lastValue = current;
  delay(50);
  delay(1500);

}

#include <Arduino.h>
#include <WiFi.h>
#include "Network.h"
#define PIN_DATA   4   // Q7 (serial output du dernier 74HC165)
#define PIN_CLOCK  19   // Horloge pour décaler les bits
#define PIN_LATCH   5   // /PL (load) pour capturer les valeurs
#define PIN_ENABLE 21
#define PIN_LED1   13
#define PIN_LED2   12
const char* ssid = "JONCKHEEREMOUNT";
const char* password = "12345678";
#include "Network.h"

uint16_t lastValue = 0;
Network network("192.168.1.1", 4000);
int i=0;
boolean hasBeenConnected = false;
// Structure pour définir le comportement de chaque bit
struct BitAction {
  const char* command;
  const char* paramOn;
  const char* paramOff;
};

// Table de configuration pour les bits 4-15
const BitAction bitActions[] = {
  /* bit 4  */ {"direction", "freeze",   "unfreeze"},
  /* bit 5  */ {"direction", "left",     "stop"},
  /* bit 6  */ {"direction", "right",    "stop"},
  /* bit 7  */ {"direction", "down",     "stop"},
  /* bit 8  */ {"direction", "up",       "stop"},
  /* bit 9  */ {nullptr, nullptr, nullptr},  // géré séparément (processBit9)
  /* bit 10 */ {"coupole",   "left",     "stop"},
  /* bit 11 */ {"coupole",   "right",    "stop"},
  // bits 12-15 non utilisés
  /* bit 12 */ {nullptr, nullptr, nullptr},
  /* bit 13 */ {nullptr, nullptr, nullptr},
  /* bit 14 */ {nullptr, nullptr, nullptr},
  /* bit 15 */ {nullptr, nullptr, nullptr},
};

// -------------------------------------------------------------------
//  RX
// -------------------------------------------------------------------
void sendUdpCommand(const char* type, const char* cmd = "", const char* prarams = "") {
  JsonDocument doc; 
  doc["type"] = type;
  doc["command"] = cmd;
  doc["parameters"] = prarams;
  doc["client_id"] = "ESP32_001";
  Serial.println(prarams);
  if (!network.sendJson(doc)) {
    Serial.println("#### Error sending UDP command ####");
  }
}

// -------------------------------------------------------------------
//  LECTURE 16 BITS
// -------------------------------------------------------------------
uint16_t readSwitches() {
  uint16_t switches = 0;

  // Capture instantanée
  digitalWrite(PIN_LATCH, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_LATCH, HIGH);

  // Lecture série
  for (int i = 0; i < 16; i++) {
    int bitValue = digitalRead(PIN_DATA);
    switches |= (bitValue << (15 - i));

    digitalWrite(PIN_CLOCK, HIGH);
    delayMicroseconds(2);
    digitalWrite(PIN_CLOCK, LOW);
    delayMicroseconds(2);
  }

  return switches;
}

// -------------------------------------------------------------------
//  GESTION DES BITS SPÉCIAUX 0–1  (3 positions : solaire / sidéral / lunaire)
// -------------------------------------------------------------------
void processMode01(uint16_t value) {
  int b0 = (value >> 0) & 1;
  int b1 = (value >> 1) & 1;

  if (b0 == 0 && b1 == 0) {
    sendUdpCommand("command", "mode", "solar");
  }
  else if (b0 == 1 && b1 == 0) {
    sendUdpCommand("command", "mode", "sidereal");
  }
  else if (b0 == 0 && b1 == 1) {
    sendUdpCommand("command", "mode", "lunar");
  }
}

// -------------------------------------------------------------------
//  GESTION DES BITS SPÉCIAUX 2–3  (v1 / v2 / v3)
// -------------------------------------------------------------------
void processMode23(uint16_t value) {
  int b2 = (value >> 2) & 1;
  int b3 = (value >> 3) & 1;

  if (b2 == 0 && b3 == 1) {
    sendUdpCommand("command", "multiplier", "x2");
  }
  else if (b2 == 1 && b3 == 1) {
    sendUdpCommand("command", "multiplier", "x4");
  }
  else if (b2 == 1 && b3 == 0) {
    sendUdpCommand("command", "multiplier", "x16");
  }
}

// -------------------------------------------------------------------
//  GESTION DU BIT SPECIAL 9  (DEC SLOW / DEC FAST)
// -------------------------------------------------------------------
void processBit9(uint16_t value) {
  int b9 = (value >> 9) & 1;

  if (b9 == 1) {
    sendUdpCommand("command", "decspeed", "slow");
  } else {
    sendUdpCommand("command", "decspeed", "fast");
  }
}

// -------------------------------------------------------------------
//  ENVOI INITIALE DE L’ÉTAT DES 4 BITS SPÉCIAUX
// -------------------------------------------------------------------
void sendInitialSpecialModes(uint16_t value) {
  processMode01(value);
  processMode23(value);
  processBit9(value);
}


// -------------------------------------------------------------------
//  DETECTE LA PERTE DU WIFI ET COUPE L'ALIMENTATION
// -------------------------------------------------------------------
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_STA_DISCONNECTED:
      if (hasBeenConnected) {
        Serial.println("⚠️ WiFi perdu, tentative de reconnexion...");
        WiFi.reconnect();
      
        Serial.println("❌ WiFi déconnecté !");
        digitalWrite(PIN_ENABLE, LOW);
      }
      break;

    case SYSTEM_EVENT_STA_CONNECTED:
      Serial.println("✅ WiFi connecté !");
      break;

    default:
      break;
  }
}

// -------------------------------------------------------------------
//  SETUP
// -------------------------------------------------------------------
void setup() {
    Serial.begin(115200);
    pinMode(PIN_ENABLE, OUTPUT);
    digitalWrite(PIN_ENABLE, HIGH); 

    pinMode(PIN_DATA, INPUT);
    pinMode(PIN_CLOCK, OUTPUT);
    pinMode(PIN_LATCH, OUTPUT);
    pinMode(PIN_LED1, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);
    digitalWrite(PIN_CLOCK, LOW);
    digitalWrite(PIN_LATCH, HIGH);
    digitalWrite(PIN_LED1, LOW);
    digitalWrite(PIN_LED2, LOW);
    delay(100);

    // Lecture initiale
    lastValue = readSwitches();
    WiFi.onEvent(WiFiEvent);

    WiFi.begin(ssid, password);
    Serial.print("Connexion WiFi");
    int count=0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        count++;
        if (count>120) {
          Serial.println("\n❌ Échec de la connexion WiFi");
          digitalWrite(PIN_ENABLE, LOW);

        }
    }
    hasBeenConnected = true;
    Serial.println("\nWiFi connecté");

    network.begin(8000); // démarre UDP sur port 8000 local
    JsonDocument doc;   // plus besoin de StaticJsonDocument<256>
    doc["type"] = "register";
    doc["client_id"] = "ESP32_001";

    network.sendJson(doc);
    // → envoyer état initial des 4 bits spéciaux
    sendInitialSpecialModes(lastValue);

    Serial.println("Init OK");
}



// -------------------------------------------------------------------
//  LOOP
// -------------------------------------------------------------------
void loop() {
    if (i%1000==0) {
      sendUdpCommand("ping");
    }

    i+=1;
    // -----------------------------------
    // Read end of course notifications
    // -----------------------------------
    NetworkMessage msg = network.readMessage();
    if (msg.valid) {
      if (msg.type == "endCourse") {
        if (msg.message=="OFF") {
          Serial.println("Extinction des LEDs");
          digitalWrite(PIN_LED1, LOW);
          digitalWrite(PIN_LED2, LOW);
        } else if (msg.parameters=="UP") {
          digitalWrite(PIN_LED2, HIGH);
        } else  {
          digitalWrite(PIN_LED1, HIGH);
        }
      }
        Serial.println("📩 Notification reçue :");
        Serial.println("  Type      : " + msg.type);
        Serial.println("  Message   : " + msg.message);
        Serial.println("  Parameters: " + msg.parameters);
    } else {
        Serial.print(".");
    }

    uint16_t current = readSwitches();

    // Bits qui changent
    uint16_t diff = current ^ lastValue;

    if (diff != 0) {
      // ------------------------
      // Bits 0–1 : Mode sidéral/lunaire/solaire
      // ------------------------
      if (diff & 0b0000000000000011) {   // si b0 ou b1 a changé
        processMode01(current);
      }

      // ------------------------
      // Bits 2–3 : v1 / v2 / v3
      // ------------------------
      if (diff & 0b0000000000001100) {   // si b2 ou b3 a changé
        processMode23(current);
      }

      // ------------------------
      // Bits 9  dec fast / slow
      // ------------------------

      if (diff & 0b0000001000000000) {   // si b9 a changé
        processBit9(current);
      }
      // ------------------------
      // Autres bits 4–15
      // ------------------------
      for (int bit = 4; bit < 16; bit++) {
        if (bit == 9) continue;  // géré par processBit9
        if (diff & (1 << bit)) {
          const BitAction& action = bitActions[bit - 4];
          if (action.command != nullptr) {
            bool isOn = (current & (1 << bit)) != 0;
            sendUdpCommand("command", action.command, !isOn ? action.paramOn : action.paramOff);
          }
        }
      }
    }
    lastValue = current;
    delay(20);
}


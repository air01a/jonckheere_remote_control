#include <Arduino.h>
#include <WiFi.h>
#include "Network.h"
#define PIN_DATA   4   // Q7 (serial output du dernier 74HC165)
#define PIN_CLOCK  19   // Horloge pour décaler les bits
#define PIN_LATCH   5   // /PL (load) pour capturer les valeurs
uint16_t lastValue = 0;

// -------------------------------------------------------------------
//  RX
// -------------------------------------------------------------------
void sendUdpCommand(const char* cmd) {
  Serial.print("SEND: ");
  Serial.println(cmd);

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
    sendUdpCommand("SOLAIRE");
  }
  else if (b0 == 1 && b1 == 0) {
    sendUdpCommand("SIDERAL");
  }
  else if (b0 == 0 && b1 == 1) {
    sendUdpCommand("LUNAIRE");
  }
}



// -------------------------------------------------------------------
//  GESTION DES BITS SPÉCIAUX 2–3  (v1 / v2 / v3)
// -------------------------------------------------------------------
void processMode23(uint16_t value) {
  int b2 = (value >> 2) & 1;
  int b3 = (value >> 3) & 1;

  if (b2 == 0 && b3 == 1) {
    sendUdpCommand("v1");
  }
  else if (b2 == 1 && b3 == 1) {
    sendUdpCommand("v2");
  }
  else if (b2 == 1 && b3 == 0) {
    sendUdpCommand("v3");
  }
}



// -------------------------------------------------------------------
//  ENVOI INITIALE DE L’ÉTAT DES 4 BITS SPÉCIAUX
// -------------------------------------------------------------------
void sendInitialSpecialModes(uint16_t value) {
  processMode01(value);
  processMode23(value);
}



// -------------------------------------------------------------------
//  SETUP
// -------------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  pinMode(PIN_DATA, INPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_LATCH, OUTPUT);

  digitalWrite(PIN_CLOCK, LOW);
  digitalWrite(PIN_LATCH, HIGH);

  delay(100);

  // Lecture initiale
  lastValue = readSwitches();

  // → envoyer état initial des 4 bits spéciaux
  sendInitialSpecialModes(lastValue);

  Serial.println("Init OK");
}



// -------------------------------------------------------------------
//  LOOP
// -------------------------------------------------------------------
void loop() {
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
    // Autres bits 4–15
    // ------------------------
    for (int bit = 4; bit < 16; bit++) {
      if (diff & (1 << bit)) {
        int newValue = (current & (1 << bit)) ? 1 : 0;

        if (bit == 5) {
          if (newValue == 1) sendUdpCommand("CMD51");
          else               sendUdpCommand("CMD52");
        }

        else if (bit == 10) {
          if (newValue == 1) sendUdpCommand("CLEAR01");
          else               sendUdpCommand("SET01");
        }


      }
    }
  }

  lastValue = current;
  delay(20);
}

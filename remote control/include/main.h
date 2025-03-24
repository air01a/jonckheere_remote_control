#ifndef MYAPP_CLIENT_H
#define MYAPP_CLIENT_H


#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <vector>
#include <Wire.h>
#include "command.h"

#define SSID "JONCKHEEREMOUNT"
#define PASSWORD "12345678"

//#define CLOCKSIMULATOR 1

#define SSID "JONCKHEEREMOUNT"
#define PASSWORD "12345678"
#define LED_BUILTIN 1
#define LOCAL_UDP_PORT 4000
// Configuration de l'IP statique
IPAddress local_IP(192, 168, 1, 1);        // IP fixe
IPAddress gateway(192, 168, 1, 1);         // Passerelle
IPAddress subnet(255, 255, 255, 0);        // Masque de sous-réseau
IPAddress broadcast(192,168,1,255);


namespace MyApp {
    struct Client {
      IPAddress ip;
      uint16_t port;
      String id;
      unsigned long lastSeen;
    };
};

// Liste des clients connectés
std::vector<MyApp::Client> UdpClients;





Command commands[NUM_COMMANDS] = {
  {"AD+",ad_plus},
    {"AD-",ad_minus},
    {"AD",ad_stop},
    {"DEC+",dec_plus},
    {"DEC-",dec_minus},
    {"DEC",dec_stop},
    {"sidereal", setSidereal},
    {"lunar", setLunar},
    {"solar", setSolar},
    {"x1",x1},
    {"x2",x2},
    {"x4",x4},
    {"x16",x16},
    {"COU+",cou_plus},
    {"COU-",cou_minus},
    {"COUSTOP",cou_stop},

};

#endif
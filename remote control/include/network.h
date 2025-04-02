#ifndef JONCKHEERE_NETWORK_H
#define JONCKHEERE_NETWORK_H
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <vector>

#define SSID "JONCKHEEREMOUNT"
#define PASSWORD "12345678"

#define LOCAL_UDP_PORT 4000
// Configuration de l'IP statique



namespace NetworkModule {
    struct Client {
      IPAddress ip;
      uint16_t port;
      String id;
      unsigned long lastSeen;
    };

};


void processUdpPacket();
void sendNotificationsToUdpClients(String type, String message, String parameters);
bool startWifi();
void startUdpServer();
void cleanupClient();

#endif
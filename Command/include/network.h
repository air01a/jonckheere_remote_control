#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

struct NetworkMessage {
    String type;
    String message;
    String parameters;
    bool valid;
};

class Network {
public:
    Network(const char* serverIp, uint16_t serverPort = 4000);

    bool begin(uint16_t localPort = 5000);

    bool sendMessage(const String& message);
    bool sendJson(const JsonDocument& doc);

    // 🔥 lit et parse un message JSON (non bloquant)
    NetworkMessage readMessage();

private:
    const char* _serverIp;
    uint16_t _serverPort;
    uint16_t _localPort;

    WiFiUDP _udp;
};

#endif

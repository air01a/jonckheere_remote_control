#include "Network.h"




Network::Network(const char* serverIp, uint16_t serverPort)
: _serverIp(serverIp), _serverPort(serverPort), _localPort(5000) {}

bool Network::begin(uint16_t localPort) {
    _localPort = localPort;
    return _udp.begin(_localPort);
}

bool Network::sendMessage(const String& message) {
    _udp.beginPacket(_serverIp, _serverPort);
    _udp.write((const uint8_t*)message.c_str(), message.length());
    return _udp.endPacket() == 1;
}

bool Network::sendJson(const JsonDocument& doc) {
    String jsonString;
    serializeJson(doc, jsonString);
    return sendMessage(jsonString);
}

NetworkMessage Network::readMessage() {
    NetworkMessage msg = {"", "", "", false};

    int packetSize = _udp.parsePacket();
    if (packetSize > 0) {
        char buffer[512];
        int len = _udp.read(buffer, sizeof(buffer) - 1);
        if (len > 0) {
            buffer[len] = '\0';

            // 🔎 Parsing JSON automatique
            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, buffer);
            if (!err) {
                msg.type = doc["type"] | "unknown";
                msg.message = doc["message"] | "";
                msg.parameters = doc["parameters"] | "";
                msg.valid = true;
            }
        }
    }
    return msg;
}

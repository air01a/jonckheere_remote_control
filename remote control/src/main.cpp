#include "main.h"
// Port UDP
WiFiUDP udp;

unsigned long lastNotificationTime = 0;
const unsigned long notificationInterval = 5000;  // 5 secondes
unsigned long lastCleanupTime = 0;
const unsigned long cleanupInterval = 30000;     // 30 secondes
const unsigned long clientTimeout = 60000;       // 60 secondes

// Tampon pour les paquets UDP
char packetBuffer[1024];

// Fonction pour envoyer une réponse JSON à un client
void sendJsonResponse(IPAddress clientIp, uint16_t clientPort, JsonDocument& doc) {
  String jsonResponse;
  serializeJson(doc, jsonResponse);
  
  udp.beginPacket(clientIp, clientPort);
  udp.print(jsonResponse);
  udp.endPacket();
}

// Fonction pour rechercher ou ajouter un client
int findOrAddClient(IPAddress ip, uint16_t port, String id) {
  // Chercher si le client existe déjà
  for (size_t i = 0; i < UdpClients.size(); i++) {
    if (UdpClients[i].ip == ip && UdpClients[i].id == id) {
      UdpClients[i].lastSeen = millis();
      return i;
    }
  }
  
  // Si non, ajouter un nouveau client
  MyApp::Client newClient;
  newClient.ip = ip;
  newClient.port = port;
  newClient.id = id.isEmpty() ? String("client_") + String(UdpClients.size() + 1) : id;
  newClient.lastSeen = millis();
  
  UdpClients.push_back(newClient);
  Serial.println("Nouveau client enregistré: " + newClient.id + " à " + newClient.ip.toString() + ":" + String(newClient.port));
  
  return UdpClients.size() - 1;
}

// Fonction pour nettoyer les UdpClients inactifs
void cleanupInactiveUdpClients() {
  unsigned long currentTime = millis();
  
  for (size_t i = 0; i < UdpClients.size(); ) {
    if (currentTime - UdpClients[i].lastSeen > clientTimeout) {
      Serial.println("Client expiré: " + UdpClients[i].id + " à " + UdpClients[i].ip.toString());
      UdpClients.erase(UdpClients.begin() + i);
    } else {
      i++;
    }
  }
}

// Fonction pour traiter les commandes reçues
String processCommand(String command, String parameters, IPAddress clientIp, uint16_t clientPort, String clientId) {
  Serial.println("Commande reçue: " + command + " de " + clientId);
  
  // Traiter la commande (exemple)
  String result;
  for (int i = 0; i < NUM_COMMANDS; i++) {
    if (command.compareTo(commands[i].command) == 0) {
        Serial.println(commands[i].command);
        JsonDocument responseDoc;
        responseDoc["type"] = "response";
        responseDoc["status"] = "success";
        responseDoc["message"] = "Commande traitée";
        responseDoc["command_result"] = "OK";      
        commands[i].function(); 
        sendJsonResponse(clientIp, clientPort, responseDoc);
        return responseDoc["command_result"];
      }
  }
  Serial.println("Chaîne inconnue reçue.");
  return "Unknown command";
}

// Fonction pour traiter les paquets UDP entrants
void processUdpPacket() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(packetBuffer, sizeof(packetBuffer) - 1);
    if (len > 0) {
      packetBuffer[len] = 0; // Null-terminate le paquet
      
      // Récupérer l'IP et le port du client
      IPAddress clientIp = udp.remoteIP();
      uint16_t clientPort = udp.remotePort();
      
      // Analyser le JSON
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, packetBuffer);
      
      if (!error) {
        String messageType = doc["type"] | "unknown";
        String clientId = doc["client_id"] | "";
        
        // Mettre à jour ou ajouter le client
        int clientIndex = findOrAddClient(clientIp, clientPort, clientId);
        
        if (messageType == "register") {
          // Répondre à l'enregistrement
          JsonDocument responseDoc;
          responseDoc["type"] = "register_response";
          responseDoc["status"] = "success";
          responseDoc["client_id"] = UdpClients[clientIndex].id;
          
          sendJsonResponse(clientIp, clientPort, responseDoc);
        } 
        else if (messageType == "command") {
          String command = doc["command"] | "";
          String parameters = doc["parameters"] | "";
          processCommand(command, parameters, clientIp, clientPort, UdpClients[clientIndex].id);
        }
        else if (messageType == "ping") {
          // Répondre au ping
          JsonDocument responseDoc;
          responseDoc["type"] = "pong";
          
          sendJsonResponse(clientIp, clientPort, responseDoc);
        }
      } else {
        Serial.println("Erreur de décodage JSON");
      }
    }
  }
}

// Fonction pour envoyer des notifications aux UdpClients
void sendNotificationsToUdpClients() {
  if (UdpClients.empty()) return;
  
  // Créer la notification (exemple: données de capteur)
  JsonDocument  notificationDoc;
  notificationDoc["type"] = "notification";
  notificationDoc["timestamp"] = millis();
  notificationDoc["temperature"] = random(20, 31); // Valeur simulée
  notificationDoc["humidity"] = random(40, 91);    // Valeur simulée
  
  // Pour chaque client, envoyer la notification
  Serial.println("Envoi de notifications à " + String(UdpClients.size()) + " UdpClients:");
  for (const auto& client : UdpClients) {
    Serial.println("- Notification à " + client.id + " à " + client.ip.toString() + ":" + String(client.port));
    sendJsonResponse(client.ip, client.port, notificationDoc);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Connexion au WiFi
  Serial.print("Connexion au WiFi");
  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
    Serial.println("Erreur lors de la configuration de l'IP statique");
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connecté");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());
  
  // Démarrage du serveur UDP
  udp.begin(LOCAL_UDP_PORT);
  Serial.println("Serveur UDP démarré sur le port " + String(LOCAL_UDP_PORT));
}

void loop() {
  // Traiter les paquets UDP entrants
  processUdpPacket();
  
  // Vérifier s'il est temps d'envoyer des notifications
  unsigned long currentTime = millis();
  if (currentTime - lastNotificationTime >= notificationInterval) {
    lastNotificationTime = currentTime;
    sendNotificationsToUdpClients();
  }
  
  // Nettoyer les UdpClients inactifs
  if (currentTime - lastCleanupTime >= cleanupInterval) {
    lastCleanupTime = currentTime;
    cleanupInactiveUdpClients();
  }
}
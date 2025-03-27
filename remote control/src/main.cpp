#include "main.h"
#include "network.h"
// Port UDP


void setup() {
  Serial.begin(115200);
  //pinMode(LED_BUILTIN, OUTPUT);
  initClock();
  
  // Connexion au WiFi
  Serial.println("Création du réseau Wifi");
  startWifi();

  Serial.println("Démarrage du serveur UDP");
  startUdpServer();
}

void loop() {
  // Traiter les paquets UDP entrants
  processUdpPacket();

  cleanupClient();
  // Vérifier s'il est temps d'envoyer des notifications
  
  /*if (currentTime - lastNotificationTime >= notificationInterval) {
    lastNotificationTime = currentTime;
    sendNotificationsToUdpClients();
  }
  
  // Nettoyer les UdpClients inactifs
  if (currentTime - lastCleanupTime >= cleanupInterval) {
    lastCleanupTime = currentTime;
    cleanupInactiveUdpClients();
  }*/

}
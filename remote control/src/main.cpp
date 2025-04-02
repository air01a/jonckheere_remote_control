#include "main.h"
#include "network.h"
#include "command.h"
// Port UDP

volatile bool stateEndOfCourseHasChanged = false;


void IRAM_ATTR interruptFunction() {
  stateEndOfCourseHasChanged = true;
}

void setup() {
  Serial.begin(115200);
  //pinMode(LED_BUILTIN, OUTPUT);
  initClock();
  
  // Connexion au WiFi
  Serial.println("Création du réseau Wifi");
  startWifi();

  Serial.println("Démarrage du serveur UDP");
  startUdpServer();
  pinMode(ENDCOURSE1, INPUT_PULLUP); 
  pinMode(ENDCOURSE2, INPUT_PULLUP); 

  attachInterrupt(digitalPinToInterrupt(ENDCOURSE1), interruptFunction, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENDCOURSE2), interruptFunction, CHANGE);
  bool ec1 = digitalRead(ENDCOURSE1);
  bool ec2 = digitalRead(ENDCOURSE2);
  setEndCourse(ec1, ec2);
}




void loop() {

  // Traiter les paquets UDP entrants
  processUdpPacket();
  if (stateEndOfCourseHasChanged) {
    // Code à exécuter quand l'état du pin change
    Serial.println("État du pin changé!");
    bool ec1 = digitalRead(ENDCOURSE1);
    bool ec2 = digitalRead(ENDCOURSE2);
    setEndCourse(ec1, ec2);
    if (!ec1 && !ec2) {
      sendNotificationsToUdpClients("endCourse","OFF","NA");
      
    } else {
      dec_stop();
      if (ec1) {
        sendNotificationsToUdpClients("endCourse","ON","UP");
      } else {
        sendNotificationsToUdpClients("endCourse","ON","DOWN");
      }
    }
    stateEndOfCourseHasChanged = false;  // Réinitialiser le drapeau
  }

  if (abs(retTest1())>5) {
    if (retTest1()>5) {
      sendNotificationsToUdpClients("endCourse","ON","UP");
    } else {
      sendNotificationsToUdpClients("endCourse","ON","DOWN");
    }
  }

  if (abs(retTest1()==6))
    sendNotificationsToUdpClients("endCourse","OFF","NA");

  //cleanupClient();
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
#include "main.h"
#include "network.h"
#include "command.h"
// Port UDP

volatile bool stateEndOfCourseHasChanged = false;
bool isEndOfCourse = false; 

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
  pinMode(DIR_DEC_ACTIVATE, OUTPUT);
  pinMode(DIR_AD_PIN, OUTPUT);  
  pinMode(DIR_DEC_PIN, OUTPUT);
  pinMode(DIR_COU1, OUTPUT);  
  pinMode(DIR_COU2, OUTPUT);
  digitalWrite(DIR_DEC_ACTIVATE, LOW);

  attachInterrupt(digitalPinToInterrupt(ENDCOURSE1), interruptFunction, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENDCOURSE2), interruptFunction, CHANGE);
  bool ec1 = digitalRead(ENDCOURSE1);
  bool ec2 = digitalRead(ENDCOURSE2);
  setEndCourse(ec1, ec2);
}




void loop() {

  // Traiter les paquets UDP entrants
  processUdpPacket();
 //if(stateEndOfCourseHasChanged) {
    // Code à exécuter quand l'état du pin change
   // Serial.println("État du pin changé!");
    bool ec1 = digitalRead(ENDCOURSE1);
    bool ec2 = digitalRead(ENDCOURSE2);
    setEndCourse(ec1, ec2);

    if (!ec1 && !ec2 ) {
      if ( isEndOfCourse) {
        Serial.println("send off course");
      
        sendNotificationsToUdpClients("endCourse","OFF","NA");
        isEndOfCourse = false;
      }
    } else {
      if (!isEndOfCourse) {
        isEndOfCourse = true;
        dec_stop();
        if (ec1) {
          sendNotificationsToUdpClients("endCourse","ON","UP");
        } else {
          sendNotificationsToUdpClients("endCourse","ON","DOWN");
        }
     }
    }
   // stateEndOfCourseHasChanged = false;  // Réinitialiser le drapeau
 // }

 

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
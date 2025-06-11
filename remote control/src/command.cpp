#include "command.h"
#include <ArduinoJson.h>
#include "network.h"

Command commands[NUM_COMMANDS] = {
    {"mode",mode},
    {"multiplier",multiplier},
    {"direction",direction},
    {"coupole",coupole},
    {"connect",connect},
    {"decspeed",decspeed},
  };

  typedef struct  {
    bool upLimit;
    bool downLimit;
  } limitSwitchStruct;
  limitSwitchStruct decLimit = { false, false};
  
int test1=0;


int retTest1() {
    return test1;
}


si5351RDiv_t r_div = SI5351_R_DIV_32;
int freq_index = 0;
int freq_index_dec = 0;
Adafruit_SI5351 clockgen = Adafruit_SI5351();

FrequencyParams ADFrequencies[NUM_FREQUENCIES] = {
    {24, 5161, 309650, 892, 0, 1, SI5351_R_DIV_64}, // "sidereal"
    {24, 1453, 305639, 894, 0, 1, SI5351_R_DIV_64},  // "solar"
    {24, 7500, 386239, 926, 0, 1, SI5351_R_DIV_64}   // "lunar"
};

FrequencyParams DECFrequencies[NUM_FREQUENCIES_DEC] = {
    {24, 5161, 266660, 892, 0, 1, SI5351_R_DIV_32}, // "Slow"
    {24, 6293, 234048, 699, 0, 1, SI5351_R_DIV_8},  // "fast"
};


const char *frequencyName[3] = {"sidereal", "solar", "lunar"};
String ok="OK";

String connect(String parameters) {
    String mode=frequencyName[freq_index];
    String multiplier;
    switch(r_div) {
        case SI5351_R_DIV_64:
            multiplier="x1";break;
        case SI5351_R_DIV_32:
            multiplier="x2";break;
        case SI5351_R_DIV_16:
            multiplier="x4";break;
        case SI5351_R_DIV_4:
            multiplier="x16";break;
    }   
    String dec[]={"slow","fast"};


    JsonDocument doc;
    doc["type"]="connect";
    doc["dec_speed"]=dec[freq_index_dec];
    doc["multiplier"]=multiplier;
    doc["time_system"]=mode;

    String response;
    serializeJson(doc, response);
    return response;
}
/***********************************************************/
/*                   Manage si5351 command                 */
/***********************************************************/
void set_frequency(si5351PLL_t pll, uint8_t clock, FrequencyParams params, si5351RDiv_t r_div) {
#ifndef CLOCKSIMULATOR
    clockgen.setupPLL(pll,params.pll_mult,params.pll_num,params.pll_denom);
    clockgen.setupMultisynth(clock,pll, params.ms_divider, params.ms_num, params.ms_denom);
    clockgen.setupRdiv(clock, r_div);
    clockgen.enableOutputs(true);
#endif
    Serial.println("New frequency set"); Serial.println("");
}

void setADFrequencies(si5351RDiv_t rdiv){
    FrequencyParams params = ADFrequencies[freq_index];
    set_frequency(CLOCK_AD_PLL, CLOCK_AD_OUTPUT,  params, rdiv);
}

void setDECFrequencies(){
    FrequencyParams params = DECFrequencies[freq_index_dec];
    set_frequency(CLOCK_DEC_PLL, CLOCK_DEC_OUTPUT, params, params.r_div);
}


//**********************************************************
//*                    Manage mode command                  
//**********************************************************

String retCommand(int status, String message) {
    JsonDocument doc;
    doc["status"] = status;
    doc["message"] = message;
    String response;
    serializeJson(doc, response);
    return response;
}




String setLunar()
{
    freq_index=2;
    setADFrequencies(ADFrequencies[freq_index].r_div);
    return retCommand(0, ok);


}

String setSidereal()
{
    freq_index=0;
    setADFrequencies(ADFrequencies[freq_index].r_div);
    return retCommand(0, ok);

}

String setSolar()
{
    freq_index=1;
    setADFrequencies(ADFrequencies[freq_index].r_div);
    return retCommand(0, ok);

}

/***********************************************************/
/*                    Manage speed command                 */
/***********************************************************/

String x2()
{
    r_div = SI5351_R_DIV_32;
    return retCommand(0, ok);

}

String x4()
{
    r_div = SI5351_R_DIV_16;
    return retCommand(0, ok);

}

String x16()
{
    r_div = SI5351_R_DIV_4;
    return retCommand(0, ok);

}


String ad_stop() {
    digitalWrite(DIR_AD_PIN,HIGH);
    setADFrequencies(SI5351_R_DIV_64);
    return retCommand(0, ok);

}

String  ad_plus() {
    digitalWrite(DIR_AD_PIN,HIGH);
    setADFrequencies(r_div);
    return retCommand(0, ok);

}

String  ad_minus() {
    digitalWrite(DIR_AD_PIN,LOW);
    setADFrequencies(r_div);
    return retCommand(0, ok);
}

String  ad_freeze() {
    digitalWrite(DIR_AD_ACTIVATE,LOW);
    return retCommand(0, ok);
}


String  ad_unfreeze() {
    digitalWrite(DIR_AD_ACTIVATE,HIGH);
    return retCommand(0, ok);
}
    
    
String  dec_stop() {
    //set_frequency(SI5351_R_DIV_64);
    Serial.println("Stop dec");
    digitalWrite(DIR_DEC_ACTIVATE, LOW);
    return retCommand(0, ok);

}
    
String  dec_plus() {
    // set_frequency();

    if (digitalRead(ENDCOURSE1)) {
        digitalWrite(DIR_DEC_ACTIVATE, LOW);
        sendNotificationsToUdpClients("endCourse","ON","UP");
        return retCommand(1, "End course 1 reached");
    }
    digitalWrite(DIR_DEC_PIN,HIGH);
    digitalWrite(DIR_DEC_ACTIVATE, HIGH);

    setDECFrequencies();
    return retCommand(0, ok);
    
    
}
    
String dec_minus() {
    if (digitalRead(ENDCOURSE2)) {
        digitalWrite(DIR_DEC_ACTIVATE, LOW);
        sendNotificationsToUdpClients("endCourse","ON","DOWN");

        return retCommand(1, "End course 2 reached");
    }
    digitalWrite(DIR_DEC_PIN,LOW);
    digitalWrite(DIR_DEC_ACTIVATE, HIGH);

    setDECFrequencies();
    // set_frequency();
    return retCommand(0, ok);
    
}


String decspeed(String parameters) {
    if (parameters.compareTo("fast") == 0) {
        freq_index_dec=1;
        setDECFrequencies();
    } else if (parameters.compareTo("slow") == 0) {
        freq_index_dec=0;
        setDECFrequencies();
    } else {
        Serial.println("Invalid dec speed");
        return retCommand(1, "Invalid dec speed");
    }
    return retCommand(0, ok);
}

    
    
/***********************************************************/
/*               Manage Coupole                            */
/***********************************************************/

String  cou_stop() {
    digitalWrite(DIR_COU1,LOW);
    digitalWrite(DIR_COU2,LOW);
    return retCommand(0, ok);
}
    
String  cou_plus() {
    // set_frequency();
    digitalWrite(DIR_COU1,HIGH);
    digitalWrite(DIR_COU2,LOW);
    return retCommand(0, ok);
}

String  cou_minus() {
    digitalWrite(DIR_COU1,LOW);
    digitalWrite(DIR_COU2,HIGH);
    return retCommand(0, ok);

}



String multiplier(String parameters) {
    if (parameters.compareTo("x2") == 0) {
        x2();
        return retCommand(0, ok);
    } else if (parameters.compareTo("x4") == 0) {
        x4();
        return retCommand(0, ok);
    } else if (parameters.compareTo("x16") == 0) { 
        x16();
        return retCommand(0, ok);  
    } else {
        return retCommand(1, "Invalid multiplier"); 
    }
}

String direction(String parameters) {
    if (parameters.compareTo("right") == 0) {
        ad_plus();
        return retCommand(0, ok);
    } else if (parameters.compareTo("left") == 0) {
        ad_minus();
        return retCommand(0, ok);
    } else if (parameters.compareTo("up") == 0) {
        dec_plus();
        return retCommand(0, ok);
    } else if (parameters.compareTo("down") == 0) {
        dec_minus();
        return retCommand(0, ok);
    } else if (parameters.compareTo("freeze") == 0) {
        ad_freeze();
        return retCommand(0, ok);
    } else if (parameters.compareTo("unfreeze") == 0) {
        ad_unfreeze();
        return retCommand(0, ok);
    } else if (parameters.compareTo("stop") == 0) {
        ad_stop();
        dec_stop();
        return retCommand(0, ok);
    } else {
        return retCommand(1, "Invalid direction"); 
    }
}

String mode(String parameters) {
    if (parameters.compareTo("solar") == 0) {
        setSolar();
        return retCommand(0, ok);
    } else if (parameters.compareTo("sidereal") == 0) {
        setSidereal();
        return retCommand(0, ok);
    } else if (parameters.compareTo("lunar") == 0) {
        setLunar();
        return retCommand(0, ok);
    } else {
        return retCommand(1, "Invalid mode"); 
    }
}

String coupole(String parameters) {
    if (parameters.compareTo("right") == 0) {
        cou_plus();
        return retCommand(0, ok);
    } else if (parameters.compareTo("left") == 0) {
        cou_minus();
        return retCommand(0, ok);
    } else if (parameters.compareTo("stop") == 0) {
        cou_stop();
        return retCommand(0, ok);
    } else {
        return retCommand(1, "Invalid coupole"); 
    }
}


String  processCommand(String command, String parameters, String clientId) {
    Serial.println("Commande reçue: " + command + " de " + clientId);
    
    // Traiter la commande (exemple)
    String result;
    for (int i = 0; i < NUM_COMMANDS; i++) {
      if (command.compareTo(commands[i].command) == 0) {
          Serial.println(commands[i].command);

    
          return commands[i].function(parameters); 
          //sendJsonResponse(clientIp, clientPort, responseDoc);
        }
    }
    return retCommand(1, "Invalid command");
}
void setEndCourse(bool up, bool down) {
    decLimit.upLimit=up;
    decLimit.downLimit=down;


}

void initClock() {
#ifndef CLOCKSIMULATOR
  /* Initialise the sensor */
  if (clockgen.begin() != ERROR_NONE)
  {
    /* There was a problem detecting the IC ... check your connections */
    Serial.print("Ooops, no Si5351 detected ... Check your wiring or I2C ADDR!");
   // while(1);
  } else {
    Serial.println("Si5351 detected!");
    setSidereal();
  }
#endif
}
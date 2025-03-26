#include "command.h"
#include <ArduinoJson.h>

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

si5351RDiv_t r_div = SI5351_R_DIV_64;
int freq_index = 0;
Adafruit_SI5351 clockgen = Adafruit_SI5351();

FrequencyParams ADFrequencies[NUM_FREQUENCIES] = {
    {24, 23607, 997652, 697, 0, 1, SI5351_R_DIV_64}, // "sidereal"
    {24, 6293, 234048, 699, 0, 1, SI5351_R_DIV_64},  // "solar"
    {24, 4303, 526562, 723, 0, 1, SI5351_R_DIV_64}   // "lunar"
};

const char *frequencyName[3] = {"solar", "sidereal", "lunar"};

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

void setDECFrequencies(si5351RDiv_t rdiv){
    FrequencyParams params = ADFrequencies[freq_index];
    set_frequency(CLOCK_DEC_PLL, CLOCK_DEC_OUTPUT, params, rdiv);
}


//**********************************************************
//*                    Manage mode command                  
//**********************************************************
char* setLunar()
{
    freq_index=2;
    setADFrequencies(ADFrequencies[freq_index].r_div);
    return "OK";

}

char* setSidereal()
{
    freq_index=1;
    setADFrequencies(ADFrequencies[freq_index].r_div);
    return "OK";

}

char* setSolar()
{
    freq_index=0;
    setADFrequencies(ADFrequencies[freq_index].r_div);
    return "OK";

}

/***********************************************************/
/*                    Manage speed command                 */
/***********************************************************/
char* x1()
{
    r_div = SI5351_R_DIV_64;
    return "OK";
}

char* x2()
{
    r_div = SI5351_R_DIV_32;
    return "OK";

}

char* x4()
{
    r_div = SI5351_R_DIV_16;
    return "OK";

}

char* x16()
{
    r_div = SI5351_R_DIV_4;
    return "OK";

}


char* ad_stop() {
    digitalWrite(DIR_AD_PIN,HIGH);
    setADFrequencies(SI5351_R_DIV_64);
    return "OK";

}

char*  ad_plus() {
    digitalWrite(DIR_AD_PIN,HIGH);
    setADFrequencies(r_div);
    return "OK";

}

char*  ad_minus() {
    digitalWrite(DIR_AD_PIN,LOW);
    setADFrequencies(r_div);
    return "OK";

}
    
    
char*  dec_stop() {
    //set_frequency(SI5351_R_DIV_64);
    return "OK";

}
    
char*  dec_plus() {
    // set_frequency();
    digitalWrite(DIR_DEC_PIN,HIGH);
    setDECFrequencies(r_div);
    return "OK";
    
    
}
    
char* dec_minus() {
    digitalWrite(DIR_DEC_PIN,LOW);
    setDECFrequencies(r_div);
    // set_frequency();
    return "OK";
    
}
    
    
/***********************************************************/
/*               Manage Coupole                            */
/***********************************************************/

char*  cou_stop() {

    digitalWrite(DIR_COU1,LOW);
    digitalWrite(DIR_COU2,LOW);
    return "OK";

}
    
char*  cou_plus() {
    // set_frequency();
    digitalWrite(DIR_COU1,HIGH);
    digitalWrite(DIR_COU2,LOW);
    return "OK";

}

char*  cou_minus() {
    digitalWrite(DIR_COU1,LOW);
    digitalWrite(DIR_COU2,HIGH);
    return "OK";

}

String  processCommand(String command, String parameters, String clientId) {
    Serial.println("Commande reçue: " + command + " de " + clientId);
    
    // Traiter la commande (exemple)
    String result;
    for (int i = 0; i < NUM_COMMANDS; i++) {
      if (command.compareTo(commands[i].command) == 0) {
          Serial.println(commands[i].command);

    
          return commands[i].function(); 
          //sendJsonResponse(clientIp, clientPort, responseDoc);
        }
    }
    return "Unknown command";
}
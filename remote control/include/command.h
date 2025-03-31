#ifndef JONCKHEERE_COMMAND_H
#define JONCKHEERE_COMMAND_H
#include "Adafruit_SI5351.h"


#define NUM_FREQUENCIES 3
// Output for AD and DEV
#define CLOCK_AD_OUTPUT 2
#define CLOCK_DEC_OUTPUT 0

// PLL managing AD and DEC
#define CLOCK_AD_PLL SI5351_PLL_A
#define CLOCK_DEC_PLL SI5351_PLL_B

#define NUM_COMMANDS 16
// PIN For orientation 
#define DIR_AD_PIN 15
#define DIR_DEC_PIN 16
#define DIR_COU1  25
#define DIR_COU2 26



//SI5351 Configuration parameters
typedef struct FrequencyParams{
    uint8_t pll_mult;    // a
    uint32_t pll_num;    // b
    uint32_t pll_denom;  // c
    uint32_t ms_divider; // d
    uint32_t ms_num;     // e
    uint32_t ms_denom;   // f
    si5351RDiv_t r_div;       // R
} FrequencyParams;







typedef String (*CommandFunction)(String);  // Définir un type pour le pointeur de fonction

typedef struct  {
  const char* command;
  CommandFunction function;
} Command;


// List of available commands
String mode(String parameters);
String multiplier(String parameters); 
String direction(String parameters);
String coupole(String parameters);
String connect(String parameters);


String processCommand(String command, String parameters, String clientId);

void initClock();



#endif
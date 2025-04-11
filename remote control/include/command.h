#ifndef JONCKHEERE_COMMAND_H
#define JONCKHEERE_COMMAND_H
#include "Adafruit_SI5351.h"


#define NUM_FREQUENCIES 3
#define NUM_FREQUENCIES_DEC 2
#include "pin.h"

// PLL managing AD and DEC
#define CLOCK_AD_PLL SI5351_PLL_A
#define CLOCK_DEC_PLL SI5351_PLL_B

#define NUM_COMMANDS 16

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

int retTest1();

// List of available commands
String mode(String parameters);
String multiplier(String parameters); 
String direction(String parameters);
String coupole(String parameters);
String connect(String parameters);
String decspeed(String parameters);
String dec_stop();

String processCommand(String command, String parameters, String clientId);
void setEndCourse(bool left, bool right);

void initClock();

#endif
#include "Settings.h"
#include "Commands.h"
#include "Core.h"

#include <sha1.h>
#include <CommandLine.h>

// Various ADC prescaler
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

/**
 * Setup the RNG. If PIN5 is toggled high, setup mode will be activated.
 */
void setup()
{
  // Load settings from EEPROM
  loadSettings();
  
  // Add command line commands
  addCommands();
  
  // Start serial
  Serial.begin(115200);

  // Pin setup
  pinMode(4, OUTPUT);
  pinMode(5, INPUT);
  analogReference(EXTERNAL);
  
  // Set up ADC speed
  ADCSRA &= ~PS_128;
  ADCSRA |= PS_32;
  
  // Enter setup mode
  setupMode = digitalRead(5);
  
  if (setupMode) {
    Serial.println(F("Setup mode"));

    while (setupMode) {
      commandLine.update();
    }
  }
  
  // Setup hashing
  Sha1.init();
}


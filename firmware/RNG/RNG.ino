#include "Settings.h"
#include "Commands.h"
#include "Core.h"

#include <sha1.h>
#include <CommandLine.h>

uint8_t mode;

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
  
  // Enter setup mode
  setupMode = digitalRead(5);
  
  if (setupMode) {
    Serial.println("Setup mode");

    while (setupMode) {
      commandLine.update();
    }
  }
  
  // Setup hashing
  Sha1.init();
}


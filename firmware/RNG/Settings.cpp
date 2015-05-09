#include "Settings.h"

/**
 * EEPROM configuration
 */
configuration_t configuration;

/**
 *
 */
void saveSettings()
{
  configuration.magic = EEPROM_MAGIC;
  eeprom_write_block((const void *) &configuration, (void *) 0, sizeof(configuration_t)); 
}

/**
 *
 */
void loadSettings()
{
  eeprom_read_block((void *) &configuration, (void *) 0, sizeof(configuration_t));

  if (configuration.magic != EEPROM_MAGIC) {  
    defaultSettings();
    
    // Make sure settings are persisted for next time.
    saveSettings();
  }
}

void defaultSettings() 
{
  // Load default values if magic token is wrong

  configuration.toggleSamples = 0;
  configuration.toggleHold = 1;
  configuration.toggleSkip = 0;
  
  configuration.roundDelay = 100;
  
  configuration.sampleMask = 0b0001111;
  configuration.sampleShift = 0;
  configuration.sampleTake = 5;
  
  configuration.mode = 2;
  configuration.extractor = 3;
  configuration.generator = 2;
  
  configuration.output = 2;
  configuration.outputMode = 1;
  configuration.outputBucket = 511;
}

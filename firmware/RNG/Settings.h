#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <stdint.h>
#include <avr/eeprom.h>

#include <Arduino.h>

/**
 * EEPROM layout
 */
struct configuration_t {
  uint32_t magic;
  
  uint32_t toggleSamples; // Maximum number of samples before toggling NE555
  uint32_t toggleHold; // Toggle hold delay in milliseconds
  uint32_t toggleSkip; // Toggle skip delay in milliseconds
  
  uint32_t roundDelay; // Delay after each sample round in milliseconds
  
  uint16_t sampleMask; // Mask to apply to sample
  uint8_t sampleShift; // Number of bits to shift sample to the right
  uint8_t sampleTake; // Number of bits to consider
  
  uint8_t mode; // 1: Raw sample bits, 2: Ones-majority
  uint8_t extractor; // 1: None, 2: Von Neumann, 3: Improved Von Neumann
  uint8_t generator; // 1: None, 2: SHA1
  
  uint8_t output; // 1: Bits, 2: Bytes
  uint8_t outputMode; // 1: Raw, 2: Decimal (no newline), 3: Decimal (newline)
  uint16_t outputBucket; // 0-511 bytes to buffer (will be allocated before running
};

/**
 * EEPROM magic value
 */
const uint32_t EEPROM_MAGIC = 0xAA55BB66;

/**
 * EEPROM configuration
 */
extern configuration_t configuration;

extern void saveSettings();
extern void loadSettings();
extern void defaultSettings();

#endif

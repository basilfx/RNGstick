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
  
  uint8_t sampleReset; // 0: No reset, 1: reset
  uint16_t sampleBucket; // 1-128 samples to buffer before processing
  uint16_t sampleMultiple; // Maximum number of samples buckets to take
  
  uint32_t delayHold; // Delay between toggle reset low and toggle reset high, in microseconds
  uint32_t delaySkip; // Delay between toggling and sampling, in microseconds
  uint32_t delayRound; // Delay after each sample round, in microseconds
  
  uint16_t sampleMask; // Mask to apply to sample
  uint8_t sampleShift; // Number of bits to shift sample to the right
  uint8_t sampleTake; // Number of bits to consider
  
  uint8_t mode; // 1: Raw sample bits, 2: Ones-majority, 
  uint8_t extractor; // 1: None, 2: Von Neumann, 3: Improved Von Neumann
  uint8_t generator; // 1: None, 2: SHA1
  
  uint8_t output; // 1: Bits, 2: Bytes
  uint8_t outputMode; // 1: Raw, 2: Decimal (no newline), 3: Decimal (newline)
  uint16_t outputBucket; // 1-256 bytes to buffer
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

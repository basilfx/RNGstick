#include "Core.h"

/**
 * Extraction buffer
 */
volatile uint16_t extractBuffer;
volatile uint8_t extractBufferCount;

/**
 * Generate buffer
 */
volatile uint16_t generateBuffer;
volatile uint8_t generateBufferCount;

/**
 * Output buffer
 */
volatile uint16_t outputBuffer;
volatile uint8_t outputBufferCount;

/**
 * Output bucket
 */
volatile uint8_t outputBucket[512];
volatile uint16_t outputBucketIndex;

void output(uint8_t value) 
{ 
  if (configuration.outputMode == 1) {
    Serial.write(value);
  } else if (configuration.outputMode == 2) {
    Serial.print(value, DEC);
  } else if (configuration.outputMode == 3) {
    Serial.println(value, DEC); 
  }
}

void bucket(uint8_t value)
{
  outputBucket[outputBucketIndex] = value;
  outputBucketIndex++;
  
  if (outputBucketIndex >= configuration.outputBucket) {
    while (true) {
      outputBucketIndex--;
      output(outputBucket[outputBucketIndex]);
      
      if (outputBucketIndex == 0) {
        break;
      }
    }
  }
}

void collect(uint8_t value, uint8_t count)
{ 
  outputBuffer = (outputBuffer << count) | value;
  outputBufferCount = outputBufferCount + count;
  
  while (outputBufferCount > 7) {
    if (configuration.output == 1) {
      for (uint8_t i = 0; i < 8; i++) {
        bucket((uint8_t) (outputBuffer & 0x01));
        outputBuffer = outputBuffer >> 1;
      }
    } else if (configuration.output == 2) {
      bucket((uint8_t) (outputBuffer & 0xFF));
      outputBuffer = outputBuffer >> 8; 
    }
    
    outputBufferCount = outputBufferCount - 8; 
  }
}

void generate(uint8_t value, uint8_t count)
{
  if (configuration.generator == 1) {
    collect(value, count);
  } else if (configuration.generator == 2) {
    generateBuffer = (generateBuffer << count) | value;
    generateBufferCount = generateBufferCount + count;
    
    while (generateBufferCount > 7) {
      Sha1.print((uint8_t) (generateBuffer & 0xFF));
      uint8_t* result = Sha1.result();
      
      for (uint32_t i = 0; i < 20; i++) {
         collect(result[i], 8);
      }
      
      generateBuffer = generateBuffer >> 8;
      generateBufferCount = generateBufferCount - 8;
    }
  }
}

void extract(uint8_t value, uint8_t count)
{ 
  if (configuration.extractor == 1) {
    generate(value, count);
  } else if (configuration.extractor == 2 || configuration.extractor == 3) {
    extractBuffer = (extractBuffer << count) | value;
    extractBufferCount = extractBufferCount + count;
  
    while (extractBufferCount > 3) {
      if (configuration.extractor == 2) {
        generate(VON_NEUMANN[extractBuffer & 0x0F][0], VON_NEUMANN[extractBuffer & 0x0F][1]);
      } else if (configuration.extractor == 3) {
        generate(IMPROVED_VON_NEUMANN[extractBuffer & 0x0F][0], IMPROVED_VON_NEUMANN[extractBuffer & 0x0F][1]);
      }
     
      extractBuffer = extractBuffer >> 4;
      extractBufferCount = extractBufferCount - 4; 
    }
  }
}

void sample()
{
  uint16_t sample = (analogRead(A0) & configuration.sampleMask) >> configuration.sampleShift;
  
  if (configuration.mode == 1) {
    extract(sample, configuration.sampleTake);
  } else if (configuration.mode == 2) {
    uint8_t count = 0;
    
    for (uint8_t i = 0; i < configuration.sampleTake; i++) {
      if (sample & (1 << i)) {
        count++;
      }
    }

    extract(count > (configuration.sampleTake / 2) ? 1 : 0, 1);
  }
}

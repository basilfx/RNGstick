#include "Core.h"

/**
 * Sample bucket
 */
volatile uint16_t sampleBucket[256];

/**
 * Process buffer
 */
volatile uint16_t processBuffer[2];
volatile uint8_t processBufferCount;

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
volatile uint8_t outputBucket[256];
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

void process(uint16_t sample)
{
  sample = (sample & configuration.sampleMask) >> configuration.sampleShift;
  
  if (configuration.mode == 1) {
    extract(sample, configuration.sampleTake);
  } else if (configuration.mode == 3) {
    processBuffer[processBufferCount] = sample;
    processBufferCount++;

    if (processBufferCount == 2) {
      extract(processBuffer[0] ^ processBuffer[1], configuration.sampleTake);
    }
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

void loop()
{
  // Reset the NE555
  if (configuration.sampleReset == 1) {
    digitalWrite(4, LOW);
    delayMicroseconds(configuration.delayHold);
    digitalWrite(4, HIGH);
    delayMicroseconds(configuration.delaySkip);
  }
  
  // Take samples
  for (uint16_t i = 0; i < configuration.sampleMultiple; i++) {
    // Gather samples
    for (uint16_t j = 0; j < configuration.sampleBucket; j++) {
      sampleBucket[j] = analogRead(A0);
    }
    
    // Process samples
    for (uint16_t j = 0; j < configuration.sampleBucket; j++) {
      process(sampleBucket[j]);
    }
  }
  
  // Round delay
  delayMicroseconds(configuration.delayRound); 
}

#include "Commands.h"
#include "Settings.h"

CommandLine commandLine(Serial, "> ");

bool setupMode;

/**
 * Universal method for reading 1, 2 or 4 bytes.
 * 
 * @param property Pointer to property to read from or write to.
 * @param length Length in bytes of the property to read.
 * @param min Minimal value of property.
 * @param max Maximal value of property.
 */
void parseCommand(void* property, size_t length, uint32_t min, uint32_t max)
{
  char* parameter = strtok(NULL, " ");
  
  if (parameter == NULL) {
    if (length == 1) {
      Serial.println(*(uint8_t *) property); 
    } else if (length == 2) {
      Serial.println(*(uint16_t *) property); 
    } else if (length == 4) {
      Serial.println(*(uint32_t *) property); 
    }
  } else {
    uint32_t value = atol(parameter);
    
    if (value < min || value > max) {
      Serial.print(F("Value not in range: "));
      Serial.print(min);
      Serial.print(F(" - "));
      Serial.print(max);
      Serial.println(F(""));
    } else {
      if (length == 1) {
        *(uint8_t*) property = (uint8_t) value;
      } else if (length == 2) {
        *(uint16_t*) property = (uint16_t) value;
      } else if (length == 4) {
        *(uint32_t*) property = (uint32_t) value;
      }
    }
  }
}

void handleSampleReset(char* tokens)
{
  parseCommand(&configuration.sampleReset, 1, 0, 1); 
}

void handleSampleBucket(char* tokens)
{
  parseCommand(&configuration.sampleBucket, 2, 1, 256);
}

void handleSampleMultiple(char* tokens)
{
  parseCommand(&configuration.sampleMultiple, 2, 1, 65535U);
}

void handleDelayHold(char* tokens)
{
  parseCommand(&configuration.delayHold, 4, 0, 4294967295U);
}

void handleDelaySkip(char* tokens)
{
  parseCommand(&configuration.delaySkip, 4, 0, 4294967295U);
}

void handleDelayRound(char* tokens)
{
  parseCommand(&configuration.delayRound, 4, 0, 4294967295U);
}

void handleSampleMask(char* tokens)
{
  parseCommand(&configuration.sampleMask, 2, 0, 0xFFFF);
}

void handleSampleShift(char* tokens)
{
  parseCommand(&configuration.sampleShift, 1, 0, 10);
}

void handleSampleTake(char* tokens)
{
  parseCommand(&configuration.sampleTake, 1, 0, 10);
}

void handleMode(char* tokens)
{
  parseCommand(&configuration.mode, 1, 1, 2);
}

void handleExtractor(char* tokens)
{
  parseCommand(&configuration.extractor, 1, 1, 3);
}

void handleGenerator(char* tokens)
{
  parseCommand(&configuration.generator, 1, 1, 2);
}

void handleOutput(char* tokens)
{
  parseCommand(&configuration.output, 1, 1, 2);
}

void handleOutputMode(char* tokens)
{
  parseCommand(&configuration.outputMode, 1, 1, 3);
}

void handleOutputBucket(char* tokens)
{
  parseCommand(&configuration.outputBucket, 2, 1, 256);
}

void handleSave(char* tokens)
{
  saveSettings();
  Serial.println(F("Settings saved."));
}

void handleLoad(char* tokens)
{
  loadSettings();
  Serial.println(F("Settings loaded."));
}

void handleDefaults(char* tokens)
{
  defaultSettings();
  Serial.println(F("Default settings (not saved)."));
}

void handleHelp(char* tokens)
{
  Serial.println(F("Available commands: sampleReset, sampleBucket, sampleMultiple, delayHold, delaySkip, delayRound, sampleMask, sampleShift, sampleTake, mode, extractor, generator, output, outputMode, outputBucket, save, load, defaults, help, run"));
}

void handleRun(char* tokens)
{
  setupMode = false;
}

void handlePost(char* tokens, bool success)
{
  if (!success) {
    Serial.println(F("Unknown command. Type 'help' for help."));
  }
}

void addCommands()
{
  commandLine.attachPost(handlePost);
  
  commandLine.add("sampleReset", &handleSampleReset);
  commandLine.add("sampleBucket", &handleSampleBucket);
  commandLine.add("sampleMultiple", &handleSampleMultiple);

  commandLine.add("delayHold", &handleDelayHold);
  commandLine.add("delaySkip", &handleDelaySkip);  
  commandLine.add("delayRound", &handleDelayRound);
  
  commandLine.add("sampleMask", &handleSampleMask);
  commandLine.add("sampleShift", &handleSampleShift);
  commandLine.add("sampleTake", &handleSampleTake);
  
  commandLine.add("mode", &handleMode);
  commandLine.add("extractor", &handleExtractor);
  commandLine.add("generator", &handleGenerator);
  
  commandLine.add("output", &handleOutput);
  commandLine.add("outputMode", &handleOutputMode);
  commandLine.add("outputBucket", &handleOutputBucket);
  
  commandLine.add("save", &handleSave);
  commandLine.add("load", &handleLoad);
  commandLine.add("defaults", &handleDefaults);

  commandLine.add("help", &handleHelp);  
  commandLine.add("run", &handleRun);
}

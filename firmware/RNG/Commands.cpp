#include "Commands.h"
#include "Settings.h"

CommandLine commandLine(Serial, "> ");

bool setupMode;

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
    int value = atoi(parameter);
    
    if (value < min || value > max) {
      Serial.print("Value not in range: ");
      Serial.print(min);
      Serial.print(" - ");
      Serial.print(max);
      Serial.println("");
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

void handleToggleSamples(char* tokens)
{
  parseCommand(&configuration.toggleSamples, 4, 0, 4294967295U);
}

void handleToggleHold(char* tokens)
{
  parseCommand(&configuration.toggleHold, 4, 0, 4294967295U);
}

void handleToggleSkip(char* tokens)
{
  parseCommand(&configuration.toggleSkip, 4, 0, 4294967295U);
}

void handleRoundDelay(char* tokens)
{
  parseCommand(&configuration.roundDelay, 4, 0, 4294967295U);
}

void handleSampleMask(char* tokens)
{
  parseCommand(&configuration.sampleMask, 2, 0, 0xFF);
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
  parseCommand(&configuration.outputMode, 1, 1, 2);
}

void handleOutputBucket(char* tokens)
{
  parseCommand(&configuration.outputBucket, 2, 0, 511);
}

void handleSave(char* tokens)
{
  saveSettings();
  Serial.println("Settings saved.");
}

void handleLoad(char* tokens)
{
  loadSettings();
  Serial.println("Settings loaded.");
}

void handleDefaults(char* tokens)
{
  defaultSettings();
  Serial.println("Default settings (not saved).");
}

void handleHelp(char* tokens)
{
  Serial.println("Available commands: toggleSamples, toggleHold, toggleSkip, roundDelay, sampleMask, sampleShift, sampleTake, mode, extractor, generator, output, outputMode, outputBucket, save, load, defaults, help, run"); 
}

void handleRun(char* tokens)
{
  setupMode = false;
  Serial.println("Run mode");
}

void addCommands()
{
  commandLine.add("toggleSamples", &handleToggleSamples);
  commandLine.add("toggleHold", &handleToggleHold);
  commandLine.add("toggleSkip", &handleToggleSkip);
  
  commandLine.add("roundDelay", &handleRoundDelay);
  
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

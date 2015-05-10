#ifndef __COMMANDS_H__
#define __COMMANDS_H__

// Number of commands to support, should be defined before including CommandLine.h
#define COMMANDLINE_COUNT 20

#include <stdint.h>

#include <Arduino.h>
#include <CommandLine.h>

/**
 * EEPROM configuration
 */
extern CommandLine commandLine;

/**
 * Indicate if we are in setup mode or not.
 */
extern bool setupMode;

/**
 * Add commands to CommandLine instance.
 */
extern void addCommands();

#endif

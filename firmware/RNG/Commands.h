#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <stdint.h>

#include <Arduino.h>
#include <CommandLine.h>

/**
 * EEPROM configuration
 */
extern CommandLine commandLine;

/**
 *
 */
extern bool setupMode;

extern void addCommands();

#endif

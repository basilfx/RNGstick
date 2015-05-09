#ifndef __CORE_H__
#define __CORE_H__

#include <stdint.h>

#include <Arduino.h>
#include <sha1.h>

#include "Settings.h"

/**
 * Lookup table for Von Neumann Extractor. First element is bits to add, second is the
 * number of bits to add.
 */
const uint8_t VON_NEUMANN[][2] = {
  {0b0, 0}, {0b0, 1}, {0b1, 1}, {0b0, 0}, {0b0, 1}, {0b00, 2}, {0b01, 2}, {0b0, 1}, 
  {0b1, 1}, {0b10, 2}, {0b11, 2}, {0b1, 1}, {0b0, 0}, {0b0, 1}, {0b1, 1}, {0b0, 0}};

/**
 * Lookup table for Improved Von Neumann Extractor. First element is bits to add, second is
 * the number of bits to add.
 */
const uint8_t IMPROVED_VON_NEUMANN[][2] = {
  {0b0, 0}, {0b00, 2}, {0b10, 2}, {0b0, 1}, {0b01, 2}, {0b00, 2}, {0b01, 2}, {0b01, 2}, 
  {0b11, 2}, {0b10, 2}, {0b11, 2}, {0b11, 2}, {0b1, 1}, {0b00, 2}, {0b10, 2}, {0b0, 0}};

/**
 * Start sampling and gather random bits.
 */
extern void sample();

#endif

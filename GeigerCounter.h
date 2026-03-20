/**
 * @file GeigerCounter.h
 * @author Tyler Nielsen
 * @brief GeigerCounter class definition for interfacing with the "pulse" out of
 * a Mighty Ohm Geiger Counter.
 * @version 0.1
 * @date 2024-04-05
 *
 */

#ifndef GEIGER_COUNTER_H
#define GEIGER_COUNTER_H

#include <stdint.h>

#include "Arduino.h"

#define SCALE_FACTOR 57

class GeigerCounter {
 private:
  unsigned long minimumSamplePeriod_ms;
  float lastCPSMeasurement;
  unsigned long samplePeriodStart;

 public:
  GeigerCounter() {}
  GeigerCounter(int8_t geigerPin);
  GeigerCounter(int8_t geigerPin, unsigned long minimumSamplePeriod_ms);

  void begin(int8_t geigerPin);
  void begin(int8_t geigerPin, unsigned long minimumSamplePeriod_ms);

  // blocking functions
  float getCPSBlocking(unsigned long samplePeriod_ms);
  float getDoseBlocking(unsigned long samplePeriod_ms);

  // running functions
  float getCPSRunning();
  float getInstCPSRunning();
  float getDoseRunning();
  float getInstDoseRunning();
  void refreshSample();

  // utility functions
  float CPStoDose(float cps);
};

#endif
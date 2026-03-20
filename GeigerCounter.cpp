/**
 * @file GeigerCounter.cpp
 * @author Tyler Nielsen
 * @brief GeigerCounter class function implementation.
 * @version 0.1
 * @date 2024-04-05
 *
 */

#include "GeigerCounter.h"

static volatile uint32_t STATIC_GEIGER_COUNT = 0;

static void counter_isr() { STATIC_GEIGER_COUNT++; }

/**
 * @brief Construct a Geiger Counter object with default minimum sample period
 * of 5000 ms.
 *
 * @param geigerPin Pin connected to the Geiger Counter's data or "pulse"
 * output. Make sure that it is interrupt-enabled.
 */
GeigerCounter::GeigerCounter(int8_t geigerPin)
    : GeigerCounter(geigerPin, 5000) {}

/**
 * @brief Construct a Geiger Counter object with specified minimum sample
 * period.
 *
 * @param geigerPin Pin connected to the Geiger Counter's data or "pulse" output
 * (make sure that it is interrupt-enabled)
 * @param minimumSamplePeriod_ms Minimum sample period in ms for "running"
 * functions
 */
GeigerCounter::GeigerCounter(int8_t geigerPin,
                             unsigned long minimumSamplePeriod_ms) {
  pinMode(geigerPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(geigerPin), counter_isr, FALLING);
  this->samplePeriodStart = millis();
  this->lastCPSMeasurement = 0;
  this->minimumSamplePeriod_ms = minimumSamplePeriod_ms;
}

/**
 * @brief If the empty (no param) constructor is used, this can be used to
 * initialize. Or reinitialize after any initial constructor
 *
 * @param geigerPin Pin connected to the Geiger Counter's data or "pulse" output
 * (make sure that it is interrupt-enabled)
 */
void GeigerCounter::begin(int8_t geigerPin) { this->begin(geigerPin, 1000); }

/**
 * @brief If the empty (no params) constructor is used, this can be used to
 * initialize. Or reinitialize after any initial constructor
 *
 * @param geigerPin Pin connected to the Geiger Counter's data or "pulse" output
 * (make sure that it is interrupt-enabled)
 * @param minimumSamplePeriod_ms Minimum sample period in ms for "running"
 * functions
 */
void GeigerCounter::begin(int8_t geigerPin,
                          unsigned long minimumSamplePeriod_ms) {
  pinMode(geigerPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(geigerPin), counter_isr, FALLING);
  this->refreshSample();
  this->lastCPSMeasurement = 0;
  this->minimumSamplePeriod_ms = minimumSamplePeriod_ms;
}

/**
 * @brief Calculates CPS for the specified sample period, starting at function
 * call.
 *
 * @param samplePeriod_ms Period to sample over
 * @return float - CPS
 */
float GeigerCounter::getCPSBlocking(unsigned long samplePeriod_ms) {
  this->refreshSample();
  delay(samplePeriod_ms);

  return this->getInstCPSRunning();
}

/**
 * @brief Calculates dose in uSv/hr for the specified sample period, starting at
 * function call.
 *
 * @param samplePeriod_ms Period to sample over
 * @return float - Dose (uSv/hr)
 */
float GeigerCounter::getDoseBlocking(unsigned long samplePeriod_ms) {
  float CPS = this->getCPSBlocking(samplePeriod_ms);

  return this->CPStoDose(CPS);
}

/**
 * @brief Calculates CPS for the period since last CPS calculation. If the
 * period single last CPS calculation is less than the minimum sample period
 * (minimumSamplePeriod_ms, default 1000 ms) then the last calculated CPS
 * measurement is returned.
 *
 * @return float - CPS
 */
float GeigerCounter::getCPSRunning() {
  unsigned long samplePeriod = millis() - this->samplePeriodStart;
  float CPS = -1;

  if (samplePeriod < this->minimumSamplePeriod_ms) {
    return this->lastCPSMeasurement;
  } else {
    noInterrupts();
    CPS = STATIC_GEIGER_COUNT / (samplePeriod / 1000.0);
    STATIC_GEIGER_COUNT = 0;
    interrupts();

    this->samplePeriodStart = millis();

    this->lastCPSMeasurement = CPS;
    return CPS;
  }
}

/**
 * @brief Calculates CPS for the period since last CPS calculation. Does NOT
 * check if the period since last CPS calculation is more than the minimum
 * sample period (minimumSamplePeriod_ms). Due to that, calling this function
 * too frequently will cause inaccurate readings.
 *
 * @return float - CPS
 */
float GeigerCounter::getInstCPSRunning() {
  unsigned long samplePeriod = millis() - this->samplePeriodStart;

  float CPS = 0.0;  // Default to 0
  if (samplePeriod == 0) {
    return -1;  //  return -1 to indicate invalid reading
  }

  noInterrupts();
  CPS = STATIC_GEIGER_COUNT / (samplePeriod / 1000.0);
  STATIC_GEIGER_COUNT = 0;
  interrupts();

  this->samplePeriodStart = millis();

  this->lastCPSMeasurement = CPS;
  return CPS;
}

/**
 * @brief Calculates dose based on CPS for the period since last CPS
 * calculation. Uses getCPSRunning() so the minimum sample period is enforced.
 *
 * @return float - Dose (uSv/hr)
 */
float GeigerCounter::getDoseRunning() {
  float CPS = this->getCPSRunning();

  return this->CPStoDose(CPS);
}

/**
 * @brief Calculates dose based on CPS for the period since last CPS
 * calculation. Uses getInstCPSRunning() so the minimum sample period is NOT
 * enforced. Calling getInstDoseRunning immediately after getInstCPSRunning will
 * result in bad readings (likely 0) as a getInstCPSRunning call will refresh
 * the count to 0
 *
 * @return float - Dose (uSv/hr)
 */
float GeigerCounter::getInstDoseRunning() {
  float CPS = this->getInstCPSRunning();

  return this->CPStoDose(CPS);
}

/**
 * @brief Refreshes sample period, resetting STATIC_GEIGER_COUNT and
 * samplePeriodStart.
 *
 */
void GeigerCounter::refreshSample() {
  noInterrupts();

  STATIC_GEIGER_COUNT = 0;

  interrupts();

  this->samplePeriodStart = millis();
}

/**
 * @brief Converts a given CPS value to Dose (uS/hr) using equation taken from
 * the firmware for the ATtiny on the Mighty Ohm Geiger Counter board
 *
 * @param cps Counts Per Second
 * @return float Dose (uS/hr)
 */
float GeigerCounter::CPStoDose(float cps) {
  return ((cps * 60.0) * SCALE_FACTOR) / 10000.0;
}
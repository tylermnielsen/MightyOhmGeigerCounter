/**
 * @file GeigerMinimumSamplePeriod.ino
 * @author Tyler Nielsen
 * @brief Example code for basic Might Ohm Geiger Counter Arduino library using custom minimum sample period
 * @version 0.1
 * @date 2024-04-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/**
 * This example uses the "running" functions but sets the minimum sample period. Refer to the 
 * GeigerRunningTest.ino example for information on the running functions. 
 */

#include "GeigerCounter.h"

// can be any interrupt-enabled pin (these vary based on the microcontroller you are using)
#define GEIGER_PIN A2
// the desired minimum sample period, in this case for a full minute
// longer is typically more accurate but will be less able to detect spikes 
// as it averages the CPS over the sample period
#define MINIMUM_SAMPLE_PERIOD_MS 60000 

GeigerCounter gc;

void setup(){
    Serial.begin(9600);
    while(!Serial);

    gc.begin(GEIGER_PIN, MINIMUM_SAMPLE_PERIOD_MS);

    Serial.println("Beginning Geiger Counter Test...");
    delay(1000);
}

void loop(){
    Serial.print("CPS: "); Serial.println(gc.getCPSRunning());
    Serial.print("Dose (uSv/hr): "); Serial.println(gc.getDoseRunning());

    delay(1000);
}
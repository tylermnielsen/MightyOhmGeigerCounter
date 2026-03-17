/**
 * @file GeigerTest.ino
 * @author Tyler Nielsen
 * @brief Example code for Might Ohm Geiger Counter Arduino library
 * @version 0.1
 * @date 2024-04-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/**
 * This example uses the "running" functions. These functions will find the measurements with a CPS 
 * based on the counts in the time period since the last measurement or the begin() function. This means
 * that if you have long waiting periods between each measurement the measurements returned will be based
 * on the average CPS during that time period. If you want to focus the measurement to a specific time frame,
 * consider using refreshSample() to reset the sample period and count or the "blocking" functions (example in GeigerBlockingTest.ino).
 * 
 * These "running" functions also have a built-in minimum sample period (5000 ms), meaning that they will not update the 
 * values they are returning until the period is long enough, helping with reading accuracy. This can cause the first few 
 * measurements after a begin() or refreshSample() to be inaccurate, if the time since the period start has not been long 
 * enough to replace the old values. If you want to set this minimum sample period manually, you can pass it as a second 
 * argument to begin() (example in GeigerMinimumSamplePeriod.ino). 
 */

#include "GeigerCounter.h"

// can be any interrupt-enabled pin (these vary based on the microcontroller you are using)
// connected to the data pin of the "pulse" output (the middle one)
#define GEIGER_PIN A2

GeigerCounter gc;

void setup(){
    Serial.begin(9600);
    while(!Serial);

    gc.begin(GEIGER_PIN);

    Serial.println("Beginning Geiger Counter Running Test...");
    delay(1000);
}

void loop(){
    
    Serial.print("CPS: "); Serial.println(gc.getCPSRunning());
    Serial.print("Dose: (uSv/hr): "); Serial.println(gc.getDoseRunning());

    delay(1000);
}
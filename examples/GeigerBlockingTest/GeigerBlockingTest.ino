/**
 * @file GeigerBlockingTest.ino
 * @author Tyler Nielsen
 * @brief Example code for Might Ohm Geiger Counter Arduino library
 * @version 0.1
 * @date 2024-04-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/**
 * This example uses the "blocking" functions. These functions will find the measurements through a CPS calculated
 * in the period that is passed to them. These are significantly slower than the "running" functions but are more 
 * consistant. 
 */

#include "GeigerCounter.h"

// can be any interrupt-enabled pin (these vary based on the microcontroller you are using)
// connected to the data pin of the "pulse" output (the middle one)
#define GEIGER_PIN A2
// sample period in ms
#define SAMPLE_PERIOD 1000

GeigerCounter gc;

void setup(){
    Serial.begin(9600);
    while(!Serial);

    gc.begin(GEIGER_PIN);

    Serial.println("Beginning Geiger Counter Blocking Test...");
    delay(1000);
}

void loop(){
    Serial.print("CPS: "); Serial.println(gc.getCPSBlocking(SAMPLE_PERIOD));
    Serial.print("Dose (uSv/hr): "); Serial.println(gc.getDoseBlocking(SAMPLE_PERIOD));

    delay(100);
}
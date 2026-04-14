/* 
 * Project IR_Flame_Sensor
 * Description: This project demonstrates how to use an IR flame sensor with a Particle device. The
 * Author: Mario Cisneros
 * Date: 4/14/2026
 */

// Include Particle Device OS APIs
#include "Particle.h"

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);


const int DO_PIN = D2; // P2's pin connected to DO pin of the flame sensor


void setup() {
  // initialize serial communication
  Serial.begin(9600);
  // initialize the Arduino's pin as an input
  pinMode(DO_PIN, INPUT);
}

void loop() {
  int flame_state = digitalRead(DO_PIN);

  if (flame_state == HIGH)
    Serial.printf("The flame is NOT present => The fire is NOT detected\n");
  else
    Serial.printf("The flame is present => The fire is detected\n");
}


/*
#define AO_PIN A0  // Arduino's pin connected to AO pin of the flame sensor

void setup() {
  // initialize serial communication
  Serial.begin(9600);
}

void loop() {
  int flameValue = analogRead(AO_PIN);

  Serial.println(flameValue);
}
*/
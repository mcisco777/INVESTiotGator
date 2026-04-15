/* 
 * Project Name: Motor Encoder
 * Author: Mario Cisneros
 * Date: 4/13/2026
 */

// Include Particle Device OS APIs
#include "Particle.h"

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

/******************Declare Constants***************/
const int ENC_ADDR = 0x34;
const int MOTOR_TYPE_ADDR = 0x14;
const int BATT_ADDR = 0x00;
const int POLARITY_ADDR = 0x15;
const int PWM_ADDR = 0x1F;
const int FIXED_SPEED_ADDR = 0x33;
const int ENC_TOTAL_ADDR = 0x3C;
const int MOTOR_TYPE_JGB37_520_12V_110RPM = 3;

/******************Declare Variables***************/
int8_t motorType = MOTOR_TYPE_JGB37_520_12V_110RPM;
int8_t motorPolarity = 0;

int8_t tankForward[4]={0,-20,0,20};
int8_t tankReverse[4]={0,20,0,-20};
int8_t tankStop[4]={0,0,0,0};
int8_t hardLeft[4]={0,-20,0,-20};
int8_t hardRight[4]={0,20,0,20};
int8_t leftForward[4]={0,-20,0,10};
int8_t rightForward[4]={0,-10,0,20};
int8_t leftReverse[4]={0,20,0,-10};
int8_t rightReverse[4]={0,10,0,-20};

/*********************Declare Functions*******************/
//bool WireWriteByte(uint8_t val);   //(Send byte data through I2C)
bool WireWriteDataArray(uint8_t reg,int8_t *val,unsigned int len);    //(Send data through I2C)


void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 5000);
  Wire.begin();
  delay(200);
  WireWriteDataArray(MOTOR_TYPE_ADDR,&motorType,1);
  delay(5);
  WireWriteDataArray(POLARITY_ADDR,&motorPolarity,1);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  WireWriteDataArray(FIXED_SPEED_ADDR,tankForward,4);
  delay(2000);
  WireWriteDataArray(FIXED_SPEED_ADDR,tankStop,4);
  delay(1000);
  WireWriteDataArray(FIXED_SPEED_ADDR,hardLeft,4);
  delay(2000);
  WireWriteDataArray(FIXED_SPEED_ADDR,tankStop,4);
  delay(1000);
  WireWriteDataArray(FIXED_SPEED_ADDR,tankReverse,4);
  delay(2000);
  WireWriteDataArray(FIXED_SPEED_ADDR,tankStop,4);
  delay(1000);  
  WireWriteDataArray(FIXED_SPEED_ADDR,rightForward,4);
  delay(2000);
  WireWriteDataArray(FIXED_SPEED_ADDR,tankStop,4);
  delay(1000);
  WireWriteDataArray(FIXED_SPEED_ADDR,leftForward,4);
  delay(2000);
  WireWriteDataArray(FIXED_SPEED_ADDR,tankStop,4);
  delay(1000);
  WireWriteDataArray(FIXED_SPEED_ADDR,hardRight,4);
  delay(2000);
  WireWriteDataArray(FIXED_SPEED_ADDR,tankStop,4);
  delay(1000);  
  WireWriteDataArray(FIXED_SPEED_ADDR,leftReverse,4);
  delay(2000);
  WireWriteDataArray(FIXED_SPEED_ADDR,tankStop,4);
  delay(1000);
  WireWriteDataArray(FIXED_SPEED_ADDR,rightReverse,4);
  delay(2000);
  WireWriteDataArray(FIXED_SPEED_ADDR,tankStop,4);
  delay(1000);

}

bool WireWriteDataArray(uint8_t reg,int8_t *val,unsigned int len){    //(Send data through I2C)
    unsigned int i;

    Wire.beginTransmission(ENC_ADDR);   //(Set the starting address of the task)
    Wire.write(reg);    //(Write the header address)
    for(i = 0; i < len; i++) {
        Wire.write(val[i]);   //(Write the content passed in to I2C)
    }
    if( Wire.endTransmission() != 0 ) {     //(Determine if the write is connected)
        return false;
    }

    return true;
}

/*bool WireWriteByte(uint8_t val){   //(Send byte data through I2C)
    Wire.beginTransmission(ENC_ADDR);
    Wire.write(val);
    if( Wire.endTransmission() != 0 ) {
        return false;
    }
    return true;
}*/
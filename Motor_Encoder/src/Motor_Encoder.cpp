/* 
 * Project Name: Motor Encoder
 * Author: Mario Cisneros
 * Date: 4/13/2026
 */

// Include Particle Device OS APIs
#include "Particle.h"

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

/******************Declare Variables***************/
const int ENC_ADDR = 0x34;
const int MOTOR_TYPE_ADDR = 0x14;
const int BATT_ADDR = 0x00;
const int POLARITY_ADDR = 0x15;
const int PWM_ADDR = 0x1F;
const int FIXED_SPEED_ADDR = 0x33;
const int ENC_TOTAL_ADDR = 0x3C;
const int MOTOR_TYPE_JGB37_520_12V_110RPM = 3;

int8_t MotorType = MOTOR_TYPE_JGB37_520_12V_110RPM;
int8_t MotorPolarity = 0;

int8_t car_forward[4]={0,23,0,-23};
int8_t car_retreat[4]={0,-23,0,23};
int8_t car_stop[4]={0,0,0,0};

/*********************Declare Functions*******************/
//bool WireWriteByte(uint8_t val);   //(Send byte data through I2C)
bool WireWriteDataArray(uint8_t reg,int8_t *val,unsigned int len);    //(Send data through I2C)


void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 5000);
  Wire.begin();
  delay(200);
  WireWriteDataArray(MOTOR_TYPE_ADDR,&MotorType,1);
  delay(5);
  WireWriteDataArray(POLARITY_ADDR,&MotorPolarity,1);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  WireWriteDataArray(FIXED_SPEED_ADDR,car_forward,4);
  delay(2000);
  WireWriteDataArray(FIXED_SPEED_ADDR,car_stop,4);
  delay(1000);
  /* (Car moves backward) */
  WireWriteDataArray(FIXED_SPEED_ADDR,car_retreat,4);
  delay(2000);
  WireWriteDataArray(FIXED_SPEED_ADDR,car_stop,4);
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
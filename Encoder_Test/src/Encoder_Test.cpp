/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);

#define I2C_ADDR        0x34
#define MOTOR_TYPE_ADDR               0x14
#define MOTOR_ENCODER_POLARITY_ADDR   0x15
#define MOTOR_FIXED_PWM_ADDR      0x1F
#define MOTOR_FIXED_SPEED_ADDR    0x33
#define MOTOR_ENCODER_TOTAL_ADDR  0x3C
#define MOTOR_TYPE_JGB37_520_12V_110RPM   3


int8_t MotorType = MOTOR_TYPE_JGB37_520_12V_110RPM;
int8_t MotorEncoderPolarity = 0;
int8_t data[20];

int8_t car_forward[4]={0,23,0,-23};
int8_t car_retreat[4]={0,-23,0,23};
int8_t car_stop[4]={0,0,0,0};
int8_t test[4]={0,20,0,20};

bool WireWriteByte(uint8_t val);
bool WireWriteDataArray(  uint8_t reg, int8_t *val,unsigned int len);

void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected,5000);
  Wire.begin();
  delay(200);
  WireWriteDataArray(MOTOR_TYPE_ADDR,&MotorType,1);
  delay(5);
  WireWriteDataArray(MOTOR_ENCODER_POLARITY_ADDR,&MotorEncoderPolarity,1);

}

void loop() {
  //WireWriteDataArray(MOTOR_FIXED_SPEED_ADDR,test,4);
  WireWriteDataArray(MOTOR_FIXED_SPEED_ADDR,car_forward,4);
  delay(2000);
  WireWriteDataArray(MOTOR_FIXED_SPEED_ADDR,car_stop,4);
  delay(1000);
  /* (Car moves backward) */
  WireWriteDataArray(MOTOR_FIXED_SPEED_ADDR,car_retreat,4);
  delay(2000);
  WireWriteDataArray(MOTOR_FIXED_SPEED_ADDR,car_stop,4);
  delay(1000);  
}




bool WireWriteByte(uint8_t val)   //(Send byte data through I2C)
{
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(val);
    if( Wire.endTransmission() != 0 ) {
        return false;
    }
    return true;
}

bool WireWriteDataArray(  uint8_t reg, int8_t *val,unsigned int len)    //(Send data through I2C)
{
    unsigned int i;

    Wire.beginTransmission(I2C_ADDR);   //(Set the starting address of the task)
    Wire.write(reg);    //(Write the header address)
    for(i = 0; i < len; i++) {
        Wire.write(val[i]);   //(Write the content passed in to I2C)
    }
    if( Wire.endTransmission() != 0 ) {     //(Determine if the write is connected)
        return false;
    }

    return true;
}
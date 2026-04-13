/* 
 * Project Encoder_Driver
 * Author: Mario Cisneros
 * Date: 4/13/2026
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include <Wire.h>

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

#define I2C_ADDR        0x34

#define ADC_BAT_ADDR                  0x00
#define MOTOR_TYPE_ADDR               0x14 //(Set encoder motor type)
#define MOTOR_ENCODER_POLARITY_ADDR   0x15 //(Set the encoder direction polarity)，
//(If the motor speed is completely uncontrollable, either rotating at the fastest speed or stopping. You can reset the value of this address)
//(Range 0 or 1, default 0)
#define MOTOR_FIXED_PWM_ADDR      0x1F //（-100~100）(Fixed PWM control, belongs to open-loop control, range from -100 to 100)
//#define SERVOS_ADDR_CMD 40        
#define MOTOR_FIXED_SPEED_ADDR    0x33 //(Fixed speed control, belongs to closed-loop control)，
//(Unit: pulse count per 10 milliseconds, range (depending on the specific encoder motor, affected by the number of encoder lines, voltage size, load size, etc., generally around ±50))

#define MOTOR_ENCODER_TOTAL_ADDR  0x3C //(Total pulse value of each of the four encoder motors)
//(If the pulse count per revolution of the motor is known to be U, and the diameter of the wheel D is known, then the distance traveled by each wheel can be obtained through pulse counting)
//(For example, if the total pulse count of motor 1 is P, then the distance traveled is (P/U) * (3.14159*D))
//(For different motors, you can test the number of pulses per revolution U by manually rotating 10 times and reading the pulse count, and then take the average value to obtain it)


//(motor type specific values)
/*#define MOTOR_TYPE_WITHOUT_ENCODER        0
#define MOTOR_TYPE_TT                     1
#define MOTOR_TYPE_N20                    2*/
#define MOTOR_TYPE_JGB37_520_12V_110RPM   3 //(Magnetic ring rotates 44 pulses per revolution, reduction ratio: 90, default)

u8 data[20];

uint8_t MotorType = MOTOR_TYPE_JGB37_520_12V_110RPM;    //(Motor mode setting)
uint8_t MotorEncoderPolarity = 0;     //(Motor polarity setting)

int8_t car_forward[4]={0,23,0,-23};   //(Forward)
int8_t car_retreat[4]={0,-23,0,23};   //(Backward)
int8_t car_turnLeft[4]={0,20,0,20}; //(Turn left in place)
int8_t car_stop[4]={0,0,0,0};

/*****************************Define Functions*/
bool WireWriteByte(uint8_t val);   //(Send byte data through I2C)
bool WireWriteDataArray(  uint8_t reg,uint8_t *val,unsigned int len);    //(Send data through I2C)


void setup() {
  Wire.begin();
  delay(200);
  WireWriteDataArray(MOTOR_TYPE_ADDR,&MotorType,1);
  delay(5);
  WireWriteDataArray(MOTOR_ENCODER_POLARITY_ADDR,&MotorEncoderPolarity,1);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
   /* (Car moves forward) */
  WireWriteDataArray(MOTOR_FIXED_SPEED_ADDR,car_forward,4);
  delay(2000);
  WireWriteDataArray(MOTOR_FIXED_SPEED_ADDR,car_stop,4);
  delay(1000);
  /* (Car moves backward) */
  WireWriteDataArray(MOTOR_FIXED_SPEED_ADDR,car_retreat,4);
  delay(2000);
  WireWriteDataArray(MOTOR_FIXED_SPEED_ADDR,car_stop,4);
  delay(1000);    
  /* (Car turns left in place) */
  WireWriteDataArray(MOTOR_FIXED_SPEED_ADDR,car_turnLeft,4);
  delay(600);
  WireWriteDataArray(MOTOR_FIXED_SPEED_ADDR,car_stop,4);
  delay(1000);
  /* (Car moves forward in the current direction) */ 
  WireWriteDataArray(MOTOR_FIXED_SPEED_ADDR,car_forward,4);
  delay(2000);
  WireWriteDataArray(MOTOR_FIXED_SPEED_ADDR,car_stop,4);
  while(1);
}

bool WireWriteByte(uint8_t val){   //(Send byte data through I2C)
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(val);
    if( Wire.endTransmission() != 0 ) {
        return false;
    }
    return true;
}
bool WireWriteDataArray(  uint8_t reg,uint8_t *val,unsigned int len){    //(Send data through I2C)
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

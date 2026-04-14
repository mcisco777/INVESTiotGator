/* 
 * Project Invest_IoT_Gator
 * Description: Tracked inspection robot.  
 * Author: Mario Cisneros
 * Date: 4/14/2026
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "math.h"
#include "Air_Quality_Sensor.h"

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

const int MPU_ADDR = 0x68;

const int AO_PIN = A0;  // P2's pin connected to AO pin of the flame sensor

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


byte accel_x_h, accel_x_l, accel_y_h, accel_y_l, accel_z_h, accel_z_l;
int16_t accelX, accelY, accelZ;
float xGaccel, yGaccel, zGaccel;
unsigned int lastTime;
float pitch, roll, radians;

int quality;

/*********************Declare Objects**********************/
AirQualitySensor sensor(A2);

/*********************Declare Functions********************/
bool WireWriteDataArray(uint8_t reg,int8_t *val,unsigned int len);    //(Send data through I2C)
float radToDeg(float radians);

void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 5000);
  
  while(!Particle.connected()) {
    Particle.connect();
    delay(100);           //Small delay needed
    Serial.printf("x");
  }
  Serial.printf("\n\n");
  delay(300);
  while(!Serial);
    Serial.printf("\nWaiting on sensor to initiate...");
    delay(20000);
  if(sensor.init()){
    Serial.printf("Sensor ready.");
  }
  else{
    Serial.printf("Sensor ERROR");
    quality = 0;
  }
// Begin I2C communications
  Wire.begin();
  delay(200);
  WireWriteDataArray(MOTOR_TYPE_ADDR,&motorType,1);
  delay(5);
  WireWriteDataArray(POLARITY_ADDR,&motorPolarity,1);

  // Begin transmission to MPU-6050
  Wire.beginTransmission(MPU_ADDR);
  // Select and write to PWR_MGMT1 register
  Wire.write(0x6B);
  Wire.write(0x00); // Wakes MPU-6050
  //End transmission and close connection
  Wire.endTransmission(true);
}

void loop() {
  // Set the "pointer" to the 0x3B memory location of the MPU (x-axis) and wait for data
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // starting with register 0x3B (x-axis)
  Wire.endTransmission(false); // send the set pointer command and keep active

  Wire.requestFrom(MPU_ADDR, 6, true);
  accel_x_h = Wire.read(); // x accel MSB
  accel_x_l = Wire.read(); // x accel LSB
    
  accel_y_h = Wire.read(); // y accel MSB
  accel_y_l = Wire.read(); // y accel LSB

  accel_z_h = Wire.read(); // z accel MSB
  accel_z_l = Wire.read(); // z accel LSB

  accelX = (accel_x_h << 8) | accel_x_l;
  xGaccel = accelX*0.00006103701895199438;
      
  accelY = (accel_y_h << 8) | accel_y_l;
  yGaccel = accelY*0.00006103701895199438;

  accelZ = (accel_z_h << 8) | accel_z_l;
  zGaccel = accelZ*0.00006103701895199438;
  
  roll = radToDeg(-asin(xGaccel));
  pitch = radToDeg(atan2(yGaccel,zGaccel));
  
  if((millis()-lastTime > 1000)) {
    Serial.printf("Pitch = %0.2f degrees\n",pitch);
    Serial.printf("Roll = %0.2f degrees\n\n", roll);
    
    //Serial.printf("X-axis acceleration is %i\n", accelX);
    Serial.printf("X-axis 'g' acceleration is %0.5f\n", xGaccel);
    //Serial.printf("Y-axis acceleration is %i\n", accelY);
    Serial.printf("Y-axis 'g' acceleration is %0.5f\n", yGaccel);
    //Serial.printf("Z-axis acceleration is %i\n", accelZ);
    Serial.printf("Z-axis 'g' acceleration is %0.5f\n\n", zGaccel);
  lastTime = millis();
  }

  quality = sensor.slope();
  Serial.printf("Air quality value: %i - ", sensor.getValue());
  if(quality == AirQualitySensor::FORCE_SIGNAL){
    Serial.printf("High pollution! Force signal active.\n");
  }
  else if(quality == AirQualitySensor::HIGH_POLLUTION){
    Serial.printf("High Pollution!\n");
  }
  else if(quality == AirQualitySensor::LOW_POLLUTION){
    Serial.printf("Low Pollution!\n");
  }
  else if(quality == AirQualitySensor::FRESH_AIR){
    Serial.printf("Fresh air.\n");
  }
  int flameValue = analogRead(AO_PIN); // value of ~ 3900 when the flame is not present and ~ 200 (or 0) when the flame is present
  Serial.printf("Flame Value: %d, ~ 4000 when the flame is not present and (200 - 0) when close to flame\n", flameValue);
  delay(1000);
}

float radToDeg(float radians){
    float degrees;
    degrees = ((180/M_PI)*radians);
    return degrees; 
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
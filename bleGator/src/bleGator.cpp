/* 
 * Project bleGator
 * Description: Bluetooth gator communicator
 * Author: Mario Cisneros
 * Date: 4/14/2026
 */

#include "Particle.h"

SYSTEM_MODE(SEMI_AUTOMATIC); //Using BLE and not Wifi
SYSTEM_THREAD(ENABLED);


/******************************Declare Constants************************/
const size_t UART_TX_BUF_SIZE = 30;

// These UUIDs were defined by Nordic Semiconductor and are now the defacto standard for
// UART-like services over BLE. Many apps support the UUIDs now, like the Adafruit Bluefruit app.
const BleUuid SERVICEUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid RXUUID("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid TXUUID("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");

const int ENC_ADDR = 0x34;
const int MOTOR_TYPE_ADDR = 0x14;
const int BATT_ADDR = 0x00;
const int POLARITY_ADDR = 0x15;
const int FIXED_SPEED_ADDR = 0x33;
const int MOTOR_TYPE_JGB37_520_12V_110RPM = 3;

/******************************Declare Variables************************/
uint8_t txBuf[UART_TX_BUF_SIZE];
uint8_t i;
//unsigned int last1;
bool dataReceived = false;

int8_t motorType = MOTOR_TYPE_JGB37_520_12V_110RPM;
int8_t motorPolarity = 0;

int8_t tankForward[4]={0,-20,0,20};
int8_t tankReverse[4]={0,20,0,-20};
int8_t tankStop[4]={0,0,0,0};
int8_t hardLeft[4]={0,-20,0,-20};
int8_t hardRight[4]={0,20,0,20};
/*int8_t leftForward[4]={0,-20,0,10};
int8_t rightForward[4]={0,-10,0,20};
int8_t leftReverse[4]={0,20,0,-10};
int8_t rightReverse[4]={0,10,0,-20};*/

/*******************************Declare Function************************/
void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context);
bool WireWriteDataArray(uint8_t reg,int8_t *val,unsigned int len);    //(Send data through I2C)


/*******************************Declare Object**************************/

BleCharacteristic txCharacteristic("tx", BleCharacteristicProperty::NOTIFY, TXUUID, SERVICEUUID);
BleCharacteristic rxCharacteristic("rx", BleCharacteristicProperty::WRITE_WO_RSP, RXUUID, SERVICEUUID, onDataReceived, NULL);
BleAdvertisingData data;



void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 15000);
    
  Wire.begin();
  delay(200);
  WireWriteDataArray(MOTOR_TYPE_ADDR,&motorType,1);
  delay(5);
  WireWriteDataArray(POLARITY_ADDR,&motorPolarity,1);

  BLE.on();
  BLE.addCharacteristic(txCharacteristic);
  BLE.addCharacteristic(rxCharacteristic);
  data.appendServiceUUID(SERVICEUUID);
  BLE.advertise(&data);

  Serial.printf("Photon2 BLE Address: %s\n", BLE.address().toString().c_str());

}

void loop() {
  if(dataReceived){
    Serial.printf("Data received, sending response...");
    if((data[2] == 0x35) && (data[3] == 0x31)){ 
      WireWriteDataArray(ENC_ADDR, tankForward, 4);
      if((data[2] == 0x35) && (data[3] == 0x30)){
        WireWriteDataArray(ENC_ADDR, tankStop, 4);
      }
    }
  }    
}

//onDataReceived is used to receive data from Bluefruit Connect App
void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) {
  uint8_t i;
  bool dataReceived = true;
  Serial.printf("Received data from: %02X:%02X:%02X:%02X:%02X:%02X \n", peer.address()[0], peer.address()[1],peer.address()[2], peer.address()[3], peer.address()[4], peer.address()[5]);

  Serial.printf("Bytes: ");
  for(i=0; i< len; i++){
    Serial.printf("%02X ", data[i]);
  }
  Serial.printf("\n");
  Serial.printf("Message: %s\n",(char *)data);
  if((data[0] == 0x21) && (data[1] == 0x42)){
    Serial.printf("1st value: %02X\n2nd value: %02X\n3rd value: %02X\n", data[2], data[3], data[4]);
  }
  else{
    WireWriteDataArray(ENC_ADDR, tankStop, 4);
    }
  
  //dataReceived = false;
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
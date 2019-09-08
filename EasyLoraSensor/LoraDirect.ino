#include <SPI.h>
#include <LoRa.h> //  https://github.com/sandeepmistry/arduino-LoRa
#include "EasyLoraSensor-Globals.h"
// This library exposes the LoRa radio directly, and allows you to send data to any radios in range with same radio parameters.

// CAPABILITIES byte: first bit 1 if able to send, second bit 1 if able to receive
#define CAPABILITY_BYTE  1 // Only SENDING implemented here

struct Lora_SysInfoStruct
{
  // header field
  byte header = 255;
  byte ID = 1;
  byte sourcelUnit;
  byte destUnit=0; // default destination is to broadcast
  // start of payload
  byte mac[6];
  word build;
  byte nodeType;
  byte cap = CAPABILITY_BYTE;
  byte NameLength; // length of the following char array to send
  char Name[25];  
};

struct Lora_SensorDataStruct_Raw
{
  // header field
  byte header = 255; 
  byte ID = 5;
  byte sourcelUnit;
  byte destUnit;
  // start of payload
  word plugin_id; 
  word idx;
  byte samplesetcount=0;
  byte valuecount;  
  float Values[VARS_PER_TASK];  // use original float data
};

struct Lora_SensorDataStruct_TTN
{
  // header field
  byte header = 255; 
  byte ID = 6;
  byte sourcelUnit;
  byte destUnit;
  // start of payload
  word plugin_id; 
  word idx;
  byte samplesetcount=0;
  byte valuecount;  
  byte data[8];    // implement base16Encode thing if you like
};

struct Lora_CommandDataStruct
{
  // header field
  byte header = 255;
  byte ID = 7;
  byte sourcelUnit;
  byte destUnit;
  // start of payload
  byte CommandLength; // length of the following char array to send 
  char CommandLine[64];
};

void(*Reboot)(void) = 0;

void initLora() {
  LoRa.setPins(LORA_SS, LORA_RESET, LORA_DIO0);
  if (!LoRa.begin(LORA_FREQ)) {
    String logl = "Starting LoRa failed!";
    addLog(0, logl);
    delay(10000);
    Reboot();
  }
  LoRa.setSpreadingFactor(LORA_SF);
  LoRa.setSignalBandwidth(LORA_BW);
  LoRa.setCodingRate4(LORA_CODE);
  LoRa.setSyncWord(LORA_SYNC); 
  LoRa.enableCrc();
}


boolean sendSysInfo() {
  Serial.print("Sending info packet: ");
  struct Lora_SysInfoStruct dataReply;
  
  memcpy(dataReply.mac, mac, 6);
  memset(dataReply.Name,0,sizeof(dataReply.Name));
  strncpy(dataReply.Name, Settings.Name,24);
  dataReply.NameLength = strlen(Settings.Name);

  dataReply.sourcelUnit = byte(UNIT);
  dataReply.build = word(BUILD);
  dataReply.nodeType = byte(NODE_TYPE_ID);
  
  Serial.print(dataReply.sourcelUnit);
  Serial.println(dataReply.Name); // DEBUG  
  // send packet  
  byte psize = sizeof(Lora_SysInfoStruct);
  if (dataReply.NameLength<24) { // do not send zeroes
    psize = psize-(24-dataReply.NameLength+1);
  }
  return lorasend((byte*) &dataReply, psize);
}

boolean sendData(struct EventStruct *event) {
  if (!Settings.TaskDeviceSendData[event->TaskIndex])
    return false;
  Serial.print("Sending data packet: ");
  struct Lora_SensorDataStruct_Raw dataReply;

  dataReply.sourcelUnit = byte(UNIT);
  dataReply.destUnit = byte(LORA_DESTINATION_UNIT);
  
  dataReply.plugin_id = word(Settings.TaskDeviceNumber[event->TaskIndex]);  
  dataReply.idx = word(Settings.TaskDeviceID[event->TaskIndex]);
  dataReply.valuecount = byte(getValueCountFromSensorType(event->sensorType));
  
  for (byte x = 0; x < dataReply.valuecount; x++)
   dataReply.Values[x] = UserVar[event->BaseVarIndex + x];
  
  Serial.print(dataReply.idx);
  Serial.print(",");  
  Serial.println(dataReply.Values[0]); // DEBUG
  // send packet  

  byte psize = sizeof(Lora_SensorDataStruct_Raw);
  if (dataReply.valuecount<4) { // do not send zeroes
    psize = psize - ((4-dataReply.valuecount)*4);
  }

  return lorasend((byte*) &dataReply, psize);
}

boolean lorasend(const uint8_t *buffer, size_t size)
{
  unsigned long StartTime = millis();
  Serial.print("Transmit req: "); // debug
  Serial.println(StartTime); // debug   
  if (NextTransmit<=StartTime){
   LoRa.beginPacket(); // go to radio stdby mode
   LoRa.write(buffer, size);
   StartTime = millis();
   LoRa.endPacket();
   LastTransmit = millis();
   #if SLEEP_RADIO_AFTERSEND   
    LoRa.sleep();      // go to radio sleep mode - save 2mA
   #endif
   NextTransmit = ((LastTransmit-StartTime)*LORA_DUTY)+LastTransmit;
   Serial.print("Transmitting @"); // debug
   Serial.println(LastTransmit); // debug   
   return true;
  }
  Serial.print("Transmit skipped, next possible @"); // debug
  Serial.println(NextTransmit); // debug  
  return false;
}

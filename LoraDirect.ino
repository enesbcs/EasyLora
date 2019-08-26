#include <SPI.h>
#include <LoRa.h> //  https://github.com/sandeepmistry/arduino-LoRa
#include "EasyLoraSensor-Globals.h"

void(*Reboot)(void) = 0;

struct Lora_SysInfoStruct
{
  byte header = 255;
  byte ID = 1;
  byte sourcelUnit;
  byte mac[6];
  word build;
  char Name[25];
  byte nodeType;
};

struct Lora_SensorInfoStruct
{
  byte header = 255;
  byte ID = 3;
  byte sourcelUnit;
  byte destUnit;
  byte destTaskIndex;
  byte sensorType;
  char taskName[26];
  char ValueNames[VARS_PER_TASK][26];
};

struct Lora_SensorDataStruct
{
  byte header = 255;
  byte ID = 5;
  byte sourcelUnit;
  byte destUnit;
  byte destTaskIndex;
  float Values[VARS_PER_TASK];
};

struct Lora_CommandDataStruct
{
  byte header = 255;
  byte ID = 7;
  byte sourcelUnit;
  byte destUnit;
  char commandline[64];
};

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
  strncpy(dataReply.Name, Settings.Name,24);

  dataReply.sourcelUnit = byte(UNIT);
  dataReply.build = word(BUILD);
  dataReply.nodeType = byte(NODE_TYPE_ID);
  
  Serial.print(dataReply.sourcelUnit);
  Serial.println(dataReply.Name); // DEBUG  
  // send packet  
  return lorasend((byte*) &dataReply, sizeof(Lora_SysInfoStruct));
}

boolean sendHeader(struct EventStruct *event) {
  if (!Settings.TaskDeviceSendData[event->TaskIndex])
    return false;
  struct Lora_SensorInfoStruct dataReply;

  Serial.print("Sending header packet: ");
  LoadTaskSettings(event->TaskIndex);
  strcpy(dataReply.taskName, ExtraTaskSettings.TaskDeviceName);
  dataReply.sourcelUnit = UNIT;
  dataReply.destUnit = LORA_DESTINATION_UNIT;
  dataReply.destTaskIndex = Settings.TaskDeviceID[event->TaskIndex];
  dataReply.sensorType =  event->sensorType;
  for (byte x = 0; x < VARS_PER_TASK; x++)
   strcpy(dataReply.ValueNames[x], ExtraTaskSettings.TaskDeviceValueNames[x]);     
  Serial.print(dataReply.destTaskIndex);
  Serial.print(",");
  Serial.print(dataReply.sensorType); // DEBUG
  Serial.print(",");
  Serial.print(dataReply.taskName); // DEBUG
  Serial.print(",");  
  Serial.println(dataReply.ValueNames[0]);
  // send packet  
  return lorasend((byte*) &dataReply, sizeof(Lora_SensorInfoStruct));
}

boolean sendData(struct EventStruct *event) {
  if (!Settings.TaskDeviceSendData[event->TaskIndex])
    return false;
  Serial.print("Sending data packet: ");
  struct Lora_SensorDataStruct dataReply;
  
  dataReply.sourcelUnit = UNIT;
  dataReply.destUnit = LORA_DESTINATION_UNIT;
  dataReply.destTaskIndex = Settings.TaskDeviceID[event->TaskIndex];
  for (byte x = 0; x < VARS_PER_TASK; x++)
   dataReply.Values[x] = UserVar[event->BaseVarIndex + x];
  
  Serial.print(dataReply.destTaskIndex);
  Serial.print(",");  
  Serial.println(dataReply.Values[0]); // DEBUG
  // send packet  
  return lorasend((byte*) &dataReply, sizeof(Lora_SensorDataStruct));
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

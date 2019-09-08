#include <SPI.h>
#include <LoRa.h> //  https://github.com/sandeepmistry/arduino-LoRa
#include "EasyLoraRemote-Globals.h"
// This library exposes the LoRa radio directly, and allows you to send data to any radios in range with same radio parameters.

// CAPABILITIES byte: first bit 1 if able to send, second bit 1 if able to receive
#define CAPABILITY_BYTE  2 // Only RECEIVING implemented here (partially as Command execution ID=7 watched only)

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
  word plugin_id;       // if (plugin_id <= 127) 1 byte ID
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

  LoRa.onReceive(onReceive);
  // put the radio into receive mode
  LoRa.receive();  
}


boolean sendSysInfo() {
  Serial.print(F("Sending info packet: "));
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
  Serial.print(F("Sending data packet: "));
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
   LoRa.receive();                     // go back into receive mode
   NextTransmit = ((LastTransmit-StartTime)*LORA_DUTY)+LastTransmit;
   Serial.print(F("Transmitting @")); // debug
   Serial.println(LastTransmit); // debug   
   return true;
  }
  Serial.print(F("Transmit skipped, next possible @")); // debug
  Serial.println(NextTransmit); // debug  
  return false;
}

boolean sendCMD(char* cmdline) {
  Serial.print(F("Sending command packet: "));
  struct Lora_CommandDataStruct cmdPkt;
  String cmdString(cmdline);
  
  cmdPkt.sourcelUnit = byte(UNIT);
  cmdPkt.destUnit = byte(LORA_DESTINATION_UNIT);
  strcpy(cmdPkt.CommandLine, cmdline);
  cmdPkt.CommandLength = cmdString.length();
  
  // send packet  
  byte psize = sizeof(Lora_CommandDataStruct);
  if (cmdPkt.CommandLength<64) { // do not send zeroes
    psize = psize -(64-cmdPkt.CommandLength);
  }
  return lorasend((byte*) &cmdPkt, psize);
}

 
void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return
  Serial.print(F("Received packet "));

  if (packetSize>70) {
    packetSize=70;
  }
  byte dbuffer[71];
  struct Lora_CommandDataStruct cmdPkt;
  // read packet
  for (int i = 0; i < packetSize; i++) {
    dbuffer[i] = LoRa.read();
  }
//  dbuffer[packetSize]=0; 
  memcpy(&cmdPkt,&dbuffer,packetSize);
  cmdPkt.CommandLine[cmdPkt.CommandLength] = 0; // make it zero terminated
  if ((cmdPkt.header==255 and cmdPkt.ID==7) and (cmdPkt.destUnit==UNIT or cmdPkt.destUnit==0)){ // only ID 7 command packet is accepted
//   Serial.println(cmdPkt.CommandLine);
   struct EventStruct TempEvent;
   String request = (char*)cmdPkt.CommandLine;
   parseCommandString(&TempEvent, request);
   TempEvent.Source = VALUE_SOURCE_SYSTEM;
   if (!PluginCall(PLUGIN_WRITE, &TempEvent, request))            // call P001 plugin
     ExecuteCommand(VALUE_SOURCE_SYSTEM, cmdPkt.CommandLine);     // or execute system command
  }
}

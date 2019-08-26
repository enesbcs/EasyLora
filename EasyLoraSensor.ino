/****************************************************************************************************************************\
   Arduino project "Easy LORA Sensor"

   This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABIlLITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
   You received a copy of the GNU General Public License along with this program in file 'License.txt'.

   IDE download    : https://www.arduino.cc/en/Main/Software

   Additional information about licensing can be found at : http://www.gnu.org/licenses
  \*************************************************************************************************************************/

// This file incorporates work covered by the following copyright and permission notice:

// ********************************************************************************
//   User specific configuration
// ********************************************************************************

#include "EasyLoraSensor-Globals.h"
#include "__Plugin.h"
#include "Misc.h"
#include "LoraDirect.h"

int deviceCount = -1;

float UserVar[VARS_PER_TASK * TASKS_MAX];

unsigned long timerSensor[TASKS_MAX];
unsigned long timer;
unsigned long timer100ms;
unsigned long timer1s;
unsigned long timerwd;
unsigned long wdcounter = 0;

boolean (*Plugin_ptr[PLUGIN_MAX])(byte, struct EventStruct*, String&);
byte Plugin_id[PLUGIN_MAX];

String dummyString = "";

boolean systemOK = false;
boolean sysinfosent = false;
boolean headersent[TASKS_MAX];

unsigned long LastTransmit = 0;
unsigned long NextTransmit = 0;

unsigned long start = 0;
unsigned long elapsed = 0;


/*********************************************************************************************\
   SETUP
  \*********************************************************************************************/
void setup()
{
#if LEDPIN>-1
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, 1);
#endif

#if !DISABLE_USART
  Serial.begin(115200);
  #if WAIT_SERIAL_AT_STARTUP
   while (! Serial);
  #endif
#endif

  LoadSettings();
  ExtraTaskSettings.TaskIndex = 255; // make sure this is an unused nr to prevent cache load on boot

  String logl = "EasyLoraSensor v" + String(VERSION);
  addLog(0, logl);

  if (mac[5] == 0)
    mac[5] = UNIT; // make sure every unit has a unique mac address
  hardwareInit();
  PluginInit();
  /*
      Set Devices with create_task() command
       First param: TaskID (1-4)
       Second param: PLUGIN_ID of selected device
       Third param:  Interval in seconds
       Fourth param: First GPIO
       Fifth param:  Port value (for analog plugin)
       Sixth param:  DestinationTaskNumber at the receiving side!
  */
  create_task(1, 1, 48, 5, 0, 2);  // D5 = digital
  create_task(2, 2, 62, 0, 9, 3);  // D9-ADC12 = BATTERYstate analog!
  initLora(); //CPluginInit

  logl = "Booting Build nr: " + String(BUILD);
  addLog(0, logl);

  sysinfosent = sendSysInfo(); // try to send alive packet at startup

  for (byte x = 0; x < TASKS_MAX; x++)
    if (Settings.TaskDeviceTimer[x] != 0)
      timerSensor[x] = millis() + 4000 + (x * 1000);

  timer = millis() + 4000; // startup delay 4 sec

  timer100ms = millis() + 100; // timer for periodic actions 10 x per/sec
  timer1s = millis() + 1000;   // timer for periodic actions once per/sec
  timerwd = millis() + 30000;  // timer for watchdog once per 30 sec
  systemOK = true;
#if LEDPIN>-1
  digitalWrite(LEDPIN, 0);
#endif
}


/*********************************************************************************************\
   MAIN LOOP
  \*********************************************************************************************/
void loop()
{
  if (systemOK)
  {
    if (millis() > timer100ms)
      run10TimesPerSecond();

    if (millis() > timer1s)
      runOncePerSecond();

    if (millis() > timerwd)
      runEach30Seconds();

    backgroundtasks();

  }
  else
    delay(1);
}


/*********************************************************************************************\
   Tasks that run 10 times per second
  \*********************************************************************************************/
void run10TimesPerSecond()
{
  start = micros();
  timer100ms = millis() + 100;
  PluginCall(PLUGIN_TEN_PER_SECOND, 0, dummyString);
  elapsed = micros() - start;
}


/*********************************************************************************************\
   Tasks each second
  \*********************************************************************************************/
void runOncePerSecond()
{
  timer1s = millis() + 1000;
  checkSensors();

  unsigned long timer = micros();
  PluginCall(PLUGIN_ONCE_A_SECOND, 0, dummyString);

  timer = micros() - timer;

}

/*********************************************************************************************\
   Tasks each 30 seconds
  \*********************************************************************************************/
void runEach30Seconds()
{
  wdcounter++;
  timerwd = millis() + 30000;
  String log = F("WD   : Uptime ");
  log += wdcounter / 2;
  addLog(0, log);
  if (!sysinfosent) { // try to send sysinfo packet if not sent already
    sysinfosent = sendSysInfo();
  }
}


/*********************************************************************************************\
   Check sensor timers
  \*********************************************************************************************/
void checkSensors()
{
  byte TasksByPriority[TASKS_MAX];
  for (byte x = 0; x < TASKS_MAX; x++)
  {
    TasksByPriority[x] = x;
  }
  if (TASKS_MAX > 1) { // if more than 1 possible task defined, try to sort them according to their next report time
    int t1;
    int t2;
    for (byte x = 0; x < TASKS_MAX - 1; x++)
    {
      for (byte y = 0; y < (TASKS_MAX - (x + 1)); y++) {
        t1 = TasksByPriority[y];
        t2 = TasksByPriority[y + 1];
        if ((timerSensor[t1] > timerSensor[t2]) && ((timerSensor[t1] + Settings.TaskDeviceTimer[t1] * 1000) > (timerSensor[t2] + Settings.TaskDeviceTimer[t2] * 1000))) {
          TasksByPriority[y] = t2;
          TasksByPriority[y + 1] = t1;
        }
      }
    }
  }
  int t;
  for (byte x = 0; x < TASKS_MAX; x++)
  {
    t = TasksByPriority[x];
    if ((Settings.TaskDeviceNumber[t] > 0) && (Settings.TaskDeviceTimer[t] > 0) && (millis() > timerSensor[t]))
    {
      Serial.print("Task: ");
      Serial.print(t);
      if (SensorSendTask(t)) { // only set the new report time if sending succeeds
        timerSensor[t] = millis() + Settings.TaskDeviceTimer[t] * 1000;
        if (timerSensor[t] == 0) // small fix if result is 0, else timer will be stopped...
          timerSensor[t] = 1;
      } else
      {
        timerSensor[t] = NextTransmit + 1000; // othwerise re-schedule to the next possible transmit time
      }
    }
  }
}


/*********************************************************************************************\
   send specific sensor task data
  \*********************************************************************************************/
boolean SensorSendTask(byte TaskIndex)
{
  boolean success = false;
  if (Settings.TaskDeviceID[TaskIndex] != 0)
  {
    byte varIndex = TaskIndex * VARS_PER_TASK;

    byte DeviceIndex = getDeviceIndex(Settings.TaskDeviceNumber[TaskIndex]);

    struct EventStruct TempEvent;
    TempEvent.TaskIndex = TaskIndex;
    TempEvent.BaseVarIndex = varIndex;
    //TempEvent.idx = Settings.TaskDeviceID[TaskIndex];
    TempEvent.sensorType = Device[DeviceIndex].VType;

    if (!headersent[TaskIndex]) { // try to send header informations if not sent already
      headersent[TaskIndex] = sendHeader(&TempEvent);
    }

    success = PluginCall(PLUGIN_READ, &TempEvent, dummyString); // get current Values from plugin

    if (success)
    {
      success = sendData(&TempEvent);
    }
  }
  return success;
}


/*********************************************************************************************\
   run background tasks
  \*********************************************************************************************/
void backgroundtasks()
{
  yield();
}

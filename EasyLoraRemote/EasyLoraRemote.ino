/****************************************************************************************************************************\
   Arduino project "Easy LORA Remote"

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

#include "EasyLoraRemote-Globals.h"
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

  String logl = "EasyLoraRemote v" + String(VERSION);
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
       Sixth param:  IDX at the receiving side!
  */
  Settings.TaskDevicePin1PullUp[1]=0;
  create_task(1, 1, 0, 2, 0, 255); // only one plugin activated P001 to enable GPIO, PWM, PULSE, LONGPULSE, TONE commands

  // Init LoRa radio
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
  checkSystemTimers();
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
}


/*********************************************************************************************\
   send specific sensor task data
  \*********************************************************************************************/
boolean SensorSendTask(byte TaskIndex)
{
}

/*********************************************************************************************\
   set global system timer
  \*********************************************************************************************/
boolean setSystemTimer(unsigned long timer, byte plugin, byte Par1, byte Par2, byte Par3)
{
  // plugin number and par1 form a unique key that can be used to restart a timer
  // first check if a timer is not already running for this request
  boolean reUse = false;
  for (byte x = 0; x < SYSTEM_TIMER_MAX; x++)
    if (systemTimers[x].timer != 0)
    {
      if ((systemTimers[x].plugin == plugin) && (systemTimers[x].Par1 == Par1))
      {
        systemTimers[x].timer = millis() + timer;
        reUse = true;
        break;
      }
    }

  if (!reUse)
  {
    // find a new free timer slot...
    for (byte x = 0; x < SYSTEM_TIMER_MAX; x++)
      if (systemTimers[x].timer == 0)
      {
        systemTimers[x].timer = millis() + timer;
        systemTimers[x].plugin = plugin;
        systemTimers[x].Par1 = Par1;
        systemTimers[x].Par2 = Par2;
        systemTimers[x].Par3 = Par3;
        break;
      }
  }
}


/*********************************************************************************************\
   set global system command timer
  \*********************************************************************************************/
boolean setSystemCMDTimer(unsigned long timer, String& action)
{
  for (byte x = 0; x < SYSTEM_CMD_TIMER_MAX; x++)
    if (systemCMDTimers[x].timer == 0)
    {
      systemCMDTimers[x].timer = millis() + timer;
      systemCMDTimers[x].action = action;
      break;
    }
}


/*********************************************************************************************\
   check global system timers
  \*********************************************************************************************/
boolean checkSystemTimers()
{
  for (byte x = 0; x < SYSTEM_TIMER_MAX; x++)
    if (systemTimers[x].timer != 0)
    {
      if (timeOut(systemTimers[x].timer))
      {
        struct EventStruct TempEvent;
        TempEvent.Par1 = systemTimers[x].Par1;
        TempEvent.Par2 = systemTimers[x].Par2;
        TempEvent.Par3 = systemTimers[x].Par3;
        for (byte y = 0; y < PLUGIN_MAX; y++)
          if (Plugin_id[y] == systemTimers[x].plugin)
            Plugin_ptr[y](PLUGIN_TIMER_IN, &TempEvent, dummyString);
        systemTimers[x].timer = 0;
      }
    }

  for (byte x = 0; x < SYSTEM_CMD_TIMER_MAX; x++)
    if (systemCMDTimers[x].timer != 0)
      if (timeOut(systemCMDTimers[x].timer))
      {
        struct EventStruct TempEvent;
        parseCommandString(&TempEvent, systemCMDTimers[x].action);
        if (!PluginCall(PLUGIN_WRITE, &TempEvent, systemCMDTimers[x].action))
          ExecuteCommand(VALUE_SOURCE_SYSTEM, systemCMDTimers[x].action.c_str());
        systemCMDTimers[x].timer = 0;
        systemCMDTimers[x].action = "";
      }
}


/*********************************************************************************************\
   run background tasks
  \*********************************************************************************************/
void backgroundtasks()
{
  yield();
}

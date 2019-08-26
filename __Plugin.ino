//********************************************************************************
// Initialize all plugins that where defined earlier
// and initialize the function call pointer into the plugin array
//********************************************************************************
void PluginInit(void)
{
  byte x;

  // Clear pointer table for all plugins
  for (x = 0; x < PLUGIN_MAX; x++)
  {
    Plugin_ptr[x] = 0;
    Plugin_id[x] = 0;
  }

  x = 0;

#ifdef PLUGIN_001
  Plugin_id[x] = 1; Plugin_ptr[x++] = &Plugin_001;
#endif

#ifdef PLUGIN_002
  Plugin_id[x] = 2; Plugin_ptr[x++] = &Plugin_002;
#endif

#ifdef PLUGIN_003
  Plugin_id[x] = 3; Plugin_ptr[x++] = &Plugin_003;
#endif

#ifdef PLUGIN_004
  Plugin_id[x] = 4; Plugin_ptr[x++] = &Plugin_004;
#endif

#ifdef PLUGIN_005
  Plugin_id[x] = 5; Plugin_ptr[x++] = &Plugin_005;
#endif

#ifdef PLUGIN_006
  Plugin_id[x] = 6; Plugin_ptr[x++] = &Plugin_006;
#endif

#ifdef PLUGIN_007
  Plugin_id[x] = 7; Plugin_ptr[x++] = &Plugin_007;
#endif

#ifdef PLUGIN_008
  Plugin_id[x] = 8; Plugin_ptr[x++] = &Plugin_008;
#endif

#ifdef PLUGIN_009
  Plugin_id[x] = 9; Plugin_ptr[x++] = &Plugin_009;
#endif

#ifdef PLUGIN_010
  Plugin_id[x] = 10; Plugin_ptr[x++] = &Plugin_010;
#endif

#ifdef PLUGIN_011
  Plugin_id[x] = 11; Plugin_ptr[x++] = &Plugin_011;
#endif

#ifdef PLUGIN_012
  Plugin_id[x] = 12; Plugin_ptr[x++] = &Plugin_012;
#endif

#ifdef PLUGIN_013
  Plugin_id[x] = 13; Plugin_ptr[x++] = &Plugin_013;
#endif

#ifdef PLUGIN_014
  Plugin_id[x] = 14; Plugin_ptr[x++] = &Plugin_014;
#endif

#ifdef PLUGIN_015
  Plugin_id[x] = 15; Plugin_ptr[x++] = &Plugin_015;
#endif

#ifdef PLUGIN_016
  Plugin_id[x] = 16; Plugin_ptr[x++] = &Plugin_016;
#endif

#ifdef PLUGIN_017
  Plugin_id[x] = 17; Plugin_ptr[x++] = &Plugin_017;
#endif

#ifdef PLUGIN_018
  Plugin_id[x] = 18; Plugin_ptr[x++] = &Plugin_018;
#endif

#ifdef PLUGIN_019
  Plugin_id[x] = 19; Plugin_ptr[x++] = &Plugin_019;
#endif

#ifdef PLUGIN_020
  Plugin_id[x] = 20; Plugin_ptr[x++] = &Plugin_020;
#endif

#ifdef PLUGIN_021
  Plugin_id[x] = 21; Plugin_ptr[x++] = &Plugin_021;
#endif

#ifdef PLUGIN_022
  Plugin_id[x] = 22; Plugin_ptr[x++] = &Plugin_022;
#endif

#ifdef PLUGIN_023
  Plugin_id[x] = 23; Plugin_ptr[x++] = &Plugin_023;
#endif

#ifdef PLUGIN_024
  Plugin_id[x] = 24; Plugin_ptr[x++] = &Plugin_024;
#endif

#ifdef PLUGIN_025
  Plugin_id[x] = 25; Plugin_ptr[x++] = &Plugin_025;
#endif

#ifdef PLUGIN_026
  Plugin_id[x] = 26; Plugin_ptr[x++] = &Plugin_026;
#endif

#ifdef PLUGIN_027
  Plugin_id[x] = 27; Plugin_ptr[x++] = &Plugin_027;
#endif

#ifdef PLUGIN_028
  Plugin_id[x] = 28; Plugin_ptr[x++] = &Plugin_028;
#endif

#ifdef PLUGIN_029
  Plugin_id[x] = 29; Plugin_ptr[x++] = &Plugin_029;
#endif

#ifdef PLUGIN_030
  Plugin_id[x] = 30; Plugin_ptr[x++] = &Plugin_030;
#endif

#ifdef PLUGIN_031
  Plugin_id[x] = 31; Plugin_ptr[x++] = &Plugin_031;
#endif

#ifdef PLUGIN_032
  Plugin_id[x] = 32; Plugin_ptr[x++] = &Plugin_032;
#endif

#ifdef PLUGIN_033
  Plugin_id[x] = 33; Plugin_ptr[x++] = &Plugin_033;
#endif

#ifdef PLUGIN_034
  Plugin_id[x] = 34; Plugin_ptr[x++] = &Plugin_034;
#endif

#ifdef PLUGIN_035
  Plugin_id[x] = 35; Plugin_ptr[x++] = &Plugin_035;
#endif

#ifdef PLUGIN_036
  Plugin_id[x] = 36; Plugin_ptr[x++] = &Plugin_036;
#endif

#ifdef PLUGIN_037
  Plugin_id[x] = 37; Plugin_ptr[x++] = &Plugin_037;
#endif

#ifdef PLUGIN_038
  Plugin_id[x] = 38; Plugin_ptr[x++] = &Plugin_038;
#endif

#ifdef PLUGIN_039
  Plugin_id[x] = 39; Plugin_ptr[x++] = &Plugin_039;
#endif

#ifdef PLUGIN_040
  Plugin_id[x] = 40; Plugin_ptr[x++] = &Plugin_040;
#endif

#ifdef PLUGIN_041
  Plugin_id[x] = 41; Plugin_ptr[x++] = &Plugin_041;
#endif

#ifdef PLUGIN_042
  Plugin_id[x] = 42; Plugin_ptr[x++] = &Plugin_042;
#endif

#ifdef PLUGIN_043
  Plugin_id[x] = 43; Plugin_ptr[x++] = &Plugin_043;
#endif

#ifdef PLUGIN_044
  Plugin_id[x] = 44; Plugin_ptr[x++] = &Plugin_044;
#endif

#ifdef PLUGIN_045
  Plugin_id[x] = 45; Plugin_ptr[x++] = &Plugin_045;
#endif

#ifdef PLUGIN_046
  Plugin_id[x] = 46; Plugin_ptr[x++] = &Plugin_046;
#endif

#ifdef PLUGIN_047
  Plugin_id[x] = 47; Plugin_ptr[x++] = &Plugin_047;
#endif

#ifdef PLUGIN_048
  Plugin_id[x] = 48; Plugin_ptr[x++] = &Plugin_048;
#endif

#ifdef PLUGIN_049
  Plugin_id[x] = 49; Plugin_ptr[x++] = &Plugin_049;
#endif

#ifdef PLUGIN_050
  Plugin_id[x] = 50; Plugin_ptr[x++] = &Plugin_050;
#endif

#ifdef PLUGIN_051
  Plugin_id[x] = 51; Plugin_ptr[x++] = &Plugin_051;
#endif

  PluginCall(PLUGIN_DEVICE_ADD, 0, dummyString);
  PluginCall(PLUGIN_INIT_ALL, 0, dummyString);

}


/*********************************************************************************************\
* Function call to all or specific plugins
\*********************************************************************************************/
byte PluginCall(byte Function, struct EventStruct *event, String& str)
{
  int x;
  struct EventStruct TempEvent;

  if (event == 0)
    event = &TempEvent;

  switch (Function)
  {
    // Unconditional calls to all plugins
    case PLUGIN_DEVICE_ADD:
      for (x = 0; x < PLUGIN_MAX; x++)
        if (Plugin_id[x] != 0)
          Plugin_ptr[x](Function, event, str);
      return true;
      break;

    // Call to all plugins. Return at first match
    case PLUGIN_WRITE:
      for (x = 0; x < PLUGIN_MAX; x++)
        if (Plugin_id[x] != 0)
          if (Plugin_ptr[x](Function, event, str))
            return true;
      break;

    // Call to all plugins that are used in a task
    case PLUGIN_ONCE_A_SECOND:
    case PLUGIN_TEN_PER_SECOND:
    case PLUGIN_INIT_ALL:
    case PLUGIN_CLOCK_IN:
    case PLUGIN_EVENT_OUT:
      {
        if (Function == PLUGIN_INIT_ALL)
          Function = PLUGIN_INIT;
        for (byte y = 0; y < TASKS_MAX; y++)
        {
          if (Settings.TaskDeviceNumber[y] != 0)
          {
            if (Settings.TaskDeviceDataFeed[y] == 0) // these calls only to tasks with local feed
            {
              byte DeviceIndex = getDeviceIndex(Settings.TaskDeviceNumber[y]);
              TempEvent.TaskIndex = y;
              TempEvent.BaseVarIndex = y * VARS_PER_TASK;
              //TempEvent.idx = Settings.TaskDeviceID[y];
              TempEvent.sensorType = Device[DeviceIndex].VType;
              TempEvent.OriginTaskIndex = event->TaskIndex;
              for (x = 0; x < PLUGIN_MAX; x++)
              {
                if (Plugin_id[x] == Settings.TaskDeviceNumber[y])
                {
                  Plugin_ptr[x](Function, &TempEvent, str);
                }
              }
            }
          }
        }
        return true;
        break;
      }

    // Call to specific plugin that is used for current task
    case PLUGIN_INIT:
    case PLUGIN_WEBFORM_LOAD:
    case PLUGIN_WEBFORM_SAVE:
    case PLUGIN_WEBFORM_SHOW_VALUES:
    case PLUGIN_WEBFORM_SHOW_CONFIG:
    case PLUGIN_GET_DEVICEVALUENAMES:
    case PLUGIN_READ:
      for (x = 0; x < PLUGIN_MAX; x++)
      {
        if ((Plugin_id[x] != 0 ) && (Plugin_id[x] == Settings.TaskDeviceNumber[event->TaskIndex]))
        {
          event->BaseVarIndex = event->TaskIndex * VARS_PER_TASK;
          return Plugin_ptr[x](Function, event, str);
        }
      }
      return false;
      break;

  }// case
  return false;
}

void create_task(byte index, byte taskdevicenumber, int interval, int pin1, int port, byte destindex)
{
 struct EventStruct TempEvent;
 Settings.TaskDeviceNumber[index - 1] = taskdevicenumber;  
 Settings.TaskDeviceID[index - 1] = destindex;
 TempEvent.TaskIndex = index - 1;
 Settings.TaskDeviceTimer[index - 1] = interval;
 Settings.TaskDeviceSendData[index - 1] = true;
 Settings.TaskDevicePin1[index-1] = pin1;
 Settings.TaskDevicePort[index-1] = port;
 PluginCall(PLUGIN_INIT, &TempEvent, dummyString);
}

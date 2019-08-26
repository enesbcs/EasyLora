//#######################################################################################################
//#################################### Plugin 026: Analog ###############################################
//#######################################################################################################

#ifdef USES_P026

#define PLUGIN_026
#define PLUGIN_ID_026         26
#define PLUGIN_NAME_026       "System Info"
#define PLUGIN_VALUENAME1_026 ""

boolean Plugin_026(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {

    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_026;
        Device[deviceCount].VType = SENSOR_TYPE_SINGLE;
        Device[deviceCount].ValueCount = 1;
        Device[deviceCount].SendDataOption = true;
        Device[deviceCount].TimerOption = true;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_026);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_026));
        break;
      }

    case PLUGIN_INIT:
      {
        Settings.TaskDevicePluginConfig[event->TaskIndex][0] = 0;
        success = true;
        break;
      }
      
    case PLUGIN_READ:
      {
        float value = 0;
        switch(Settings.TaskDevicePluginConfig[event->TaskIndex][0])
        {
          case 0:
          {
            value = (wdcounter /2);
            break;
          }
          case 1:
          {
            value = FreeMem();
            break;
          }
        }
        UserVar[event->BaseVarIndex] = value;
        String log = F("SYS  : ");
        log += value;
        addLog(LOG_LEVEL_INFO,log);
        success = true;
        break;
      }
  }
  return success;
}
#endif

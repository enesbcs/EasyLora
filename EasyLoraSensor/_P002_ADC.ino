//#######################################################################################################
//#################################### Plugin 002: Analog ###############################################
//#######################################################################################################
#ifdef USES_P002
// Adapted from ESP Easy, changes:
//   WebServer.arg() -> WebServer.arg()
//   port selection as we have a lot of analog ports here...

#define PLUGIN_002
#define PLUGIN_ID_002         2
#define PLUGIN_NAME_002       "Analog input"
#define PLUGIN_VALUENAME1_002 "Analog"
boolean Plugin_002(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {

    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_002;
        Device[deviceCount].Type = DEVICE_TYPE_ANALOG;
        Device[deviceCount].VType = SENSOR_TYPE_SINGLE;
        Device[deviceCount].ValueCount = 1;
        Device[deviceCount].SendDataOption = true;
        Device[deviceCount].TimerOption = true;
        Device[deviceCount].GlobalSyncOption = true;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_002);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_002));
        break;
      }

    case PLUGIN_READ:
      {
#if AUTO_DISABLE_ADC
        power_adc_enable();
        ADCSRA |= (1 << ADEN);
#endif
        int value = analogRead(Settings.TaskDevicePort[event->TaskIndex]);
#if AUTO_DISABLE_ADC
        ADCSRA &= ~(1 << ADEN);
        power_adc_disable();
#endif
        UserVar[event->BaseVarIndex] = (float)value;
        String log = F("ADC  : Analog port ");
        log += Settings.TaskDevicePort[event->TaskIndex];
        log += F(" value: ");
        log += value;
        addLog(0, log);
        success = true;
        break;
      }
  }
  return success;
}
#endif

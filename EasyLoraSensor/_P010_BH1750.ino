//#######################################################################################################
//#################################### Plugin-010: LuxRead   ############################################
//#######################################################################################################

#ifdef USES_P010

#define PLUGIN_010
#define PLUGIN_ID_010         10
#define PLUGIN_NAME_010       "Luminosity - BH1750"
#define PLUGIN_VALUENAME1_010 "Lux"

#define BH1750_ADDRESS_1    0x23
#define BH1750_ADDRESS_2    0x5c
boolean Plugin_010_init_1 = false;
boolean Plugin_010_init_2 = false;

boolean Plugin_010(byte function, struct EventStruct *event, String& string)
  {
  boolean success=false;

  switch(function)
  {

    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_010;
        Device[deviceCount].Type = DEVICE_TYPE_I2C;
        Device[deviceCount].VType = SENSOR_TYPE_SINGLE;
        Device[deviceCount].ValueCount = 1;
        Device[deviceCount].SendDataOption = true;
        Device[deviceCount].TimerOption = true;
        Device[deviceCount].GlobalSyncOption = true;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_010);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_010));
        break;
      }

    case PLUGIN_INIT:
      {
        Settings.TaskDevicePluginConfig[event->TaskIndex][0] = 0x23;
        success = true;
        break;
      }

  case PLUGIN_READ:
    {
      uint8_t address = -1;
      boolean *Plugin_010_init;

      if(Settings.TaskDevicePluginConfig[event->TaskIndex][0]==0)
        {
            address = BH1750_ADDRESS_1;
            Plugin_010_init = &Plugin_010_init_1;
        }
      else
        {
            address = BH1750_ADDRESS_2;
            Plugin_010_init = &Plugin_010_init_2;
        }

      if (!*Plugin_010_init)
        {
          *Plugin_010_init = Plugin_010_setResolution(address);
        }

      if (Wire.requestFrom(address, (uint8_t)2) == 2)
        {
          byte b1 = Wire.read();
          byte b2 = Wire.read();
          float val = 0xffff; //pm-cz: Maximum obtainable value
          if (b1 != 0xff || b2 != 0xff) { //pm-cz: Add maximum range check
            val=((b1<<8)|b2)/1.2;
          }
          UserVar[event->BaseVarIndex] = val;
          String log = F("LUX 0x");
          log += String(address,HEX);
          log += F(" : Light intensity: ");
          log += UserVar[event->BaseVarIndex];
          addLog(LOG_LEVEL_INFO,log);
          success=true;
        }
      break;
    }
  }
  return success;
}

boolean Plugin_010_setResolution(uint8_t address){
          Wire.beginTransmission(address);
          Wire.write(0x10);                             // 1 lx resolution
          Wire.endTransmission();
          return true;
}
#endif

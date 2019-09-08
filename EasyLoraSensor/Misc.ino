void hardwareInit()
{
#if DISABLE_ADC
 ADCSRA &= ~(1 << ADEN);
 power_adc_disable(); 
#endif
#if DISABLE_USART
 power_usart0_disable();// Serial (USART)
#endif
#if DISABLE_I2C
 power_twi_disable(); // TWI (I2C)
#else
 #if FEATURE_I2C
  Wire.begin();
 #endif
#endif
}

/********************************************************************************************\
  Reset all settings to factory defaults
  \*********************************************************************************************/
void LoadSettings(void)
{
  memset(Settings.Name,0,sizeof(Settings.Name));
  strcpy(Settings.Name, DEFAULT_NAME);
  for (byte x = 0; x < TASKS_MAX; x++)
  {
    Settings.TaskDeviceNumber[x] = 0;
    Settings.TaskDevicePin1[x] = -1;
    Settings.TaskDevicePin2[x] = -1;
    Settings.TaskDevicePin3[x] = -1;
    Settings.TaskDevicePin1PullUp[x] = false;
    Settings.TaskDevicePin1Inversed[x] = false;
    Settings.TaskDeviceSendData[x] = false;
    Settings.TaskDeviceTimer[x] = DEFAULT_DELAY;
  }
}

void LoadTaskSettings(byte TaskIndex)
{
  String deviceName = "";
  struct EventStruct TempEvent;
  TempEvent.TaskIndex = TaskIndex;
  Plugin_ptr[TaskIndex](PLUGIN_GET_DEVICENAME, 0, deviceName);
  //deviceName.toCharArray(ExtraTaskSettings.TaskDeviceName,25);
  strncpy(ExtraTaskSettings.TaskDeviceName, deviceName.c_str(), sizeof(ExtraTaskSettings.TaskDeviceName));
  PluginCall(PLUGIN_GET_DEVICEVALUENAMES, &TempEvent, dummyString);    
  ExtraTaskSettings.TaskIndex = TaskIndex; // Needed when an empty task was requested
}

void addLog(byte loglevel, String & tlog) 
{
  #if !DISABLE_USART
  Serial.println(tlog);
  #endif
}

/********************************************************************************************\
  Get free system mem
  \*********************************************************************************************/
#if defined(__arm__)
extern "C" char* sbrk(int incr);
unsigned long FreeMem(void) {
  char top = 't';
  return &top - reinterpret_cast<char*>(sbrk(0));
}
#else
uint8_t *heapptr, *stackptr;

unsigned long FreeMem(void)
{
  stackptr = (uint8_t *)malloc(4);        // use stackptr temporarily
  heapptr = stackptr;                     // save value of heap pointer
  free(stackptr);                         // free up the memory again (sets stackptr to 0)
  stackptr =  (uint8_t *)(SP);            // save value of stack pointer
  return (stackptr - heapptr);
}
#endif

/*********************************************************************************************\
   Get value count from sensor type
  \*********************************************************************************************/

byte getValueCountFromSensorType(byte sensorType)
{
  byte valueCount = 0;

  switch (sensorType)
  {
    case SENSOR_TYPE_SINGLE:                      // single value sensor, used for Dallas, BH1750, etc
    case SENSOR_TYPE_SWITCH:
    case SENSOR_TYPE_DIMMER:
      valueCount = 1;
      break;
    case SENSOR_TYPE_LONG:                      // single LONG value, stored in two floats (rfid tags)
      valueCount = 2;
      break;
    case SENSOR_TYPE_TEMP_HUM:
    case SENSOR_TYPE_TEMP_BARO:
    case SENSOR_TYPE_DUAL:
      valueCount = 2;
      break;
    case SENSOR_TYPE_TEMP_HUM_BARO:
    case SENSOR_TYPE_TRIPLE:
      valueCount = 3;
      break;
    case SENSOR_TYPE_QUAD:
      valueCount = 4;
      break;
  }
  return valueCount;
}


/*********************************************************************************************\
   Workaround for removing trailing white space when String() converts a float with 0 decimals
  \*********************************************************************************************/
String toString(float value, byte decimals)
{
  String sValue = String(value, decimals);
  sValue.trim();
  return sValue;
}


/********************************************************************************************\
  Unsigned long Timer timeOut check
  \*********************************************************************************************/

boolean timeOut(unsigned long timer)
{
  // This routine solves the 49 day bug without the need for separate start time and duration
  //   that would need two 32 bit variables if duration is not static
  // It limits the maximum delay to 24.9 days.

  unsigned long now = millis();
  if (((now >= timer) && ((now - timer) < 1 << 31))  || ((timer >= now) && (timer - now > 1 << 31)))
    return true;

  return false;
}


/********************************************************************************************\
  delay in milliseconds with background processing
  \*********************************************************************************************/
void delayMillis(unsigned long delay)
{
  unsigned long timer = millis() + delay;
  while (millis() < timer)
    backgroundtasks();
}


/********************************************************************************************\
  Clear task settings for given task
  \*********************************************************************************************/
void taskClear(byte taskIndex, boolean save)
{
  Settings.TaskDeviceNumber[taskIndex] = 0;
  ExtraTaskSettings.TaskDeviceName[0] = 0;
  Settings.TaskDeviceID[taskIndex] = 0;
  Settings.TaskDeviceDataFeed[taskIndex] = 0;
  Settings.TaskDevicePin1[taskIndex] = -1;
  Settings.TaskDevicePin2[taskIndex] = -1;
  Settings.TaskDevicePin3[taskIndex] = -1;
  Settings.TaskDevicePort[taskIndex] = 0;
  Settings.TaskDeviceSendData[taskIndex] = true;
  Settings.TaskDeviceGlobalSync[taskIndex] = false;
  Settings.TaskDeviceTimer[taskIndex] = 0;

  for (byte x = 0; x < PLUGIN_CONFIGVAR_MAX; x++)
    Settings.TaskDevicePluginConfig[taskIndex][x] = 0;

  for (byte varNr = 0; varNr < VARS_PER_TASK; varNr++)
  {
    ExtraTaskSettings.TaskDeviceValueNames[varNr][0] = 0;
  }

}

/********************************************************************************************\
  Find device index corresponding to task number setting
  \*********************************************************************************************/
byte getDeviceIndex(byte Number)
{
  byte DeviceIndex = 0;
  for (byte x = 0; x <= deviceCount ; x++)
    if (Device[x].Number == Number)
      DeviceIndex = x;
  return DeviceIndex;
}


/********************************************************************************************\
  Convert a char string to integer
  \*********************************************************************************************/
unsigned long str2int(char *string)
{
  unsigned long temp = atof(string);
  return temp;
}


/********************************************************************************************\
  In memory convert float to long
  \*********************************************************************************************/
unsigned long float2ul(float f)
{
  unsigned long ul;
  memcpy(&ul, &f, 4);
  return ul;
}


/********************************************************************************************\
  In memory convert long to float
  \*********************************************************************************************/
float ul2float(unsigned long ul)
{
  float f;
  memcpy(&f, &ul, 4);
  return f;
}

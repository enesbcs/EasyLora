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

/*********************************************************************************************\
   Parse a string and get the xth command or parameter
  \*********************************************************************************************/
String parseString(String& string, byte indexFind)
{
  String tmpString = string;
  tmpString += ",";
  tmpString.replace(" ", ",");
  String locateString = "";
  byte count = 0;
  int index = tmpString.indexOf(',');
  while (index > 0)
  {
    count++;
    locateString = tmpString.substring(0, index);
    tmpString = tmpString.substring(index + 1);
    index = tmpString.indexOf(',');
    if (count == indexFind)
    {
      locateString.toLowerCase();
      return locateString;
    }
  }
  return "";
}

/********************************************************************************************\
  Parse a command string to event struct
  \*********************************************************************************************/
void parseCommandString(struct EventStruct *event, String& string)
{
  char command[80];
  command[0] = 0;
  char TmpStr1[80];
  TmpStr1[0] = 0;

  string.toCharArray(command, 80);
  event->Par1 = 0;
  event->Par2 = 0;
  event->Par3 = 0;

  if (GetArgv(command, TmpStr1, 2)) event->Par1 = str2int(TmpStr1);
  if (GetArgv(command, TmpStr1, 3)) event->Par2 = str2int(TmpStr1);
  if (GetArgv(command, TmpStr1, 4)) event->Par3 = str2int(TmpStr1);
}

/*********************************************************************************************\
   set pin mode & state (info table)
  \*********************************************************************************************/
boolean setPinState(byte plugin, byte index, byte mode, uint16_t value)
{
  // plugin number and index form a unique key
  // first check if this pin is already known
  boolean reUse = false;
  for (byte x = 0; x < PINSTATE_TABLE_MAX; x++)
    if ((pinStates[x].plugin == plugin) && (pinStates[x].index == index))
    {
      pinStates[x].mode = mode;
      pinStates[x].value = value;
      reUse = true;
      break;
    }

  if (!reUse)
  {
    for (byte x = 0; x < PINSTATE_TABLE_MAX; x++)
      if (pinStates[x].plugin == 0)
      {
        pinStates[x].plugin = plugin;
        pinStates[x].index = index;
        pinStates[x].mode = mode;
        pinStates[x].value = value;
        break;
      }
  }
}
/*********************************************************************************************\
   get pin mode & state (info table)
  \*********************************************************************************************/
boolean getPinState(byte plugin, byte index, byte *mode, uint16_t *value)
{
  for (byte x = 0; x < PINSTATE_TABLE_MAX; x++)
    if ((pinStates[x].plugin == plugin) && (pinStates[x].index == index))
    {
      *mode = pinStates[x].mode;
      *value = pinStates[x].value;
      return true;
    }
  return false;
}

/********************************************************************************************\
  Find positional parameter in a char string
  \*********************************************************************************************/
boolean GetArgv(const char *string, char *argv, int argc)
{
  int string_pos = 0, argv_pos = 0, argc_pos = 0;
  char c, d;

  while (string_pos < strlen(string))
  {
    c = string[string_pos];
    d = string[string_pos + 1];

    if       (c == ' ' && d == ' ') {}
    else if  (c == ' ' && d == ',') {}
    else if  (c == ',' && d == ' ') {}
    else if  (c == ' ' && d >= 33 && d <= 126) {}
    else if  (c == ',' && d >= 33 && d <= 126) {}
    else
    {
      argv[argv_pos++] = c;
      argv[argv_pos] = 0;

      if (d == ' ' || d == ',' || d == 0)
      {
        argv[argv_pos] = 0;
        argc_pos++;

        if (argc_pos == argc)
        {
          return true;
        }

        argv[0] = 0;
        argv_pos = 0;
        string_pos++;
      }
    }
    string_pos++;
  }
  return false;
}

/********************************************************************************************\
  Calculate function for simple expressions
 \*********************************************************************************************/
#define CALCULATE_OK                            0
#define CALCULATE_ERROR_STACK_OVERFLOW          1
#define CALCULATE_ERROR_BAD_OPERATOR            2
#define CALCULATE_ERROR_PARENTHESES_MISMATCHED  3
#define CALCULATE_ERROR_UNKNOWN_TOKEN           4
#define STACK_SIZE 10 // was 50
#define TOKEN_MAX 20

float globalstack[STACK_SIZE];
float *sp = globalstack - 1;
float *sp_max = &globalstack[STACK_SIZE - 1];

#define is_operator(c)  (c == '+' || c == '-' || c == '*' || c == '/' || c == '^')

int push(float value)
{
  if (sp != sp_max) // Full
  {
    *(++sp) = value;
    return 0;
  }
  else
    return CALCULATE_ERROR_STACK_OVERFLOW;
}

float pop()
{
  if (sp != (globalstack - 1)) // empty
    return *(sp--);
}

float apply_operator(char op, float first, float second)
{
  switch (op)
  {
    case '+':
      return first + second;
    case '-':
      return first - second;
    case '*':
      return first * second;
    case '/':
      return first / second;
    case '^':
      return pow(first, second);
    default:
      return 0;
  }
}

char *next_token(char *linep)
{
  while (isspace(*(linep++)));
  while (*linep && !isspace(*(linep++)));
  return linep;
}

int RPNCalculate(char* token)
{
  if (token[0] == 0)
    return 0; // geen moeite doen voor een lege string

  if (is_operator(token[0]))
  {
    float second = pop();
    float first = pop();

    if (push(apply_operator(token[0], first, second)))
      return CALCULATE_ERROR_STACK_OVERFLOW;
  }
  else // Als er nog een is, dan deze ophalen
    if (push(atof(token))) // is het een waarde, dan op de stack plaatsen
      return CALCULATE_ERROR_STACK_OVERFLOW;

  return 0;
}

// operators
// precedence   operators         associativity
// 3            !                 right to left
// 2            * / %             left to right
// 1            + - ^             left to right
int op_preced(const char c)
{
  switch (c)
  {
    case '^':
      return 3;
    case '*':
    case '/':
      return 2;
    case '+':
    case '-':
      return 1;
  }
  return 0;
}

bool op_left_assoc(const char c)
{
  switch (c)
  {
    case '^':
    case '*':
    case '/':
    case '+':
    case '-':
      return true;     // left to right
      //case '!': return false;    // right to left
  }
  return false;
}

unsigned int op_arg_count(const char c)
{
  switch (c)
  {
    case '^':
    case '*':
    case '/':
    case '+':
    case '-':
      return 2;
      //case '!': return 1;
  }
  return 0;
}


int Calculate(const char *input, float * result)
{
  const char *strpos = input, *strend = input + strlen(input);
  char token[25];
  char c, *TokenPos = token;
  char stack[32];       // operator stack
  unsigned int sl = 0;  // stack length
  char     sc;          // used for record stack element
  int error = 0;

  //*sp=0; // bug, it stops calculating after 50 times
  sp = globalstack - 1;

  while (strpos < strend)
  {
    // read one token from the input stream
    c = *strpos;
    if (c != ' ')
    {
      // If the token is a number (identifier), then add it to the token queue.
      if ((c >= '0' && c <= '9') || c == '.')
      {
        *TokenPos = c;
        ++TokenPos;
      }

      // If the token is an operator, op1, then:
      else if (is_operator(c))
      {
        *(TokenPos) = 0;
        error = RPNCalculate(token);
        TokenPos = token;
        if (error)return error;
        while (sl > 0)
        {
          sc = stack[sl - 1];
          // While there is an operator token, op2, at the top of the stack
          // op1 is left-associative and its precedence is less than or equal to that of op2,
          // or op1 has precedence less than that of op2,
          // The differing operator priority decides pop / push
          // If 2 operators have equal priority then associativity decides.
          if (is_operator(sc) && ((op_left_assoc(c) && (op_preced(c) <= op_preced(sc))) || (op_preced(c) < op_preced(sc))))
          {
            // Pop op2 off the stack, onto the token queue;
            *TokenPos = sc;
            ++TokenPos;
            *(TokenPos) = 0;
            error = RPNCalculate(token);
            TokenPos = token;
            if (error)return error;
            sl--;
          }
          else
            break;
        }
        // push op1 onto the stack.
        stack[sl] = c;
        ++sl;
      }
      // If the token is a left parenthesis, then push it onto the stack.
      else if (c == '(')
      {
        stack[sl] = c;
        ++sl;
      }
      // If the token is a right parenthesis:
      else if (c == ')')
      {
        bool pe = false;
        // Until the token at the top of the stack is a left parenthesis,
        // pop operators off the stack onto the token queue
        while (sl > 0)
        {
          *(TokenPos) = 0;
          error = RPNCalculate(token);
          TokenPos = token;
          if (error)return error;
          sc = stack[sl - 1];
          if (sc == '(')
          {
            pe = true;
            break;
          }
          else
          {
            *TokenPos = sc;
            ++TokenPos;
            sl--;
          }
        }
        // If the stack runs out without finding a left parenthesis, then there are mismatched parentheses.
        if (!pe)
          return CALCULATE_ERROR_PARENTHESES_MISMATCHED;

        // Pop the left parenthesis from the stack, but not onto the token queue.
        sl--;

        // If the token at the top of the stack is a function token, pop it onto the token queue.
        if (sl > 0)
          sc = stack[sl - 1];

      }
      else
        return CALCULATE_ERROR_UNKNOWN_TOKEN;
    }
    ++strpos;
  }
  // When there are no more tokens to read:
  // While there are still operator tokens in the stack:
  while (sl > 0)
  {
    sc = stack[sl - 1];
    if (sc == '(' || sc == ')')
      return CALCULATE_ERROR_PARENTHESES_MISMATCHED;

    *(TokenPos) = 0;
    error = RPNCalculate(token);
    TokenPos = token;
    if (error)return error;
    *TokenPos = sc;
    ++TokenPos;
    --sl;
  }

  *(TokenPos) = 0;
  error = RPNCalculate(token);
  TokenPos = token;
  if (error)
  {
    *result = 0;
    return error;
  }
  *result = *sp;
  return CALCULATE_OK;
}

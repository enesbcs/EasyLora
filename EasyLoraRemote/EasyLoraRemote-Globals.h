#ifndef EASYLORASENSOR_GLOBALS_H_
#define EASYLORASENSOR_GLOBALS_H_

#ifdef ARDUINO_SAMD_MKRWAN1300
#error "This is not compatible with the Arduino MKR WAN 1300 board!"
#endif

// ----------------------------------------------------------
// Set default configuration settings if you want (mandatory)
// ----------------------------------------------------------

/*EU duty cycle
    g (863.0 – 868.0 MHz)  : 1%   DUTY_ONE
    g1 (868.0 – 868.6 MHz) : 1%   DUTY_ONE
    g2 (868.7 – 869.2 MHz) : 0.1% DUTY_ZEROPOINTONE
    g3 (869.4 – 869.65 MHz): 10%  DUTY_TEN
    g4 (869.7 – 870.0 MHz) : 1%   DUTY_ONE
*/
#define DUTY_ZEROPOINTONE    1000    // 0.1%
#define DUTY_ONE              100    // 1%
#define DUTY_TEN               10    // 10%

#define LORA_FREQ          8695E5    // 869.5Mhz is g3 in EU
#define LORA_DUTY        DUTY_TEN    // Duty cycle is 10% for g3 frequencies

#define DEFAULT_NAME   "lora32u4-rec"    // Enter your device friendly name
#define UNIT                    3    // Unitnumber, set to non-zero (0:multicast, 1:gateway,2-254:clients,255:reserved)
#define DEFAULT_DELAY          60    // Enter your Send delay in seconds

#define LORA_DESTINATION_UNIT   1    // Unitnumber, set to non-zero (0:multicast, 1:gateway,2-254:clients,255:reserved)

#define LORA_SS                 8
#define LORA_RESET              4
#define LORA_DIO0               7
#define LORA_SF                 9    // 6-12              Spreading factor
#define LORA_BW             250E3    // 7800 - 500000     Bandwidth
#define LORA_CODE               5    // 5-8               Coding rate
#define LORA_SYNC            0x12    // SYNC WORD default setting of 0x12, LoRaWAN setting is 0x34, change to unique to isolate
#define LEDPIN                 13    // change -1 if not used, or the LED pin GPIO

#define USES_P001   // Switch  -- TaskDevicePin1

// SLEEP OPTIONS
#define DISABLE_ADC             true  // disable ADC at startup
#define DISABLE_USART           false // disable serial
#define DISABLE_I2C             true  // disable I2C
// END OF SLEEP OPTIONS
#define WAIT_SERIAL_AT_STARTUP  false  // set to false to let the sketch to run independent!! set to true if you want to see serial comm beginning

#define DEVICES_MAX                3
#define TASKS_MAX                  2
#define VARS_PER_TASK              4

byte mac[] = { 0x10, 0x00, 0x00, 1, 1, 0 }; // Make it unique manually, ATMEGA32U4 has no Unique ID

// -------------------------------------------------------------------------------
// DO NOT CHANGE BELOW THIS LINE - UNLESS YOU REALLY UNDERSTAND AND LOOK AFTER IT!
// -------------------------------------------------------------------------------

#if DISABLE_ADC || DISABLE_USART || DISABLE_I2C || AUTO_DISABLE_ADC
 #include <avr/power.h>
#endif

#if DEFAULT_DELAY<LORA_DUTY
 #define DEFAULT_DELAY              LORA_DUTY
#endif

#define VERSION                             1
#define BUILD                             156

#define PLUGIN_MAX                DEVICES_MAX // ESP Easy 64
#define PLUGIN_CONFIGVAR_MAX                4
#define PLUGIN_CONFIGFLOATVAR_MAX           1
#define PLUGIN_CONFIGLONGVAR_MAX            1
#define PLUGIN_EXTRACONFIGVAR_MAX           1
#define UNIT_MAX                          254
#define RULES_TIMER_MAX                     4
#define SYSTEM_TIMER_MAX                    4 // ESP Easy  8
#define SYSTEM_CMD_TIMER_MAX                2 // ESP Easy  2

#define NODE_TYPE_ID_ESP_EASY_STD           1
#define NODE_TYPE_ID_RPI_EASY_STD           5
#define NODE_TYPE_ID_ESP_EASYM_STD         17
#define NODE_TYPE_ID_ESP_EASY32_STD        33
#define NODE_TYPE_ID_ARDUINO_EASY_STD      65
#define NODE_TYPE_ID_NANO_EASY_STD         81
#define NODE_TYPE_ID_ATMEGA_EASY_LORA      97
#define NODE_TYPE_ID                       NODE_TYPE_ID_ATMEGA_EASY_LORA

#define PINSTATE_TABLE_MAX                 16 // ESP Easy 32

#define PIN_MODE_UNDEFINED                  0
#define PIN_MODE_INPUT                      1
#define PIN_MODE_OUTPUT                     2
#define PIN_MODE_PWM                        3
#define PIN_MODE_SERVO                      4

#define LOG_LEVEL_ERROR                     1
#define LOG_LEVEL_INFO                      2
#define LOG_LEVEL_DEBUG                     3
#define LOG_LEVEL_DEBUG_MORE                4

#define DEVICE_TYPE_SINGLE                  1  // connected through 1 datapin
#define DEVICE_TYPE_DUAL                    2  // connected through 2 datapins
#define DEVICE_TYPE_TRIPLE                  3  // connected through 3 datapins
#define DEVICE_TYPE_ANALOG                 10  // AIN/tout pin
#define DEVICE_TYPE_I2C                    20  // connected through I2C
#define DEVICE_TYPE_DUMMY                  99 // Dummy device, has no physical connection

#define SENSOR_TYPE_NONE                    0
#define SENSOR_TYPE_SINGLE                  1
#define SENSOR_TYPE_TEMP_HUM                2
#define SENSOR_TYPE_TEMP_BARO               3
#define SENSOR_TYPE_TEMP_HUM_BARO           4
#define SENSOR_TYPE_DUAL                    5
#define SENSOR_TYPE_TRIPLE                  6
#define SENSOR_TYPE_QUAD                    7
#define SENSOR_TYPE_TEMP_EMPTY_BARO         8
#define SENSOR_TYPE_SWITCH                 10
#define SENSOR_TYPE_DIMMER                 11
#define SENSOR_TYPE_LONG                   20
#define SENSOR_TYPE_WIND                   21

#define PLUGIN_INIT_ALL                     1
#define PLUGIN_INIT                         2
#define PLUGIN_READ                         3
#define PLUGIN_ONCE_A_SECOND                4
#define PLUGIN_TEN_PER_SECOND               5
#define PLUGIN_DEVICE_ADD                   6
#define PLUGIN_EVENTLIST_ADD                7
#define PLUGIN_WEBFORM_SAVE                 8
#define PLUGIN_WEBFORM_LOAD                 9
#define PLUGIN_WEBFORM_SHOW_VALUES         10
#define PLUGIN_GET_DEVICENAME              11
#define PLUGIN_GET_DEVICEVALUENAMES        12
#define PLUGIN_WRITE                       13
#define PLUGIN_EVENT_OUT                   14
#define PLUGIN_WEBFORM_SHOW_CONFIG         15
#define PLUGIN_SERIAL_IN                   16
#define PLUGIN_UDP_IN                      17
#define PLUGIN_CLOCK_IN                    18
#define PLUGIN_TIMER_IN                    19

#define VALUE_SOURCE_SYSTEM                 1
#define VALUE_SOURCE_SERIAL                 2
#define VALUE_SOURCE_HTTP                   3
#define VALUE_SOURCE_MQTT                   4
#define VALUE_SOURCE_UDP                    5

#define INT_MIN -32767
#define INT_MAX 32767

#if FEATURE_I2C
  #include <Wire.h>
#endif

struct SettingsStruct
{
  char          Name[26];
  byte          TaskDeviceNumber[TASKS_MAX];
  unsigned int  TaskDeviceID[TASKS_MAX];
  int8_t        TaskDevicePin1[TASKS_MAX];
  int8_t        TaskDevicePin2[TASKS_MAX];
  byte          TaskDevicePort[TASKS_MAX];
  boolean       TaskDevicePin1PullUp[TASKS_MAX];
  int16_t       TaskDevicePluginConfig[TASKS_MAX][PLUGIN_CONFIGVAR_MAX];  
  boolean       TaskDevicePin1Inversed[TASKS_MAX];
  boolean       TaskDeviceSendData[TASKS_MAX];
  boolean       TaskDeviceGlobalSync[TASKS_MAX];
  int8_t        TaskDevicePin3[TASKS_MAX];
  byte          TaskDeviceDataFeed[TASKS_MAX];
  unsigned long TaskDeviceTimer[TASKS_MAX];
} Settings;

struct ExtraTaskSettingsStruct
{
  byte    TaskIndex;
  char    TaskDeviceName[25];
  char    TaskDeviceValueNames[VARS_PER_TASK][25];
} ExtraTaskSettings;

struct DeviceStruct
{
  byte Number;
  byte Type;
  byte VType;
  boolean PullUpOption;
  boolean InverseLogicOption;
  byte ValueCount;
  boolean SendDataOption;
  boolean GlobalSyncOption;
  boolean TimerOption;
  boolean TimerOptional;
} Device[DEVICES_MAX + 1]; // 1 more because first device is empty device

struct pinStatesStruct
{
  byte plugin;
  byte index;
  byte mode;
  uint16_t value;
} pinStates[PINSTATE_TABLE_MAX];

struct systemTimerStruct
{
  unsigned long timer;
  byte plugin;
  byte Par1;
  byte Par2;
  byte Par3;
} systemTimers[SYSTEM_TIMER_MAX];

struct systemCMDTimerStruct
{
  unsigned long timer;
  String action;
} systemCMDTimers[SYSTEM_CMD_TIMER_MAX];

struct EventStruct
{
  byte Source;
  byte TaskIndex;
  byte BaseVarIndex;
  int idx;
  byte sensorType;
  int Par1;
  int Par2;
  int Par3;
  byte OriginTaskIndex;
  String String1;
  String String2;
  byte *Data;
};
#endif

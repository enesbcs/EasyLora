//#######################################################################################################
//#################################### Plugin 001: Input Switch #########################################
//#######################################################################################################
#ifdef USES_P001
// Adapted from ESP Easy, changes:
//   WebServer.arg() -> WebServer.arg()
//   Changed pin limit from 0-16 to 2-13
#define PLUGIN_001
#define PLUGIN_ID_001         1
#define PLUGIN_NAME_001       "Switch input"
#define PLUGIN_VALUENAME1_001 "Switch"

boolean Plugin_001(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;
  static byte switchstate[TASKS_MAX];
  static byte outputstate[TASKS_MAX];

  switch (function)
  {
    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_001;
        Device[deviceCount].Type = DEVICE_TYPE_SINGLE;
        Device[deviceCount].VType = SENSOR_TYPE_SWITCH;
        Device[deviceCount].ValueCount = 1;
        Device[deviceCount].SendDataOption = true;
        Device[deviceCount].TimerOption = true;
        Device[deviceCount].TimerOptional = true;
        Device[deviceCount].GlobalSyncOption = true;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_001);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_001));
        break;
      }

    case PLUGIN_INIT:
      {
        Settings.TaskDevicePluginConfig[event->TaskIndex][0] = 0;
        Settings.TaskDevicePluginConfig[event->TaskIndex][2] = 0;
        Settings.TaskDevicePluginConfig[event->TaskIndex][3] = 0;

        if (Settings.TaskDevicePin1PullUp[event->TaskIndex])
          pinMode(Settings.TaskDevicePin1[event->TaskIndex], INPUT_PULLUP);
        else
          pinMode(Settings.TaskDevicePin1[event->TaskIndex], INPUT);

        switchstate[event->TaskIndex] = digitalRead(Settings.TaskDevicePin1[event->TaskIndex]);
        outputstate[event->TaskIndex] = switchstate[event->TaskIndex];

        // if boot state must be send, inverse default state
        if (Settings.TaskDevicePluginConfig[event->TaskIndex][3])
        {
          switchstate[event->TaskIndex] = !switchstate[event->TaskIndex];
          outputstate[event->TaskIndex] = !outputstate[event->TaskIndex];
        }
        success = true;
        break;
      }

    case PLUGIN_TEN_PER_SECOND:
      {
        byte state = digitalRead(Settings.TaskDevicePin1[event->TaskIndex]);
        if (state != switchstate[event->TaskIndex])
        {
          switchstate[event->TaskIndex] = state;
          byte currentOutputState = outputstate[event->TaskIndex];

          if (Settings.TaskDevicePluginConfig[event->TaskIndex][2] == 0) //normal switch
            outputstate[event->TaskIndex] = state;
          else
          {
            if (Settings.TaskDevicePluginConfig[event->TaskIndex][2] == 1)  // active low push button
            {
              if (state == 0)
                outputstate[event->TaskIndex] = !outputstate[event->TaskIndex];
            }
            else  // active high push button
            {
              if (state == 1)
                outputstate[event->TaskIndex] = !outputstate[event->TaskIndex];
            }
          }

          // send if output needs to be changed
          if (currentOutputState != outputstate[event->TaskIndex])
          {
            byte sendState = outputstate[event->TaskIndex];
            if (Settings.TaskDevicePin1Inversed[event->TaskIndex])
              sendState = !outputstate[event->TaskIndex];
            UserVar[event->BaseVarIndex] = sendState;
            event->sensorType = SENSOR_TYPE_SWITCH;
            if ((sendState == 1) && (Settings.TaskDevicePluginConfig[event->TaskIndex][0] == 2))
            {
              event->sensorType = SENSOR_TYPE_DIMMER;
              UserVar[event->BaseVarIndex] = Settings.TaskDevicePluginConfig[event->TaskIndex][1];
            }
            String log = F("SW   : State ");
            log += sendState;
            addLog(LOG_LEVEL_INFO, log);
            sendData(event);
          }
        }
        success = true;
        break;
      }

    case PLUGIN_READ:
      {
        // We do not actually read the pin state as this is already done 10x/second
        // Instead we just send the last known state stored in Uservar
        String log = F("SW   : State ");
        log += UserVar[event->BaseVarIndex];
        addLog(LOG_LEVEL_INFO, log);
        success = true;
        break;
      }

    case PLUGIN_WRITE:
      {
/*        String log = "";
        String command = parseString(string, 1);

        if (command == F("gpio"))
        {
          success = true;
          if (Plugin_001_updatable_pin(event->Par1))
          {
            pinMode(event->Par1, OUTPUT);
            digitalWrite(event->Par1, event->Par2);
            log = String(F("SW   : GPIO ")) + String(event->Par1) + String(F(" Set to ")) + String(event->Par2);
            addLog(LOG_LEVEL_INFO, log);
          }
        }

        if (command == F("pwm"))
        {
          success = true;
          if (Plugin_001_updatable_pin(event->Par1))
          {
            pinMode(event->Par1, OUTPUT);

            if(event->Par3 != 0)
            {
              byte prev_mode;
              uint16_t prev_value;
              getPinState(PLUGIN_ID_001, event->Par1, &prev_mode, &prev_value);
              if(prev_mode != PIN_MODE_PWM)
                prev_value = 0;

              int32_t step_value = ((event->Par2 - prev_value) << 12) / event->Par3;
              int32_t curr_value = prev_value << 12;
              int16_t new_value;
              int i = event->Par3;
              while(i--){
                curr_value += step_value;
                new_value = (uint16_t)(curr_value >> 12);
                analogWrite(event->Par1, new_value);
                delay(1);
              }
            }

            analogWrite(event->Par1, event->Par2);
            log = String(F("SW   : GPIO ")) + String(event->Par1) + String(F(" Set PWM to ")) + String(event->Par2);
            addLog(LOG_LEVEL_INFO, log);
          }
        }

        if (command == F("pulse"))
        {
          success = true;
          if (Plugin_001_updatable_pin(event->Par1))
          {
            pinMode(event->Par1, OUTPUT);
            digitalWrite(event->Par1, event->Par2);
            delay(event->Par3);
            digitalWrite(event->Par1, !event->Par2);
            log = String(F("SW   : GPIO ")) + String(event->Par1) + String(F(" Pulsed for ")) + String(event->Par3) + String(F(" mS"));
            addLog(LOG_LEVEL_INFO, log);
          }
        }

        if (command == F("longpulse"))
        {
          success = true;
          if (Plugin_001_updatable_pin(event->Par1))
          {
            pinMode(event->Par1, OUTPUT);
            digitalWrite(event->Par1, event->Par2);
            setSystemTimer(event->Par3 * 1000, PLUGIN_ID_001, event->Par1, !event->Par2, 0);
            log = String(F("SW   : GPIO ")) + String(event->Par1) + String(F(" Pulse set for ")) + String(event->Par3) + String(F(" S"));
            addLog(LOG_LEVEL_INFO, log);
            SendStatus(event->Source, getPinStateJSON(SEARCH_PIN_STATE, PLUGIN_ID_001, event->Par1, log, 0));
          }
        }

        if (command == F("inputswitchstate"))
        {
          success = true;
          UserVar[event->Par1 * VARS_PER_TASK] = event->Par2;
          outputstate[event->Par1] = event->Par2;
        }*/
        break;
      }

    case PLUGIN_TIMER_IN:
      {
        digitalWrite(event->Par1, event->Par2);
        break;
      }
  }
  return success;
}

/*boolean Plugin_001_updatable_pin(int pin) {
  return pin == 3 || ( pin >= 5 && pin <= 9) || ( pin >= 14 && pin <= 49) || ( pin >= 56 && pin <= 69);
}*/

#endif

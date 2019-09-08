#include "Misc.h"
#include "LoraDirect.h"
#define strcasecmp_P(s1, s2) strcasecmp((s1), (s2))
#include <avr/pgmspace.h>

#define INPUT_COMMAND_SIZE          80
void ExecuteCommand(byte source, const char *Line)
{
  String status = "";
  boolean success = false;
  char TmpStr1[80];
  TmpStr1[0] = 0;
  char Command[80];
  Command[0] = 0;
  int Par1 = 0;
  int Par2 = 0;
  int Par3 = 0;
  GetArgv(Line, Command, 1);
  if (Command==""){
    strcpy(Command,Line);
  }
  
/*  Serial.print("ExecuteCommand"); // DEBUG
  Serial.println(Line);
  Serial.print(Command);
  Serial.println(":cmd");  
  Serial.println(strcasecmp_P(Command, PSTR("Settings")));
  Serial.println(strcasecmp(Command, "Settings"));*/
  
  if (GetArgv(Line, TmpStr1, 2)) Par1 = str2int(TmpStr1);
  if (GetArgv(Line, TmpStr1, 3)) Par2 = str2int(TmpStr1);
  if (GetArgv(Line, TmpStr1, 4)) Par3 = str2int(TmpStr1);

  // ****************************************
  // commands for debugging
  // ****************************************
  if (strcasecmp(Command, "TaskClear") == 0)
  {
    success = true;
    taskClear(Par1 - 1, true);
  }

  // ****************************************
  // commands for rules
  // ****************************************

  if (strcasecmp(Command, "TaskValueSet") == 0)
  {
    success = true;
    if (GetArgv(Line, TmpStr1, 4))
    {
      float result = 0;
      byte error = Calculate(TmpStr1, &result);
      UserVar[(VARS_PER_TASK * (Par1 - 1)) + Par2 - 1] = result;
    }
  }

  if (strcasecmp(Command, "TaskRun") == 0)
  {
    success = true;
    SensorSendTask(Par1 -1);
  }

  if (strcasecmp(Command, "Delay") == 0)
  {
    success = true;
    delayMillis(Par1);
  }

  if (strcasecmp(Command, "Event") == 0)
  {
    success = true;
    String event = Line;
    event = event.substring(6);
    event.replace("$", "#");
    
  }
  // ****************************************
  // configure settings commands
  // ****************************************

  if ((strcasecmp(Command, "Reboot") == 0) || (strcasecmp(Command, "Restart") == 0))
  {
    success = true;
    Reboot();
  }

  if (strcasecmp(Command,"Load") == 0)
  {
    success = true;
    LoadSettings();
  }

  if (strcasecmp(Command, "Settings") == 0)
  {
    success = true;
//    Serial.println();
    Serial.println(F("System Info"));
    Serial.print(F("  Build         : ")); Serial.println((int)BUILD);
    Serial.print(F("  Unit          : ")); Serial.println((byte)UNIT);
    Serial.print(F("  Free mem      : ")); Serial.println(FreeMem());
    sysinfosent = false;
  }
/*
  if (strcasecmp(Command, "loracmdtest") == 0)
  {
    success = true;
    sendCMD("taskvalueset,3,1,11");
  }
*/
  if (success)
    status += F("\nOk");
  else  
    status += F("\nUnknown command!");
  Serial.println(status);
}

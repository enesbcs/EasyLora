#ifndef LORADIRECT_H
#define LORADIRECT_H
void initLora();
void(*Reboot)(void) = 0;
boolean sendSysInfo();
boolean sendData(struct EventStruct *event);
#endif

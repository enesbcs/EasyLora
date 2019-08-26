#ifndef LORADIRECT_H
#define LORADIRECT_H
void initLora();
boolean sendSysInfo();
boolean sendData(struct EventStruct *event);
#endif

#ifndef PLUGIN_H
#define PLUGIN_H
void PluginInit(void);
byte PluginCall(byte Function, struct EventStruct *event, String& str);
void create_task(byte index, byte taskdevicenumber, int interval, int pin1, int port, byte destindex);
#endif

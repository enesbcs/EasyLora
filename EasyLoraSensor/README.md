# EasyLoraSensor sample sketches
Experimental LoRa sensor sketch for BSFrance LoRa32u4 hardware

Sketch is based on a heavily stripped down ArduinoEasy codebase, sketch size is ~25kb, memory usage ~1.2kb.
Purpose: LORA Sender that sends sensor readings (RECEIVING IS NOT IMPLEMENTED!)

https://bsfrance.fr/lora-long-range/1345-LoRa32u4-II-Lora-LiPo-Atmega32u4-SX1276-HPD13-868MHZ-EU-Antenna.html

# Setup
Edit EasyLoraSensor-Globals.h and change settings before compile!

1. Change own UNIT (node) number, and LORA_DESTINATION_UNIT which will receive data
2. Adjust LORA pinout settings: LORA_SS, LORA_RESET, LORA_DIO0 to match to your board!
3. Change LORA_FREQ, and adjust LORA_DUTY according to it.
4. Adjust LORA channel settings LORA_BW, LORA_SF, LORA_CODE, LORA_SYNC these settings has to be the same between the nodes!
5. Comment out the plugins (//) that you do not want to use: P001,P002,P003,P005,P010,P014,P026,P034,P051 (only P001 and P002 tested)

Edit EasyLoraSensor.ino and add the necessary Devices!

Search for create_task() lines you can add up to 4
- First param: TaskID (1-4)
- Second param: PLUGIN_ID of selected device
- Third param:  Interval in seconds
- Fourth param: First GPIO
- Fifth param:  Port value (for analog plugin)
- Sixth param:  IDX at the receiving side!

For example the following line:
`create_task(1, 1, 48, 5, 0, 2);`
creates the 1st task, with a P001 device, interval: 48 seconds, on GPIO5, Port 0, and the IDX is 2!

Now you can receive data at RPIEasy LORA Direct controller, do not forget to set it's Node ID to fit to this LORA_DESTINATION_UNIT settings. RPIEasy creates non-existent devices when needed, than it will refresh incoming data and publishes events according to it.
 

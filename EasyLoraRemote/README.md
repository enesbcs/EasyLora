# EasyLoraRemote sample sketch
Experimental LoRa remote sketch for BSFrance LoRa32u4 hardware

Sketch is based on a heavily stripped down ArduinoEasy codebase, sketch size is ~25kb, memory usage ~1kb.
Purpose: LORA Receiver that executes incoming commands

Only the P001 plugin is supported currently with gpio, pwm, pulse, longpulse, tone.. and the Core ESPEasy commands, such as Reboot, Settings...

https://bsfrance.fr/lora-long-range/1345-LoRa32u4-II-Lora-LiPo-Atmega32u4-SX1276-HPD13-868MHZ-EU-Antenna.html

# Setup
Edit EasyLoraRemote-Globals.h and change settings before compile!

1. Change UNIT (node) number
2. Adjust LORA pinout settings: LORA_SS, LORA_RESET, LORA_DIO0 to match to your board!
3. Change LORA_FREQ, and adjust LORA_DUTY according to it.
4. Adjust LORA channel settings LORA_BW, LORA_SF, LORA_CODE, LORA_SYNC these settings has to be the same between the nodes!

Now you can send commands from RPIEasy LORA Direct controller, using the "loracommand,<unitno>,command" syntax. 
 

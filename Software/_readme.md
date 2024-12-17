# Software instructions

3.1. Functional description
At startup, the module tries to connect with WiFi.
If it does not succeed, it opens an access point with SSID "LPL Filament Store", where you need to fill in (first time only):
	-WiFi SSID and password
	-MQTT server IP address
	-MQTT user and password
After saving the credentials the rest of the setup will continue with initialize the hardware and display a welcome message.

Then in a continuous loop:
-	there is a check for incoming mqtt-messages (setpoint update). If so, the new setpoint values are saved. Also,
-	each second a compartment is measured and checked whether a valve must be opened or closed, and the display is updated
-	calls MQTTAutodiscover::loop()
-	every 5 seconds a state message (temperature/humidity) is sent to mqtt broker

3.2. Software modules
Software: FStorageV2_xx
Arduino IDE 2.3.3

The software is split in a few separate modules (.h, .cpp), each with their own task.
FStorageV2.xx.ino:	Main file
device_config:	Hardware info: pin assignments, global definitions		
MQTTManager:	Responsible for WiFi connection, input of MQTT credentials. Is called from MQTTAutoDiscover.
MQTTAutodiscover: 
			This module takes care of:
	Initial connection with WiFi. (uses MQTTManager)
	Connection with HomeAssistant
	Sending discovery data 
	Update sensor states, receive setpoint update.
Cabinet:	Keeps track of sensor data, controls when the valves are opened or closed, depending on humidity level and cover states.

# Electronic design

1. Functional description
The compartments (containing the rH and cover sensors) are connected with the central unit using telephone cables (RJ12 connectors).
The I2C-bus protocol is used to communicate with the rH sensors. 
A power supply (12V/2.5A) is connected to the central unit.
A 5-valve unit is connected to the central unit.

Parts:
MCU: Wemos D1 mini. The wifi enables connection with Home Assistant, it is compact, cheap, and can be programmed with the Arduino IDE. 
Humidity sensors: AM2320
Cover sensors: on/off switches
Valves: 12V solenoid type, used to provide the compartments with low rH air . 
Display: Oled 264x64, 3.1”, I2C. The MCU has a limited number of inputs/outputs, by choosing I2C, the display can be located in any of the compartments.


2. Circuit

Central unit:
![image](https://github.com/user-attachments/assets/48843edf-776b-4dbd-8e8e-59f1a05d8410)

The D1 mini is the central processing unit. Each valve can be activated through a mosfet (Vgs(th)<3V). The required power (needed to open a valve, and to keep it open) is controlled by changing the dutycycle of the IO-pin (v1..v5).

The 5V input is generated by a step-down regulator, and derived from the external 12V adapter.

The PCA9548 is an octal bidirectional translating switch controlled via the I2C-bus. Since the humidity sensor has a fixed I2C address, the switch enables access to one compartment at the time.

The cover sensors (s1..s5) are connected to MCP23008, an 8-bit bidirectional I/O-port, which is accessed through the I2C-bus.

Due to the longer leads, the I2C-bus frequency is limited to 100 kHz.
 

Compartment module: 
![image](https://github.com/user-attachments/assets/07974c43-5410-4dbf-9ab7-0592ad5aaf2b)


The AM2320 is directly connected with the modular 6P6C jack (RJ12).
Also, an extra connector is foreseen to enable access to a I2C-display.
The cover sensor is connected to pin 6.

The 3V3 OLED-display can also be connected to the I2C-bus. Since Vcc = 5V,  a small I2C bi-directional 5V-3.3V level converter is required. 


[def]: 2024-12-17-15-07-21.png

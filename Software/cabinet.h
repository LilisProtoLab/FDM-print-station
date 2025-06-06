#ifndef CABINET_H
#define CABINET_H

/*
* Setpoints, valve pins, doormasks in "device_config.h"
*/

#include <stdint.h>
#include <arduino.h>
#include <stdbool.h>

#define NROFCABS 5
#define valve_dc_on   200 //analog value to switch valve on
#define valve_dc_hold 100 //analog value to keep valve open (100)
#define valve_ontime  100 //time to keep valve fully open before switch to hold

typedef struct {
  float temperature;  //"23.5"/0
  float humidity;
  bool door; //true is open
  uint8_t doormask;
  uint8_t valvepin;
  bool valveopen;
}cabinetdata_t;

class Cabinets {
public:
  Cabinets(void);
  void state(String &payload);  // mqtt state
  uint8_t update(char* line, uint8_t cab, float humidity, float temperature, uint8_t doors); //true if valve is opened 
  void valve(uint8_t cab, bool open);
  bool valveOpen(uint8_t cab);
private:
  cabinetdata_t _data[NROFCABS];
  char _degC[3];
};


#endif
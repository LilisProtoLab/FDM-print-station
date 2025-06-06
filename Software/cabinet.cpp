#include "cabinet.h"
#include "device_config.h"

const char* cabShortNames[] = CABSHORTNAMES;

Cabinets::Cabinets(void) {
  strcpy(_degC, "oC");
  _degC[0] = 176;
  uint8_t valvepins[] = VALVEPINS;
  uint8_t doormasks[] = DOORMASKS;
  for (uint8_t cab; cab < NROFCABS; cab++) {
    _data[cab].humidity = 0;
    _data[cab].temperature = 0;
    _data[cab].doormask = doormasks[cab];
    _data[cab].valvepin = valvepins[cab];
    _data[cab].valveopen = true; //to force close
    valve(cab, false); //close
  }
}

void Cabinets::state(String &payload) {
  payload = "{";
  for (int cab = 0; cab < NROFCABINETS; cab++) {
    //Create JSON Data
    payload += "\"t";  payload += cab; payload += "\":"; payload += String(_data[cab].temperature,1);
    payload += ",\"h"; payload += cab; payload += "\":"; payload += String(_data[cab].humidity,1);
    if ( cab < (NROFCABINETS-1) ) {
      payload += ",";
    }
  }
  payload += "}";
}


uint8_t Cabinets::update(char* line, uint8_t cab, float humidity, float temperature, uint8_t doors) {
uint8_t opencovers = 0;
  /* close valve if door is open */
  for (uint8_t cab = 0; cab<NROFCABS; cab++ ) {
    _data[cab].door = (doors & _data[cab].doormask)>0;  
    if (_data[cab].door) { 
      valve(cab, false);
      bitSet(opencovers, cab);
    }
  }

  _data[cab].humidity = humidity;
  _data[cab].temperature = temperature;

  /* display text */
  // DL, DR, P4, P5, P6 
  //strcpy(line, "1:"); line[0] += cab;
  strcpy(line, cabShortNames[cab]); strcat(line, ":");
  if (isnan(humidity)) {
    strcat(line, " not connected   ");
    valve(cab, false); 
    return (opencovers);
  }
  char value[6];  //" 23.5"\0
  dtostrf(_data[cab].humidity, 5, 1, value);
  strcat(line, value);
  strcat(line, "%");
  if ( isnan(_data[cab].temperature) ) { strcpy(value, " nan "); }
  else {dtostrf(_data[cab].temperature, 5, 1, value); }
  strcat(line, value);
  strcat(line, _degC);
  // _data[cab].door ? strcat(line, " #") : strcat(line, "  ");

  /* check setpoint */
  float setpoint = (cab<2) ? deviceconfig.setpoint[spDrawer] : deviceconfig.setpoint[spPrinter];
  float error = setpoint - humidity;
  //Serial.print("error: "); Serial.println(error);
  if (error < 0) {  // humidity above setpoint, open valve
    if(!_data[cab].door) { 
      valve(cab, true); 
    }  
  } else if (error > setpoint_margin) { // humidity below (setpoint-margin), close valve
    valve(cab, false); 
  } 

  return(opencovers);
}

void Cabinets::valve(uint8_t cab, bool open) {
  if ( _data[cab].valveopen == open ) { return; }
  
  if (open) {
    analogWrite(_data[cab].valvepin, valve_dc_on);
    delay(valve_ontime);
    analogWrite(_data[cab].valvepin, valve_dc_hold); 
  } else {
    digitalWrite(_data[cab].valvepin, LOW);
  }
  _data[cab].valveopen = open;
}

bool Cabinets::valveOpen(uint8_t cab) {
  return(_data[cab].valveopen);
}





#include "device_config.h"
#include <LittleFS.h>
#include <ArduinoJson.h> 

deviceconfig_t deviceconfig;

void checkRange(int &value, int min, int max) {
  if (value<min) { value = min; }
  else if (value>max) { value = max; }
}

bool loadConfig (void) {
bool result = false;
  /* defaults */
  deviceconfig.setpoint[spPrinter] = 16;
  deviceconfig.setpoint[spDrawer] = 14;
  if (!LittleFS.begin()) { return(result); }
  if (LittleFS.exists(configfilename)) {
    File file = LittleFS.open(configfilename, "r");
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    if (!error) {
      deviceconfig.setpoint[spDrawer] = doc["spD"];
      deviceconfig.setpoint[spPrinter] = doc["spP"];
      deviceconfig.changed = true;
      result = true;
    }file.close();
  }
  LittleFS.end();
  return (result);
}

bool saveConfig (void) {
bool result = false;
  checkRange(deviceconfig.setpoint[spDrawer], 5, 50);
  checkRange(deviceconfig.setpoint[spPrinter], 5, 50);
  if (!LittleFS.begin()) { return(result); }
  LittleFS.remove(configfilename);
  File file = LittleFS.open(configfilename, "w");
  if (file) { 
    JsonDocument doc;
    doc["spD"] = deviceconfig.setpoint[spDrawer];
    doc["spP"] = deviceconfig.setpoint[spPrinter];
    if (serializeJson(doc, file) == 0) { 
      result = false; // Failed to write to file
    } else {
      deviceconfig.changed = false;
      result = true;
    }
  } 
  file.close();
  LittleFS.end();
  return result;
}





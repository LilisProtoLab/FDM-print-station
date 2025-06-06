#include "MQTTManager.h"
#include <WiFiManager.h>          
#include <ArduinoJson.h> 

static bool _shouldSaveConfig;
static void _saveConfigCallback () {
  _shouldSaveConfig = true;
}


MQTTManager::MQTTManager() {
  _shouldSaveConfig = false;
}

int8_t MQTTManager::autoConnect(mqttconfig_t &mqtt, const char* apName) {
char filename[] = "/mqtt_config";
int8_t retry = 5;
int8_t error;
  while (!LittleFS.begin()) {
    //Failed to initialize LittleFS library
    retry--;
    if (retry==0) { return(-1); }
    delay(1000);
  }

  _loadConfig(mqtt, filename);

  // id/name, placeholder/prompt, default, length
  WiFiManagerParameter wmp_server("server", "mqtt server", mqtt.server, sizeof(mqtt.server));
  String port = String(mqtt.port);  //convert integer to string
  WiFiManagerParameter wmp_port("port", "mqtt port", port.c_str(), 6);
  WiFiManagerParameter wmp_user("user", "mqtt user", mqtt.user, sizeof(mqtt.user));
  WiFiManagerParameter wmp_passw("passw", "mqtt password", mqtt.password, sizeof(mqtt.password));

  WiFiManager wm;
  
  //set config save notify callback
  wm.setSaveConfigCallback(_saveConfigCallback);
  //add parameters
  wm.addParameter(&wmp_server);
  wm.addParameter(&wmp_port);
  wm.addParameter(&wmp_user);
  wm.addParameter(&wmp_passw);

  if (wm.autoConnect(apName)) {
    error = 0;
    if (_shouldSaveConfig) { //read updated parameters
      strcpy(mqtt.server, wmp_server.getValue());
      port = wmp_port.getValue();
      mqtt.port = port.toInt();
      strcpy(mqtt.user, wmp_user.getValue());
      strcpy(mqtt.password, wmp_passw.getValue());
      _saveConfig(mqtt, filename);
    } 
  } else {
    error = -2;
      //failed to connect and hit timeout
      /*
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
      */
  }

  LittleFS.end();

  return error;
}

void MQTTManager::resetWiFi(void) {
  WiFiManager wm;
  wm.resetSettings();
}

void MQTTManager::_loadConfig(mqttconfig_t &mqtt, const char* filename) {
  if (LittleFS.exists(filename)) {
    File file = LittleFS.open(filename, "r");
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
      //Failed to read file, using default configuration
      return;
    }
    strlcpy(mqtt.server, doc["server"], sizeof(mqtt.server));
    mqtt.port = doc["port"];
    strlcpy(mqtt.user, doc["user"], sizeof(mqtt.user));
    strlcpy(mqtt.password, doc["passw"], sizeof(mqtt.password));
    file.close();
  }
}

void MQTTManager::_saveConfig(mqttconfig_t &mqtt, const char* filename) {
  // Delete existing file, otherwise the configuration is appended to the file
  LittleFS.remove(filename);
  // Open file for writing
  File file = LittleFS.open(filename, "w");
  if (!file) {  //shit!
    // Failed to create file
    return;
  }
  JsonDocument doc;
  doc["server"] = mqtt.server;
  doc["port"] = mqtt.port;
  doc["user"] = mqtt.user;
  doc["passw"] = mqtt.password;
  if (serializeJson(doc, file) == 0) {
    // Failed to write to file
  }
  file.close();
} 





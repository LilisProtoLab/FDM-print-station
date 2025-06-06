#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <LittleFS.h>

/* autoConnect error
  0 = ok
  -1 = Failed to initialize LittleFS library
  -2 = Failed to connect and hit timeout
*/

typedef struct  {
  char server[16] = "192.168.178.110";
  int port = 1883;
  char user[20] = "mqtt_user";
  char password[20] = "mqtt_passw";
}mqttconfig_t;

class MQTTManager {
public:
  MQTTManager();
  int8_t autoConnect(mqttconfig_t &mqtt, const char* apName);
  void resetWiFi(void);
private:
  void _loadConfig(mqttconfig_t &mqtt, const char* filename);
  void _saveConfig(mqttconfig_t &mqtt, const char* filename);
};






#endif

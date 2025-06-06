#ifndef MQTTAUTODISCOVER_H
#define MQTTAUTODISCOVER_H

/*Wifi Parameters
 WIFI_SSID "Lili's Proto Lab"
 WIFI_PASSWORD "98589173"
// MQTT Parameters
 MQTT_SERVER "192.168.0.142"
 MQTT_USER "mqtt_user"
 MQTT_PASSWORD "mqtt"
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <MQTTManager.h>  // nh: esp8266 only
#include "device_config.h"

#define CLIENTBUFFERSIZE 500  //must fit largest (discovery) payload
#define AP_NAME "LPL Filament Store"  //access point identifier wifimanager
#define DEVICENAME "fstore"
#define REFRESH_DELAY 1000  

#define UIDSIZE 7   //nrof hexchars taken from macaddress, is added to devicename: fstoreA534D6. add 1 for trailing /0
#define CLIENTID_TEMPLATE "client%s" //addUID
#define DEVICEID_TEMPLATE "fstore%s" //addUID
#define ENTITYID_TEMPLATE "%s_%s%d"     //deviceid + suffix, ie fstoreABCDEF_T1
#define DISCOVERY_SENSOR_TOPIC_TEMPLATE "homeassistant/sensor/%s/config" //add entity
#define DISCOVERY_NUMBER_TOPIC_TEMPLATE "homeassistant/number/%s/config"
#define STATE_TOPIC_TEMPLATE "%s/state"  // temperature, humidity + deviceid
#define SP_CMD_TOPIC_TEMPLATE "%s/cmd%d"   // ../cmd0, ../cmd1: setpoints
#define SP_STATE_TOPIC_TEMPLATE "%s/spstate"  //deviceid
#define SP_VALUE_TEMPLATE "{\"drw\":%d,\"prt\":%d}"



class MQTTDiscover {
public:
  MQTTDiscover(void);
  void begin(void);
  bool loop(void); //call from main loop
  //payload example: {"t0":20.4,"h0":10.2,"t1":21.4,"h1":11.1,"t2":22.4,"h2":15.6,"t3":23.4,"h3":19.5,"t4":24.4,"h4":19.3}
  bool updateState(String& payload);
  bool updateSetpointState(void);

private:
  WiFiClient* _espClient;
  PubSubClient* _client;//(_espClient);
  bool _discovered;
  mqttconfig_t _mqtt;  
  char _clientid[sizeof(CLIENTID_TEMPLATE) + UIDSIZE - 2];
  char _deviceid[sizeof(DEVICEID_TEMPLATE) + UIDSIZE - 2];
  char _state_topic[sizeof(STATE_TOPIC_TEMPLATE) + sizeof(_deviceid) - 2]; // humidity/temperature
  char _sp_state_topic[sizeof(SP_STATE_TOPIC_TEMPLATE) + sizeof(_deviceid) - 2]; //setpoints
  bool reconnect(void);
  void setupWIFI(void);
  void processCallback(void);
  bool Discovery(bool on);
  bool hasHumidity(uint8_t cab, bool on);
  bool hasTemperature(uint8_t cab, bool on);
  bool hasSetpoint(setpoint_t kind, bool on);

};

void UID(char* uid, uint8_t* mac, uint8_t size);



#endif

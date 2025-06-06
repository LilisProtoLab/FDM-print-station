#include "MQTTAutodiscover.h"
#include <ArduinoJson.h>   

typedef struct {
  String topic;
  String payload;
  bool received;
} callbackdata_t;

const char* friendlynames[] = CABNAMES;

static callbackdata_t mqttcallbackdata;

static void mqttcallback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  mqttcallbackdata.topic = String((char*)topic);
  mqttcallbackdata.payload = String((char*)payload);
  mqttcallbackdata.received = true;
}

void UID(char* uid, uint8_t* mac, uint8_t size) {
  snprintf(uid, size, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}



MQTTDiscover::MQTTDiscover(void) {
  _espClient = new WiFiClient;  
  _client = new PubSubClient(*_espClient);
  _discovered = false;
}

void MQTTDiscover::begin(void) {
  setupWIFI();
}

bool MQTTDiscover::loop(void) {
bool result = false;
  result = reconnect();
  _client->loop();

  if (mqttcallbackdata.received) {
    processCallback();
    mqttcallbackdata.received = false;
  }

  return (result);

}

void MQTTDiscover::processCallback(void) {
  //example: fstoreC9C0A3/cmd1, 14)
//  debug("callback: "); debug(callbackdata.topic); debug(", "); debugln(callbackdata.payload);
char c;
setpoint_t kind;
int num = mqttcallbackdata.payload.toInt();
  c = mqttcallbackdata.topic[mqttcallbackdata.topic.length()-1];
  if (c=='0') { 
    kind = spDrawer; 
  } else if (c=='1') { 
    kind = spPrinter;
  } else { 
    return; 
  }
  int value = mqttcallbackdata.payload.toInt();
  if (value==0) { return; } 
  deviceconfig.setpoint[kind] = value;
  deviceconfig.changed = true;
}

bool MQTTDiscover::reconnect(void) { // do not loop until we're reconnected
bool ok = true;
  if (_client->connected()) { return true; }
  //debugln("Attempting MQTT connection...");
  ok = _client->connect(_clientid, _mqtt.user, _mqtt.password);
  if (ok) {
    //debugln("connected");
    //if (discovered) {debugln("discovered = true");}
    //else {debugln("discovered = false");}
    if (!_discovered) {
      Discovery(true);
      //debugln("discovered = true");
      
      _discovered = true;
    }
  } else {
    //debug("failed, rc=");
    //debug(_client.state());
  }
  return(ok);
}

bool MQTTDiscover::updateState(String& payload) {
  if (_client->publish(_state_topic, payload.c_str(), false)) { return(true); }
  return(false);
}

bool MQTTDiscover::updateSetpointState(void) {
char payload[sizeof(SP_VALUE_TEMPLATE)];
  snprintf(payload, sizeof(payload), SP_VALUE_TEMPLATE, deviceconfig.setpoint[spDrawer], deviceconfig.setpoint[spPrinter]);
  if (_client->publish(_sp_state_topic, payload) ) { return(true); }
  return(false);
}



void MQTTDiscover::setupWIFI(void) {
    //debugln("autoconnect wifi");
    MQTTManager mm;
    int8_t error = mm.autoConnect(_mqtt, AP_NAME);
    //set up unique id's
  byte mac[6];
    WiFi.macAddress(mac);
  char uid[UIDSIZE];
    UID(uid, mac, UIDSIZE);
    snprintf(_clientid, sizeof(_clientid), CLIENTID_TEMPLATE, uid);
    snprintf(_deviceid, sizeof(_deviceid), DEVICEID_TEMPLATE, uid);
    snprintf(_state_topic, sizeof(_state_topic), STATE_TOPIC_TEMPLATE, _deviceid);
    snprintf(_sp_state_topic, sizeof(_sp_state_topic), SP_STATE_TOPIC_TEMPLATE, _deviceid);
    // Dump config file
    //printWifiConfiguration();

    _client->setServer(_mqtt.server, _mqtt.port);
    _client->setCallback(mqttcallback);
    _client->setBufferSize(CLIENTBUFFERSIZE);
}

bool MQTTDiscover::hasTemperature(uint8_t cab, bool on) {
char payload[CLIENTBUFFERSIZE]="";
char entityid[sizeof(_deviceid) + 3/*suffix*/];
  snprintf(entityid, sizeof(entityid), ENTITYID_TEMPLATE, _deviceid, "T", cab);

  if (on) {
    DynamicJsonDocument doc(CLIENTBUFFERSIZE);
    doc.clear();
    doc["name"] = String("T") + String(friendlynames[cab]); //friendly name
    doc["obj_id"] = String("temperature") + String(cab);   
    doc["dev_cla"] = "temperature";                                                         
    doc["stat_t"] = _state_topic;                            
    doc["unit_of_meas"] = "°C";                                                             
    doc["val_tpl"] = "{{value_json.t" + String(cab) +  String("}}");  
    doc["uniq_id"] = entityid; 
    JsonObject device = doc.createNestedObject("device");
      device["ids"] = _deviceid;
      if (cab==0) {  // need to send only once:
        device["name"] = "Filament Store"; 
        device["mf"] = "lpl/nvh"; //manufacturer
        device["mdl"] = "Filament store";  //first line in apparaat info
        device["hw"] = HW_VERSION;
        device["sw"] = SW_VERSION; 
      //"configuration_url": "https://example.com/sensor_portal/config"
      }
    serializeJson(doc, payload);
  }

  char topic[sizeof(DISCOVERY_SENSOR_TOPIC_TEMPLATE) + sizeof(entityid) - 2];
  snprintf(topic, sizeof(topic), DISCOVERY_SENSOR_TOPIC_TEMPLATE, entityid);
  //debug("topic: "); debugln(topic);
  //debug("payload="); debugln(payload);
  return _client->publish(topic, payload, true);
}

bool MQTTDiscover::hasHumidity(uint8_t cab, bool on) {
char payload[CLIENTBUFFERSIZE] = "";
char entityid[sizeof(_deviceid) + 3/*suffix*/];
  snprintf(entityid, sizeof(entityid), ENTITYID_TEMPLATE, _deviceid, "H", cab);

  if (on) { 
    DynamicJsonDocument doc(CLIENTBUFFERSIZE);
    doc.clear();
    doc["name"] = String("H") + String(friendlynames[cab]); //friendly name  
    //doc["obj_id"] = String("humidity") + String(cab);         
    doc["dev_cla"] = "humidity";                                                         
    doc["stat_t"] = _state_topic;                            
    doc["unit_of_meas"] = "%";                                                             
    doc["val_tpl"] =  "{{value_json.h" + String(cab) +  String("}}");  
    doc["uniq_id"] = entityid; 
    JsonObject device = doc.createNestedObject("device");
      device["ids"] = _deviceid;

    serializeJson(doc, payload);
  }
  char topic[sizeof(DISCOVERY_SENSOR_TOPIC_TEMPLATE) + sizeof(entityid) - 2];
  snprintf(topic, sizeof(topic), DISCOVERY_SENSOR_TOPIC_TEMPLATE, entityid);
  //debug("topic: "); debugln(topic);
  //debug("payload="); debugln(payload);
  return _client->publish(topic, payload, true);
}

bool MQTTDiscover::hasSetpoint(setpoint_t kind, bool on) {  //drawer(kind=0) or printer(kind=1)
char payload[CLIENTBUFFERSIZE] = "";
char cmd_topic[sizeof(SP_CMD_TOPIC_TEMPLATE) + sizeof(_deviceid) - 3];
char entityid[sizeof(_deviceid) + 3/*suffix*/];
  snprintf(entityid, sizeof(entityid), ENTITYID_TEMPLATE, _deviceid, "N", kind);

  if (on) {  
    snprintf(cmd_topic, sizeof(cmd_topic), SP_CMD_TOPIC_TEMPLATE, _deviceid, kind);  // ../cmd0, ../cmd1: setpoints
    DynamicJsonDocument doc(CLIENTBUFFERSIZE);
    doc.clear();
    if (kind==0) {
      doc["name"] = "Drawer humidity setpoint"; 
      doc["val_tpl"] = String("{{ value_json.drw }}"); 
    } else {
      doc["name"] = "Printer humidity setpoint"; 
      doc["val_tpl"] = String("{{ value_json.prt }}"); 
    }
    doc["unit_of_meas"] = "%";    
    doc["mode"] = "box";    
    doc["uniq_id"] = entityid;
    doc["min"] = 10;
    doc["max"] = 25;
    doc["step"] = 1;
    doc["initial"] = 20;
    doc["stat_t"] = _sp_state_topic;
    doc["cmd_t"] = cmd_topic;
    JsonObject device = doc.createNestedObject("device");
    device["ids"] = _deviceid;
    serializeJson(doc, payload);
    //debug("payload="); debugln(payload);
  }
  char topic[sizeof(DISCOVERY_NUMBER_TOPIC_TEMPLATE) + sizeof(entityid) - 2];
  snprintf(topic, sizeof(topic), DISCOVERY_NUMBER_TOPIC_TEMPLATE, entityid);
  //debug("topic: "); debugln(topic);
  //debug("payload="); debugln(payload);
  _client->publish(topic, payload, true);
  //subscribe to command topic
  if (on) { _client->subscribe(cmd_topic); }
  return true;
}

bool MQTTDiscover::Discovery(bool on) { //discovery message
bool ok = true;
  for (int cab=0; cab<NROFCABINETS; cab++) {
    // if not(discovery) ok = false; try next time
    hasTemperature(cab, on); 
    hasHumidity(cab, on);
  }
  hasSetpoint(spDrawer, on);
  hasSetpoint(spPrinter, on);
  return ok;
}


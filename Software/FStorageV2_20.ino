#include <Wire.h>
#include <MCP23008.h>
#include <Adafruit_AM2320.h>
#include <U8g2lib.h>
#include "device_config.h"
#include "cabinet.h"
#include "MQTTAutodiscover.h"


//#define debug_print  // remove to disable serial output
#include <NHDebug.h>

//#define SKIPWIFI

U8G2_SSD1309_128X64_NONAME2_F_HW_I2C  u8g2(U8G2_R0) ;  //??software??
Cabinets cabs;
MCP23008 MCP(mcpAddr);
Adafruit_AM2320 am2320 = Adafruit_AM2320();
uint8_t displaychannel;
MQTTDiscover mqtt;


unsigned long refreshTime;

/******** forward declarations ********/
//main functions:
void ledToggle(void);
bool isConnected(uint8_t addr);  //I2C check
bool PCA_Select(uint8_t channel); //i2c switch to cabinet
void readCabinet(uint8_t cab); //read sensors
//display
bool initDisplay(void);
void Hello(void);
void testDisplay(void);
void updateState(void);  //send temperature/humidity data
//debug:
void binString(char* str, uint8_t value );
void debugCabinet(uint8_t cab);
void debugMeasure(uint8_t cab, float& hum, float& temp, uint8_t& doors);

/****** end forward declarations ******/



void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  debug_begin(115200);
  Wire.begin(sdaPin, sclPin);
  initDisplay();

  #ifndef SKIPWIFI
    u8g2.drawStr(0,12+12*1,"autoconnect wifi");	
    u8g2.sendBuffer();					// transfer internal memory to the display
    mqtt.begin();  // connect WiFi, HomeAssistant
  #endif

//  Wire.begin(sdaPin, sclPin);
  MCP.begin();
  MCP.pinMode8(0xFE); // 0=output
//  initDisplay();
  Hello();
  am2320.begin();

  debugln();
  debugln("loading config");
  loadConfig();

  refreshTime = millis();
}

/* measure one cabinet each second, refresh delay. update state every 5 seconds */
uint8_t activecab = 0;
uint16_t loopcount = 0; // for debug cabinets
bool STATEFLAG = false;  //update has every 5 loops


void loop() { 
  ++loopcount;

  #ifndef SKIPWIFI
    if (!mqtt.loop() ) { debugln("mqtt reconnect error");}
    if (STATEFLAG) { updateState(); STATEFLAG = false; } //each 5 loops
  #endif

  if (millis()>refreshTime) { //update each second
    ledToggle();
    readCabinet(activecab);  //also update display
    //debugCabinet(activecab);  //also update display
    //testDisplay(activecab);
    activecab = (activecab<4) ? (activecab+1) : 0;
    STATEFLAG = (activecab==0);
    refreshTime = millis() + REFRESH_DELAY;  
  }

  if (deviceconfig.changed) {
    debugln("saving config");
    saveConfig();
    debug("setpoints: "); debug(deviceconfig.setpoint[spDrawer]); debug(", "); debugln(deviceconfig.setpoint[spPrinter]); 
    mqtt.updateSetpointState();
  }

}

void ledToggle(void) {
  digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
}

void readCabinet(uint8_t cab) {
float hum, temp;
uint8_t doors;
char line[20];
  PCA_Select(cab);
  doors = MCP.read8();
  temp = am2320.readTemperature();
  hum = am2320.readHumidity();
  doors = cabs.update(line, cab, hum, temp, doors);  // save data, check checkpoint
  /* update display */
  if (PCA_Select(displaychannel) ) {
    u8g2.drawStr(0,12+12*cab,line);	
    debug ("doors="); debugln(doors);
    for (uint8_t i=0; i<NROFCABS; i++) {
      if (doors & 0x01) { strcpy(line,"# "); }
      else { strcpy(line,"  "); }
      if (cabs.valveOpen(i)) {line[1]=187; }
      u8g2.drawStr(18*6,12+12*i,line);	
      doors = doors >> 1;
    }
    u8g2.sendBuffer();					// transfer internal memory to the display
  }
}

void debugCabinet(uint8_t cab) {
float hum, temp;
uint8_t doors;
char line[20];
uint8_t valve;
  //debugln();
  //debug("**** testing cab "); debug(cab); debugln(" ****");
  debugMeasure(cab, hum, temp, doors);
  PCA_Select(cab);
  valve = cabs.update(line, cab, hum, temp, doors);  // save data
  if (PCA_Select(displaychannel) ) {
    u8g2.drawStr(0,12+12*cab,line);	// write a line
    u8g2.sendBuffer();					// transfer internal memory to the display
  }
  //debugln(line);
}

void debugMeasure(uint8_t cab, float& hum, float& temp, uint8_t& doors) {
  hum = 15 + 5*sin(0.01*PI*loopcount+cab);
  temp = 20.4 + cab + cab/10;
  doors = 0; // all doors closed
}

void updateState(void) {
String payload;
  cabs.state(payload);  //get payload
 // debugln("state payload:"); debugln(payload);
  mqtt.updateState(payload);
}


/*********** display ***********/

bool initDisplay(void) {
  displaychannel = 0xFF;
  for (uint8_t i=0; i<7; i++) {
    PCA_Select(i);
    if (isConnected(oledAddr)) { 
      displaychannel = i;
      u8g2.setBusClock(100000);
      u8g2.begin();
      u8g2.setFont(u8g2_font_6x10_mf);
//      Hello();
      u8g2.clearBuffer();	// clear the internal memory
      return(true);
    }
  }
  debug("display not found. Channel="); debugln(displaychannel);
  return(false);
}

void Hello(void) {
  if (!PCA_Select(displaychannel)) { return; };
  u8g2.clearBuffer();	
  u8g2.drawStr(0,12+12*0,"Lili's proto lab");	
  u8g2.drawStr(0,12+12*1,"Filament storage");	
  u8g2.drawStr(0,12+12*2,"humidity control");	
char version[] = SW_VERSION;
char line[20];
  strcpy(line, "software version ");
  strcat(line, version);
  u8g2.drawStr(0,12+12*3,line);	
  u8g2.sendBuffer();					// transfer internal memory to the display
  delay(2000);
  u8g2.clearBuffer();	
}


void testDisplay(uint8_t cab) {
char line[22];
  u8g2.clearBuffer();	
  strcpy(line,"X***************X");
  for (uint8_t i=0; i<10; i++) {
    line[i+1] = 181+i;
  }
//  strcpy(line, "123456789012345678901");
  u8g2.drawStr(0,12+12*1,line);	
  if (cab%2) { strcpy(line,"01"); }
  else {strcpy(line,"  ");}
  u8g2.drawStr(6*19,12+12*2,line);	

  u8g2.sendBuffer();					// transfer internal memory to the display
}



/****** measurements *****/

bool isConnected(uint8_t addr) {
  Wire.beginTransmission(addr);
  return ( Wire.endTransmission() == 0);
}

bool PCA_Select(uint8_t channel) {
  if (channel>7) { return(false); }
  Wire.beginTransmission(pcaAddr);
  Wire.write(0x01 << channel);
  Wire.endTransmission();  
  delay(100);
  return(true);
}

void binString(char* str, uint8_t value ) {
uint8_t mask = 0x80;
  strcpy(str, "00000000");
  for (uint8_t i=0; i<8; i++) {
    if ((value & mask) > 0) { str[i]='1'; } 
    mask = mask >> 1;  
  }
}










#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

/*
* The system consists of 5 cabinets: 2 drawers (left/right) and 3 printers (no4/5/6)
* Each cabinet has a temperature-, humidity- and door-sensor
* The printers and drawers have a separate setpoint for humidity
*/

#include <arduino.h>
#include <stdint.h>

#define HW_VERSION "1.0"
#define SW_VERSION "2.20"


//cabinet names
#define NROFCABINETS 5
/*pca channels I2C switch cab0..4 ~ channel 0..4 */
#define CABNAMES {"drawerL", "drawerR", "printer4", "printer5", "printer6"}
#define CABSHORTNAMES {"DL", "DR", "P4", "P5", "XL"}
#define VALVEPINS {D1, D2, D5, D6, D7}
#define DOORMASKS {0x02, 0x04, 0x20, 0x40, 0x80}
#define REFRESH_DELAY 1000


/*** hardware connections ***/
#define sdaPin D4
#define sclPin D3
#define pcaAddr 0x70
#define mcpAddr 0x20
#define am2320Addr 0x5C
#define oledAddr 0x3C


typedef enum {spDrawer, spPrinter, spCount} setpoint_t;

#define configfilename "/devconfig"
#define setpoint_min 5
#define setpoint_max 30
#define setpoint_margin 1  
typedef struct {
  int setpoint[spCount];
  bool changed = false;
}deviceconfig_t;



bool loadConfig (void);
bool saveConfig (void);


extern deviceconfig_t deviceconfig;


#endif

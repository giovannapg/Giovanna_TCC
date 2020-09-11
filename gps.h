
#ifndef GPS_H
#define GPS_H

#include <TinyGPS++.h> // http://arduiniana.org/libraries/tinygpsplus/

TinyGPSPlus gps;

HardwareSerial SerialGPS(2);

const int GPS_BAUD_RATE = 9600;

const int RX2_PIN = 18, TX2_PIN = 19;

 char latm[10];
 char longm[20];
 char lat_long[20];
 float latf;
 float longf;
 char sats[8], ground_speed[8];
 char loc_DMS[20];
 char N_S;
 char L_W;

void setupGPS(void);
void updateGPS(void);
void displayInfo(void);
void DegMinSec( double tot_val, double tot_val1);

#endif

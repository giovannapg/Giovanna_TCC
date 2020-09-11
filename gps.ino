
#include "gps.h"
#include "wifi.h"
//#include <TinyGPS++.h> // http://arduiniana.org/libraries/tinygpsplus/

// The TinyGPS++ object
//TinyGPSPlus gps;

char gps_status = 0;

void setupGPS(void)
{
  SerialGPS.begin(GPS_BAUD_RATE, SERIAL_8N1, RX2_PIN, TX2_PIN, false);
  delay(1000);
}

void updateGPS(void)
{
    /**********************************************/
    // Tratamento e coleta de dados do GPS
    /**********************************************/
    
    while (SerialGPS.available() > 0) if(gps.encode(SerialGPS.read()));

    if (millis() > 5000 && gps.charsProcessed() < 10)
    {
      Serial.println(F("No GPS detected: check wiring."));
      gps_status = 0;
    }else
    {
      Serial.println("GPS OK");
      gps_status = 1;
    }
}

void displayInfo(void)
{
  int ciclos = 0;
  float speed_a;
  unsigned int satelites;

  satelites = gps.satellites.value();

  if(satelites == 0)  // Se não eoncontrou satélites, tenta capturar por 10 tentativas
  {
    while(satelites == 0 && ciclos < 5)
    {
      updateGPS();
      satelites = gps.satellites.value();
      ciclos++;
      delay(1000);
    }
  }

  mqtt.publish(topicGPSStatus, gps_status ? "1" : "0");

  Serial.print("Satellites: ");
  Serial.println(satelites);
  
  sprintf(sats, "%d", gps.satellites.value());
  mqtt.publish(topicSats, sats);

  //if(gps.speed.isValid());
  
  Serial.print(F("Location: ")); 

  if(gps.location.isValid())
  {    
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.println(gps.location.lng(), 6);
    Serial.println("");
    
    latf = gps.location.lat();
    sprintf (latm, "%2.6f", abs(gps.location.lat()));
    
    longf = gps.location.lng();
    sprintf (longm, "%2.6f", abs(gps.location.lng()));
    
    N_S = gps.location.rawLat().negative? 'S' : 'N';
    L_W = gps.location.rawLng().negative? 'W' : 'L';
    
    DegMinSec(gps.location.lat(), gps.location.lng());
    
    sprintf(lat_long, "%2.6f\,%2.6f", gps.location.lat(), gps.location.lng());
    mqtt.publish(topicGPSLoc, lat_long);
  }
  else
  {
    Serial.println(F("INVALID POS"));
    sprintf(latm, "INVALID POS");
    speed_a = 0;
  }

  Serial.print(F("Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.println(gps.time.centisecond());
  }
  else
  {
    Serial.println(F("INVALID"));
  }  
}

void DegMinSec( double tot_val, double tot_val1)    /* Convert data in decimal degrees into degrees minutes seconds form */
{
  volatile float minutes, seconds;
  volatile int degree, secs, mins;

  volatile float minutes1, seconds1;
  volatile int degree1, secs1, mins1;
  
  degree = (int)tot_val;
  minutes = abs(tot_val - degree);
  seconds = 60 * minutes;
  minutes = (int)seconds;
  mins = (int)minutes;
  seconds = seconds - minutes;
  seconds = 60 * seconds;
  secs = (int)seconds;

  degree1 = (int)tot_val1;
  minutes1 = abs(tot_val1 - degree1);
  seconds1 = 60 * minutes1;
  minutes1 = (int)seconds1;
  mins1 = (int)minutes1;
  seconds1 = seconds1 - minutes1;
  seconds1 = 60 * seconds1;
  secs1 = (int)seconds1;

  Serial.print(degree);
  Serial.print("°");
  Serial.print(mins);
  Serial.print("'");
  Serial.print(seconds);
  Serial.print("\"");
  Serial.print(" ");

  Serial.print(degree1);
  Serial.print("°");
  Serial.print(mins1);
  Serial.print("'");
  Serial.print(seconds1);
  Serial.print("\"");

  sprintf(loc_DMS, "%d°%d'%2.4f\" %d°%d'%2.4f\"",degree, mins, seconds,degree1, mins1, seconds1);
  mqtt.publish(topicDMS, loc_DMS);
}

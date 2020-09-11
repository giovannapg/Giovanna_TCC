#include <Wire.h>
#include <VL53L0X.h>
#include <ADXL345.h>
#include "gps.h"
#include "wifi.h"

const int motorA1  = 9;    // Pin  5 of L293.
const int motorA2  = 3;    // Pin  6 of L293.
int eixo_X= 34;//A0; //PINO REFERENTE A LIGAÇÃO DO EIXO X
int eixo_Y = 35;//A1; //PINO REFERENTE A LIGAÇÃO DO EIXO Y
int botao = 2; //PINO REFERENTE A LIGAÇÃO NO PINO KEY (EIXO Z)
VL53L0X sensor;
int i = 0;
int j = 0;
int state_rec;
char state;

ADXL345 adxl; //variable adxl is an instance of the ADXL345 library

void setup() 
{
  setupGPS();
  wifi_connect();
  initMQTT();
  connectMQTT();
  
  adxl.powerOn();
    //set activity/ inactivity thresholds (0-255)
  adxl.setActivityThreshold(75); //62.5mg per increment
  adxl.setInactivityThreshold(75); //62.5mg per increment
  adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?
 
  //look of activity movement on this axes - 1 == on; 0 == off 
  adxl.setActivityX(1);
  adxl.setActivityY(1);
  adxl.setActivityZ(1);
 
  //look of inactivity movement on this axes - 1 == on; 0 == off
  adxl.setInactivityX(1);
  adxl.setInactivityY(1);
  adxl.setInactivityZ(1);
 
  //look of tap movement on this axes - 1 == on; 0 == off
  adxl.setTapDetectionOnX(0);
  adxl.setTapDetectionOnY(0);
  adxl.setTapDetectionOnZ(1);
 
  //set values for what is a tap, and what is a double tap (0-255)
  adxl.setTapThreshold(50); //62.5mg per increment
  adxl.setTapDuration(15); //625us per increment
  adxl.setDoubleTapLatency(80); //1.25ms per increment
  adxl.setDoubleTapWindow(200); //1.25ms per increment
 
  //set values for what is considered freefall (0-255)
  adxl.setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment
 
  //setting all interrupts to take place on int pin 1
  //I had issues with int pin 2, was unable to reset it
  adxl.setInterruptMapping( ADXL345_INT_SINGLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_DOUBLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_FREE_FALL_BIT,    ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_ACTIVITY_BIT,     ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_INACTIVITY_BIT,   ADXL345_INT1_PIN );
 
  //register interrupt actions - 1 == on; 0 == off  
  adxl.setInterrupt( ADXL345_INT_SINGLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_DOUBLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_FREE_FALL_BIT,  1);
  adxl.setInterrupt( ADXL345_INT_ACTIVITY_BIT,   1);
  adxl.setInterrupt( ADXL345_INT_INACTIVITY_BIT, 1);
  
  pinMode (12, INPUT_PULLUP);
  digitalWrite (12, HIGH);
  Serial.begin(9600);
  Serial.print("Teste");
  Wire.begin();
  sensor.init();
  sensor.setTimeout(500);
  sensor.startContinuous();
  
  pinMode (botao, INPUT_PULLUP);
  
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
}

void loop() 
{
  int distance =sensor.readRangeContinuousMillimeters();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print("mm");

  if (sensor.timeoutOccurred()) 
  { 
    Serial.print(" TIMEOUT"); 
  }

  Serial.println();


  if (Serial.available() > 0) 
  {
    state_rec = Serial.read();
    state = state_rec;
   
  }

  
  if((analogRead(eixo_X)) == 0)
  { 
    Serial.println("PARA FRENTE");
    digitalWrite(motorA1, 1);
    digitalWrite(motorA2, 1);
  }

    if((analogRead(eixo_Y)) == 1023)
   { 
     Serial.println("ESQUERDA");
     digitalWrite(motorA1, 1);
     digitalWrite(motorA2, 0);
  }

    if((analogRead(eixo_Y)) == 0)
  { 
    Serial.println("DIREITA"); 
    digitalWrite(motorA1, 0);
    digitalWrite(motorA2, 1);
  }

  if((analogRead(eixo_X)) == 1023)
 {
    Serial.println("PARA TRAS");
    digitalWrite(motorA1, 0);
    digitalWrite(motorA2, 0);
    
 }

 if(digitalRead(botao) == LOW)
 { 
   Serial.println("PARADO");
   digitalWrite(motorA1, 0);
   digitalWrite(motorA2, 0);
 }

Serial.print("eixo_X: ");
Serial.println(analogRead(eixo_X));

Serial.print("eixo_Y: ");
Serial.println(analogRead(eixo_Y));

//Boring accelerometer stuff   
 int x,y,z;  
  adxl.readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z
  // Output x,y,z values 
  Serial.print("values of X , Y , Z: ");
  Serial.print(x);
  Serial.print(" , ");
  Serial.print(y);
  Serial.print(" , ");
  Serial.println(z);
  
  double xyz[3];
  double ax,ay,az;
  adxl.getAcceleration(xyz);
  ax = xyz[0];
  ay = xyz[1];
  az = xyz[2];
  Serial.print("X=");
  Serial.print(ax);
    Serial.println(" g");
  Serial.print("Y=");
  Serial.print(ay);
    Serial.println(" g");
  Serial.print("Z=");
  Serial.print(az);
    Serial.println(" g");
  Serial.println("**********************");
delay(1000);  
}

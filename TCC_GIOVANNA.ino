#include <Wire.h>
#include <VL53L0X.h>
#include <ADXL345.h>
#include "gps.h"
#include "wifi.h"

//variáveis para caso de cadeiras com motor
//const int motorA1  = 9;    // Pin  5 of L293.
//const int motorA2  = 3;    // Pin  6 of L293.
//int eixo_X= 34;//A0; //PINO REFERENTE A LIGAÇÃO DO EIXO X
//int eixo_Y = 35;//A1; //PINO REFERENTE A LIGAÇÃO DO EIXO Y
//int botao = 2; //PINO REFERENTE A LIGAÇÃO NO PINO KEY (EIXO Z)

VL53L0X sensor;
int i = 0;
int j = 0;
int state_rec;
char state;

ADXL345 adxl; //variable adxl is an instance of the ADXL345 library

int valor_ax_anterior;
int valor_ay_anterior;
int valor_az_anterior;
int valor_ax_atual;
int valor_ay_atual;
int valor_az_atual;
int diferenca_ax;
int diferenca_ay;
int diferenca_az;

int velocidade;
char velocidade_texto [32];

char d;

void setup() 
{
  Serial.begin(115200);
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

  Wire.begin();
  sensor.init();
  sensor.setTimeout(500);
  sensor.startContinuous();
  
  //pinMode (botao, INPUT_PULLUP);
  
  //pinMode(motorA1, OUTPUT);
  //pinMode(motorA2, OUTPUT);
}

void loop() 
{
  //LIDAR
  int distance =sensor.readRangeContinuousMillimeters();
  //Serial.print("Distance: ");
  //Serial.print(distance);
  //Serial.println("mm");

  if (distance > 150)
  {
    //Serial.println("Perigo de desnível - travamento de rodas");
    mqtt.publish("/tcc/distancia", "Perigo de desnível - travamento de rodas");
  }

  //Serial.println("**********************");

  if (sensor.timeoutOccurred()) 
  { 
    //Serial.print(" TIMEOUT"); 
  }

  //Serial.println();


  if (Serial.available() > 0) 
  {
    state_rec = Serial.read();
    state = state_rec;
   
  }


  //if((analogRead(eixo_X)) == 0)
  //{ 
    //Serial.println("PARA FRENTE");
    //digitalWrite(motorA1, 1);
    //digitalWrite(motorA2, 1);
  //}

    //if((analogRead(eixo_Y)) == 1023)
   //{ 
     //Serial.println("ESQUERDA");
     //digitalWrite(motorA1, 1);
     //digitalWrite(motorA2, 0);
  //}

    //if((analogRead(eixo_Y)) == 0)
  //{ 
    //Serial.println("DIREITA"); 
    //digitalWrite(motorA1, 0);
    //digitalWrite(motorA2, 1);
  //}

  //if((analogRead(eixo_X)) == 1023)
 //{
    //Serial.println("PARA TRAS");
    //digitalWrite(motorA1, 0);
    //digitalWrite(motorA2, 0);
    
 //}

 //if(digitalRead(botao) == LOW)
 //{ 
   //Serial.println("PARADO");
   //digitalWrite(motorA1, 0);
   //digitalWrite(motorA2, 0);
 //}

//Serial.print("eixo_X: ");
//Serial.println(analogRead(eixo_X));

//Serial.print("eixo_Y: ");
//Serial.println(analogRead(eixo_Y));

//accelerometro
 int x,y,z;  
  adxl.readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z
  //Output x,y,z values 
  //Serial.print("values of X , Y , Z: ");
  //Serial.print(x);
  //Serial.print(" , ");
  //Serial.print(y);
  //Serial.print(" , ");
  //Serial.println(z);
  
  double xyz[3];
  double ax,ay,az;
  adxl.getAcceleration(xyz);
  ax = xyz[0];
  ay = xyz[1];
  az = xyz[2];
  //Serial.print("X=");
  //Serial.print(ax);
  //Serial.println(" g");
  //Serial.print("Y=");
  //Serial.print(ay);
  //Serial.println(" g");
  //Serial.print("Z=");
  //Serial.print(az);
  //Serial.println(" g");
  //Serial.println("**********************");

  valor_ax_anterior = valor_ax_atual;
  valor_ax_atual = x;
  diferenca_ax = valor_ax_atual - valor_ax_anterior;
  //Serial.print ("diferenca_ax=");
  //Serial.println (diferenca_ax);
  
  if (diferenca_ax > 150 || diferenca_ax < -150)
  {
    mqtt.publish("/tcc/acelerometro_x", "possível colisão");
    //Serial.println ("possível colisão");
  }

  valor_ay_anterior = valor_ay_atual;
  valor_ay_atual = y;
  diferenca_ay = valor_ay_atual - valor_ay_anterior;
  //Serial.print ("diferenca_ay=");
  //Serial.println (diferenca_ay);
   
  if (diferenca_ay > 150 || diferenca_ay < -150)
  {
    mqtt.publish("/tcc/acelerometro_y", "possível colisão");
    //Serial.println ("possível colisão");
  }

  valor_az_anterior = valor_az_atual;
  valor_az_atual = z;
  diferenca_az = valor_az_atual - valor_az_anterior;
  //Serial.print ("diferenca_az=");
  //Serial.println (diferenca_az);
  
  if (diferenca_az > 150 || diferenca_az < -150)
  {
    mqtt.publish("/tcc/acelerometro_z", "possível colisão");
    //Serial.println ("possível colisão");
  }

  //gps
  displayInfo();
  
  velocidade = gps.speed.value();
  sprintf(velocidade_texto, "%d", velocidade);
  Serial.println (velocidade);
  Serial.println (velocidade_texto);
  //Serial.println("*******");
  mqtt.publish("/tcc/velocidade_gps", velocidade_texto);
   if (velocidade > 10)
  {
    mqtt.publish("/tcc/velocidade_gps1", "diminuir velocidade");
    Serial.println (" diminuir velocidade");
  }
  
delay(1000);  
mqtt.loop();
}

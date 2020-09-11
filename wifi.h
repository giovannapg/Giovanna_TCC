
#ifndef __WIFI_H__
#define __WIFI_H__

#include <WiFi.h>
#include <PubSubClient.h>

const char* SSID = "HOME_DSK";
const char* PASSWORD = "probesserv";
const char* ID_MQTT = "cadeira01";
const char* BROKER_MQTT = "broker.hivemq.com";
const int BROKER_PORT = 1883;

WiFiClient espClient;
PubSubClient mqtt(espClient);

char topicSensors[10];
char topicSensor1[10];
char topicStatusSensor1[10];
char topicGPSStatus[10];
char topicWIFIStatus[10];
char topicMQTTStatus[10];
char topicSats[10];
char topicGPSLoc[20];
char topicDMS[20];

void wifi_connect(void);
void initMQTT(void);
void connectMQTT(void);
void mqttCallback(char* topic, byte* payload, unsigned int length);
void reconectWiFi();
void checkConnections(void);

#endif

#ifndef main_h
#define main_h
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <MQTT.h>
void setRelay(bool status);
void ketikaAdaPesanDatang(String &topic, String &data);
void wifiKuConnect();
void iotKuConnect();

#endif
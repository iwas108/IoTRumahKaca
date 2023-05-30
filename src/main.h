#ifndef main_h
#define main_h
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiMulti.h>
#include <MQTT.h>

#define PIN_RELAY 33
void kirimStatusRelay();
void setRelay(bool status);
void ketikaAdaPesanDatang(String &topic, String &data);
void wifiKuConnect();
void iotKuConnect();

#endif
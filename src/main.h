#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiMulti.h>
#include <MQTT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define PIN_SENSOR_SUHU 4

void wifiKuConnect();
void iotKuConnect();
void ketikaAdaPesanDatang(String &topic, String &data);
void bacaSuhu();
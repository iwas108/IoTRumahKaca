#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <MQTT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// GPIO where the DS18B20 is connected to
#define PIN_SENSOR_SUHU 4     

WiFiMulti wifiKu;
WiFiClient netKu;
MQTTClient iotKu;

OneWire jalurData1Wire(PIN_SENSOR_SUHU);
DallasTemperature sensorSuhu(&jalurData1Wire);

void ketikaAdaPesanDatang(String &topic, String &data){
  Serial.println("Ada pesan masuk di " + topic + " isinya: " + data);

  if(topic == "undiknas/ti/sensor/suhu/1"){
    float suhu = data.toFloat();
    if(suhu >= 30){
      Serial.println("Suhu panas: "+data+"C. Aktifkan pendingin!");
    }
  }
}

void iotKuConnect(){
  while(!iotKu.connect("undiknas-3127893", "undiknas", "Und1kn45")){
    Serial.print(".");
  }

  Serial.println("Mensubscribe channel");
  iotKu.subscribe("undiknas/ti/sensor/#");
  iotKu.subscribe("undiknas/ti/aktuator/#");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  wifiKu.addAP("LAB TI", "#tiundiknas");

  sensorSuhu.begin();

  Serial.println("Mencoba untuk menghubungkan ke wifi.");
  if( wifiKu.run() == WL_CONNECTED ){
    Serial.println("Wifi berhasil terhubung.");
    Serial.print("Nama wifi: ");
    Serial.print(WiFi.SSID());
    Serial.print(". Alamat IP:");
    Serial.println(WiFi.localIP());
  }else{
    Serial.println("Wifi gagal terhubung.");
  }

  iotKu.begin("mqtt.undiknas.ac.id", 1883, netKu);
  iotKu.onMessage(ketikaAdaPesanDatang);
  iotKuConnect();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(wifiKu.run() != WL_CONNECTED){
  }

  if(!iotKu.connected()){
    iotKuConnect();
  }

  iotKu.loop();
  delay(10); //delay 10 milidetik biar tidak capek keliling

  Serial.print(sensorSuhu.getTempCByIndex(0));
  Serial.println(" ºC");
}



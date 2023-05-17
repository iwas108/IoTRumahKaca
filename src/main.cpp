#include "main.h"

WiFiMulti wifiKu;
WiFiClient netKu;
MQTTClient iotKu;

#define PIN_RELAY 33

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  wifiKuConnect();
  iotKuConnect();
}

void loop() {
  // put your main code here, to run repeatedly:
  wifiKu.run();
  iotKuConnect();
  iotKu.loop();
}

void setRelay(bool status){
  pinMode(PIN_RELAY, OUTPUT);
  Serial.print("Relay diset ke: ");
  Serial.println(status);
  digitalWrite(PIN_RELAY, status);
}

void ketikaAdaPesanDatang(String &topic, String &data){
  Serial.println("Ada pesan masuk di " + topic + " isinya: " + data);

  if(topic == "undiknas/ti/sensor/suhu/1"){
    float suhu = data.toFloat();
    if(suhu >= 30){
      Serial.println("Suhu panas: "+data+"°C. Aktifkan pendingin!");
      setRelay(1);
    }
    else{
      Serial.println("Suhu normal: "+data+"°C. Matikan pendingin!");
      setRelay(0);
    }
  }
}

void wifiKuConnect(){
  wifiKu.addAP("LAB TI", "#tiundiknas");
  Serial.print("Menghubungkan ke wifi");
  while( wifiKu.run() != WL_CONNECTED ){
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi terhubung!");
}

void iotKuConnect(){
  if(!iotKu.connected()){
    iotKu.begin("mqtt.undiknas.ac.id", 1883, netKu);
    iotKu.onMessage(ketikaAdaPesanDatang);
    Serial.print("Menghubung ke broker");
    String idAcak = "joss" + String(random(1000, 1000000));
    while(!iotKu.connect(idAcak.c_str(), "undiknas", "Und1kn45")){
      Serial.print(".");
    }
    Serial.println("");
    iotKu.subscribe("undiknas/ti/sensor/suhu/1");
    Serial.println("IoT terhubung!");
  }
}
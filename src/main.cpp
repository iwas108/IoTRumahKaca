#include "main.h"

WiFiMulti wifiKu;
WiFiClient netKu;
MQTTClient iotKu;

const char* topicPublish = "undiknas/ti/sensor/suhu/1";
const char* topicSubscribe = "undiknas/ti/aktuator/suhu/1";

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
  kirimStatusRelay();
}

void setRelay(bool status){
  pinMode(PIN_RELAY, OUTPUT);
  Serial.print("Relay diset ke: ");
  Serial.println(status);
  digitalWrite(PIN_RELAY, status);
}

void ketikaAdaPesanDatang(String &topic, String &data){
  Serial.println("Ada pesan masuk di " + topic + " isinya: " + data);

  if(topic == topicSubscribe){
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
    iotKu.subscribe(topicSubscribe);
    Serial.println("IoT terhubung!");
  }
}

unsigned long timer_kirimStatusRelay = millis();
void kirimStatusRelay(){
  unsigned long now = millis();
  if( (now - timer_kirimStatusRelay) > 1000 ){
    iotKu.publish(topicPublish, String(digitalRead(PIN_RELAY)));
    timer_kirimStatusRelay = now;
  }
}
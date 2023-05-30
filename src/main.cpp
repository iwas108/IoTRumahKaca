#include "main.h"

WiFiMulti wifiKu;
WiFiClientSecure netKu;
MQTTClient iotKu;

const char* topicPublish = "undiknas/ti/aktuator/kipas/1";
const char* topicSubscribe = "undiknas/ti/sensor/suhu/1";
const char* topicAmbangBatas = "undiknas/ti/aktuator/kipas/1/ambang-batas";
bool relayON = 0;
float ambangBatas = 30;

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
  Serial.println(status == relayON ? "ON" : "OFF");
  digitalWrite(PIN_RELAY, status);
}

void ketikaAdaPesanDatang(String &topic, String &data){
  Serial.println("Ada pesan masuk di " + topic + " isinya: " + data);

  if(topic == topicSubscribe){
    float suhu = data.toFloat();
    if(suhu >= ambangBatas){
      Serial.println("Suhu panas: "+data+"°C. Aktifkan pendingin!");
      setRelay(relayON);
    }
    else{
      Serial.println("Suhu normal: "+data+"°C. Matikan pendingin!");
      setRelay(!relayON);
    }
  }
  else if(topic == topicAmbangBatas){
    ambangBatas = (float) data.toFloat();
  }
}

void wifiKuConnect(){
  wifiKu.addAP("Wokwi-GUEST", "");
  Serial.print("Menghubungkan ke wifi");
  while( wifiKu.run() != WL_CONNECTED ){
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi terhubung!");
}

void iotKuConnect(){
  if(!iotKu.connected()){
    netKu.setInsecure();
    iotKu.begin("2e8aeb43a4694b348a69cc8a4d4bf39a.s2.eu.hivemq.cloud", 8883, netKu);
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
    iotKu.publish(topicPublish, digitalRead(PIN_RELAY) == relayON ? "ON" : "OFF");
    timer_kirimStatusRelay = now;
  }
}
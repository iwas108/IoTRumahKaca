#include "main.h"

WiFiMulti wifiKu;
WiFiClient netKu;
MQTTClient iotKu;

OneWire jalurData1Wire(PIN_SENSOR_SUHU);
DallasTemperature sensorSuhu(&jalurData1Wire);

const char* topicPublish = "undiknas/ti/sensor/suhu/1";
const char* topicSubscribe = "undiknas/ti/aktuator/suhu/1";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  sensorSuhu.begin();

  wifiKuConnect();
  iotKuConnect();
}

void loop() {
  // put your main code here, to run repeatedly:
  wifiKu.run();
  iotKu.loop();
  bacaSuhu();
}

void ketikaAdaPesanDatang(String &topic, String &data){
  Serial.println("Ada pesan masuk di " + topic + " isinya: " + data);

  if(topic == "undiknas/ti/sensor/suhu/1"){
    float suhu = data.toFloat();
    if(suhu >= 30){
      Serial.println("Suhu panas: "+data+"C. Aktifkan pendingin!");
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
    while(!iotKu.connect("undiknas-3127893", "undiknas", "Und1kn45")){
      Serial.print(".");
    }
    Serial.println("");
    iotKu.subscribe(topicSubscribe);
    Serial.println("IoT terhubung!");
  }
}

unsigned long timer_bacaSuhu = millis();
float bacaSuhu(){
  unsigned long now = millis();
  if( (now - timer_bacaSuhu) > 1000 ){
    sensorSuhu.requestTemperatures();
    float celcius = sensorSuhu.getTempCByIndex(0);
    Serial.print("Mengirim data suhu ke ");
    Serial.print(topicPublish);
    Serial.print(" dengan nilai ");
    Serial.print(celcius);
    Serial.println("°C");
    iotKu.publish(topicPublish, String(celcius));
    timer_bacaSuhu = now;
    return celcius;
  }
}


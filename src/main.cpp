#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <MQTT.h>

WiFiMulti wifiKu;
WiFiClient netKu;
MQTTClient iotKu;

#define PIN_LM35 A0
#define vRef 3.30
#define ADC_Resolution 4095
#define LM35_Per_Degree_Volt 0.01
#define Zero_Deg_ADC_Value 879.00

float bacaSensor(){
  float _temperature, temp_val, ADC_Per_Degree_Val;
  int temp_adc_val;
  ADC_Per_Degree_Val = (ADC_Resolution/vRef)*LM35_Per_Degree_Volt;
  for (int i = 0; i < 10; i++) {
    temp_adc_val += analogRead(PIN_LM35);  /* Read ADC value */
    delay(10);
  }
  temp_adc_val = temp_adc_val/10.0;

  temp_adc_val = temp_adc_val - Zero_Deg_ADC_Value;
  temp_adc_val=(temp_adc_val/ADC_Per_Degree_Val);
  /*Serial.print("LM35 Temperature = ");
  Serial.print(temp_adc_val);       
  Serial.print("°C\n");*/
  return temp_adc_val;
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

void iotKuConnect(){
  Serial.println("Menghubung ke broker");
  while(!iotKu.connect("undiknas-3127893")){
    Serial.print(".");
  }
  Serial.println("IoT nyambung!");

  Serial.println("Mensubscribe channel");
  iotKu.subscribe("undiknas/ti/sensor/#");
  iotKu.subscribe("undiknas/ti/aktuator/#");
  Serial.println("IoT done! Siap mainkan.");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Menyetel password wifi.");
  wifiKu.addAP("LAB TI", "#tiundiknas");

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

  iotKu.begin("test.mosquitto.org", 1883, netKu);
  iotKu.onMessage(ketikaAdaPesanDatang);
  iotKuConnect();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(wifiKu.run() != WL_CONNECTED){
    Serial.println("Wifi terputus, telah dicoba untuk menghubung ulang.");
  }

  if(!iotKu.connected()){
    Serial.println("IoTnya putus, mencoba menghubung ulang.");
    iotKuConnect();
  }

  iotKu.loop();
  delay(10); //delay 10 milidetik biar tidak capek keliling

  Serial.print("LM35 Temperature = ");
  Serial.print(bacaSensor());       
  Serial.print("°C\n");
  delay(1000);
}



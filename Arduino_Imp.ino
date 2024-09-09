#include <SoftwareSerial.h>
#include <dht11.h>
dht11 DHT;

SoftwareSerial mySerial(2, 3);  // RX,TX
SoftwareSerial ArduinoUno(8, 9);

void setup() {
  pinMode(14, OUTPUT); //distance sensor
  pinMode(15, INPUT); //distance sensor

  Serial.begin(9600);
  //ArduinoUno.begin(4800);
  ArduinoUno.begin(4800);
  pinMode(7, OUTPUT);
}

float get_water_level(){
  digitalWrite(14, LOW);
  delayMicroseconds(2);

  digitalWrite(14, HIGH);
  delayMicroseconds(10);
  digitalWrite(14, LOW);
  

  float duration = pulseIn(15, HIGH);

  float distance = (duration * 0.0343) / 2;

  float water_level_percentage = ((12.55 - distance)/12.55) * 100.0;
  
  return water_level_percentage;
}

String get_wheather_temperature_humidity(){
  int chk = DHT.read(16);
  String temperature = String(DHT.temperature);
  delay(100);
  String humidity = String(DHT.humidity);
  delay(100);
  return temperature + "|" + humidity;

}


void getSensorDataDecimalOnGivenAddress(byte* queryData, byte* receivedData, int querySize, int& receivedSize, int maxSize) {
  mySerial.write(queryData, querySize);
  delay(100);
  Serial.println(mySerial.available());
  if (mySerial.available() >= 19) {
    receivedSize = mySerial.available();
    if (receivedSize > 0) {
      for (int i = 0; i < receivedSize; i++) {
        receivedData[i] = mySerial.read();
      }
    }
  } else {
    for (int i = 0; i < maxSize; i++) {
      receivedData[i] = 0;
    }
  }
}

String getAndPrintAllSensorData() {
  ArduinoUno.end();

  mySerial.begin(4800);
  byte queryAllData[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x07, 0x04, 0x08 };
  const int querySize = sizeof(queryAllData);
  const int maxSize = 32;
  byte receivedData[maxSize];
  int receivedSize = 0;
  getSensorDataDecimalOnGivenAddress(queryAllData, receivedData, querySize, receivedSize, maxSize);
  mySerial.end();
  ArduinoUno.begin(4800);

  Serial.print("Received data: ");
  for (int i = 0; i < receivedSize; i++) {
    Serial.print(receivedData[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  float soil_moisture = ((receivedData[8] << 8) | receivedData[9]) / 10.0;
  Serial.print("soil_moisture: ");
  Serial.print(soil_moisture);
  Serial.println("%");
  float soil_temperature = ((receivedData[10] << 8) | receivedData[11]) / 10.0;
  Serial.print("soil_temperature: ");
  Serial.print(soil_temperature);
  Serial.println("℃");
  unsigned int conductivity = receivedData[13];
  Serial.print("conductivity: ");
  Serial.print(conductivity);
  Serial.println(" us/cm");
  float ph = receivedData[15] / 10.0;
  Serial.print("ph: ");
  Serial.println(ph);
  unsigned int nitrogen = receivedData[17];
  Serial.print("nitrogen: ");
  Serial.print(nitrogen);
  Serial.println(" mg /kg");
  unsigned int phosporus = receivedData[19];
  Serial.print("phosporus: ");
  Serial.print(phosporus);
  Serial.println(" mg /kg");
  unsigned int potasium = receivedData[21];
  Serial.print("potasium: ");
  Serial.print(potasium);
  Serial.println(" mg /kg");
  Serial.println("");

  float water_level = get_water_level();
  Serial.print("water_level: ");
  Serial.print(water_level);
  Serial.println("");

  String wheather_temperature_humidity = get_wheather_temperature_humidity();

  return String(soil_moisture) + "|" + String(soil_temperature) + "|" + String(conductivity) + "|" + String(ph) + "|" + String(nitrogen) + "|" + String(phosporus) + "|" + String(potasium)+"|"+String(water_level)+"|" + wheather_temperature_humidity;
}

void loop() {
   
    String nodemcu_data = ArduinoUno.readString();
    Serial.println(nodemcu_data);

    if (nodemcu_data == "Read sensor") {
      String sensorData = getAndPrintAllSensorData();
      Serial.println("");
      Serial.println("");
      Serial.println(sensorData);
      Serial.println("");
      Serial.println("");

      ArduinoUno.print(sensorData);
    }
    else if (nodemcu_data == "Irrigate for 5 seconds!") {
      digitalWrite(7, HIGH);
      delay(3000);
      digitalWrite(7, LOW);
      delay(2000);
      ArduinoUno.print("Irrigation finished");
      Serial.println("Irrigation finished has been transmitted!!!");
    }

  //10, Software Serial protokolü ile yollanacak
  
}

#include <Arduino.h>
#include <ESP8266WiFi.h>  
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <cstring>
#include <iostream>
using namespace websockets;

SoftwareSerial NodeMCU(D6,D7); // RX and TX pins

const char* ssid = "";
const char* password = "";

const char* websocket_server = "ws://....:3005/WebSocketApp/websocketendpoint";  // WebSocket server URL
bool ifWebsocketConnectionEstablished = false;

WebsocketsClient client;

int clientId = 128094712;
const char* plantId = "ImgK7HBzrLhoCm0xoWE2";
//ImgK7HBzrLhoCm0xoWE265
//AysWhz6FHH0Nr8RkCriH

char sensorValues[100];
float soil_moisture;
float soil_temperature;
unsigned int conductivity;
float ph;
unsigned int nitrogen;
unsigned int phosporus;
unsigned int potasium;

float water_level = 200000.9;


void getDataFromArduinoSerial(float& soil_moisture, float& soil_temperature, int& conductivity, float& ph, int& nitrogen, int& phosporus, int& potasium, float& water_level,float& wheather_temperature, float& wheather_humidity);
void sendInitialData(float soil_moisture, float soil_temperature, int conductivity, float ph, int nitrogen, int phosporus, int potasium,float water_level,float wheather_temperature, float wheather_humidity);
void sendPlantData(float soil_moisture, float soil_temperature, int conductivity, float ph, int nitrogen, int phosporus, int potasium,float water_level,float wheather_temperature, float wheather_humidity);
String formatFloat(float value);

void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  NodeMCU.begin(4800);
  WiFi.begin(ssid, password);
  Serial.println();
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(2, LOW);
    delay(1000);
    Serial.print(".");
  }
  Serial.println(" connected.");
  digitalWrite(2, HIGH);

  client.onMessage(onMessageCallback);
  client.onEvent(onEventsCallback);
  client.connect(websocket_server);
}

void loop() {
  client.poll();
  if(!ifWebsocketConnectionEstablished){
    Serial.println("nedednnn");
    delay(1000);
    digitalWrite(2, LOW);
    client.connect(websocket_server);
    delay(1000);
    digitalWrite(2, HIGH);
  }
  else{
    digitalWrite(2, HIGH);
  }

  String nodeMCUString = NodeMCU.readString();
  delay(100);
  Serial.print("From the main: "); 
  Serial.println(nodeMCUString);
  if(nodeMCUString == "Irrigation finished"){
    StaticJsonDocument<200> doc;
    doc["clientId"] = clientId;
    doc["command"] = "Irrigation finished";
    doc["plantId"] = plantId;
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);

    client.send(jsonBuffer);
    nodeMCUString = "";
  }

}

void onMessageCallback(WebsocketsMessage message) {
  Serial.print("Got Message: ");
  Serial.println(message.data());

  if (message.data() == "send sensor data") {
    float soil_moisture = 0.0;
    float soil_temperature = 0.0;
    int conductivity = 0;
    float ph = 0.0;
    int nitrogen = 0;
    int phosporus = 0;
    int potasium = 0;
    float water_level = 0;
    float wheather_temperature = 0;
    float wheather_humidity = 0;
    
    getDataFromArduinoSerial(soil_moisture, soil_temperature, conductivity, ph, nitrogen, phosporus, potasium,water_level,wheather_temperature,wheather_humidity);
    sendPlantData(soil_moisture, soil_temperature, conductivity, ph, nitrogen, phosporus, potasium,water_level,wheather_temperature,wheather_humidity);
    Serial.println("send sensor data has been transmitted!!!");
  }
  else if(message.data()=="Irrigate for 5 seconds!"){
    NodeMCU.print("Irrigate for 5 seconds!");
  }
}

void onEventsCallback(WebsocketsEvent event, String data) {
  if (event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("WebSocket Connection Opened");
    ifWebsocketConnectionEstablished = true;
    digitalWrite(2, HIGH);
    float soil_moisture = 0.0;
    float soil_temperature = 0.0;
    int conductivity = 0;
    float ph = 0.0;
    int nitrogen = 0;
    int phosporus = 0;
    int potasium = 0;
    float water_level = 0;
    float wheather_temperature = 0;
    float wheather_humidity = 0;

    getDataFromArduinoSerial(soil_moisture, soil_temperature, conductivity, ph, nitrogen, phosporus, potasium,water_level,wheather_temperature,wheather_humidity);
    sendInitialData(soil_moisture, soil_temperature, conductivity, ph, nitrogen, phosporus, potasium,water_level,wheather_temperature,wheather_humidity);
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    digitalWrite(2, LOW);
    Serial.println("WebSocket Connection Closed");
    ifWebsocketConnectionEstablished = false;
  } else if (event == WebsocketsEvent::GotPing) {
    Serial.println("Got a Ping!");
  } else if (event == WebsocketsEvent::GotPong) {
    Serial.println("Got a Pong!");
  }
}

void getDataFromArduinoSerial(float& soil_moisture, float& soil_temperature, int& conductivity, float& ph, int& nitrogen, int& phosporus, int& potasium, float& water_level,float& wheather_temperature, float& wheather_humidity){
  NodeMCU.print("Read sensor");
  delay(100); 
  while(true){
    if (NodeMCU.available() > 0) { // if there is data transfer to Arduino
      Serial.println("Data received from the Arduino UNO board!");
      String nodeMCUString = NodeMCU.readString(); // Read the String from Arduino
      Serial.println(nodeMCUString);

      nodeMCUString.toCharArray(sensorValues, 100); // Convert String to char array  

      soil_moisture = atof(strtok(sensorValues, "|"));// soil_moisture (float)
      soil_temperature = atof(strtok(NULL, "|"));// soil_temperature (float)
      conductivity = atoi(strtok(NULL, "|"));// conductivity (integer)
      ph = atof(strtok(NULL, "|"));// ph (float)
      nitrogen = atoi(strtok(NULL, "|"));// nitrogen (inte
      phosporus = atoi(strtok(NULL, "|"));// phosphorus (integer)
      potasium = atoi(strtok(NULL, "|"));    // potasium (integer)
      water_level = atof(strtok(NULL, "|"));
      wheather_temperature = atof(strtok(NULL, "|"));
      wheather_humidity = atof(strtok(NULL, "|"));


      break;
      }
  }
  
  
}

void sendInitialData(float soil_moisture, float soil_temperature, int conductivity, float ph, int nitrogen, int phosporus, int potasium,float water_level,float wheather_temperature, float wheather_humidity){
  StaticJsonDocument<200> doc;
  doc["clientId"] = clientId;
  doc["plantId"] = plantId;
  doc["command"] = "addNewPlantToUser";
  doc["water_level"] = formatFloat(water_level);
  doc["weather_humidity"] = formatFloat(wheather_humidity);
  doc["weather_temperature"] = formatFloat(wheather_temperature);
  doc["humidity"] = formatFloat(soil_moisture);
  doc["soil_moisture"] = formatFloat(soil_moisture);
  doc["temperature"] = formatFloat(soil_temperature); 
  doc["conductivity"] = conductivity; 
  doc["ph"] = formatFloat(ph); 
  doc["nitrogen"] = nitrogen; 
  doc["phosporus"] = phosporus; 
  doc["potasium"] = potasium;

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);

  client.send(jsonBuffer);
  Serial.println("Initial data sent to WebSocket server");
}

void sendPlantData(float soil_moisture, float soil_temperature, int conductivity, float ph, int nitrogen, int phosporus, int potasium,float water_level,float wheather_temperature, float wheather_humidity){
  StaticJsonDocument<200> doc;
  doc["clientId"] = clientId;
  doc["command"] = "updateExistingPlantValuesRealtime";
  doc["plantId"] = plantId;
  doc["water_level"] = formatFloat(water_level);
  doc["weather_humidity"] = formatFloat(wheather_humidity);
  doc["weather_temperature"] = formatFloat(wheather_temperature);
  doc["soil_moisture"] = formatFloat(soil_moisture);
  doc["temperature"] = formatFloat(soil_temperature); 
  doc["conductivity"] = conductivity; 
  doc["ph"] = formatFloat(ph); 
  doc["nitrogen"] = nitrogen; 
  doc["phosporus"] = phosporus; 
  doc["potasium"] = potasium;

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);

  client.send(jsonBuffer);

  Serial.println("Data sent to WebSocket server");
}

String formatFloat(float value) {
  char buffer[10];
  dtostrf(value, 6, 2, buffer);
  return String(buffer);
}

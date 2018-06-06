
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <PubSubClient.h>
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>
#include <stdlib.h>
#include <string.h>


//Location where header files are searched /tools/xtensa-esp32-elf/xtensa-esp32-elf/include/

const char* mqttServer = "m14.cloudmqtt.com";
const int mqttPort = 19831;
const char* mqttUser = "pljkdmik";
const char* mqttPassword = "QfOk84Wjen03";
const int ledPin1 = 14;
const int ledPin2 = 12;
const int ledPin3 = 13;
const int ledPin4 = 15;
//const char* deviceId = "sumit";
//const char* devicePassword = "sumit";
//const char* deviceId = "etsb1737";
//const char* devicePassword = "CU3Mg81PjM";
const char* deviceId = "etsb1738";
const char* devicePassword = "H51eKmRXUv";

bool activationStatus = true;
long lastMsg = 0;
char msg[50];
int value = 0;
const int RED = 5;
const int GREEN = 4;
const int BLUE = 0;


char* concat(const char *s1, const char *s2)
{ 
    char *result = (char*)malloc(strlen(s1)+strlen(s2)+1);//+1 for the null-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
char* subscribe1 = concat(deviceId, "/pin1");
char* subscribe2 = concat(deviceId, "/pin2");
char* subscribe3 = concat(deviceId, "/pin3");
char* subscribe4 = concat(deviceId, "/pin4");

WiFiClient espClient;
PubSubClient client(espClient);

void setRed() {
  analogWrite(RED, 1023);
  analogWrite(GREEN, -10);
  analogWrite(BLUE, -10); 
}

void setBlue() {
  analogWrite(RED, -10);
  analogWrite(GREEN, -10);
  analogWrite(BLUE, 1023); 
}

void setGreen(){
  analogWrite(RED, -10);
  analogWrite(GREEN, 1023);
  analogWrite(BLUE, -10); 
}


void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    ESP.eraseConfig();
    pinMode(ledPin1, OUTPUT);
    pinMode(ledPin2, OUTPUT);
    pinMode(ledPin3, OUTPUT);
    pinMode(ledPin4, OUTPUT);
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
    setRed();
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    /*digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, HIGH);
    digitalWrite(ledPin4, HIGH);
    */
    //reset saved settings
    //wifiManager.resetSettings();
    
    //set custom ip for portal
    //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect(deviceId, devicePassword);
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
 
  
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
   
      client.subscribe(subscribe1);
      client.subscribe(subscribe2);
      client.subscribe(subscribe3);
      client.subscribe(subscribe4);
      client.subscribe(deviceId);
 
    } else {
 
      Serial.print("failed with state");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
    //if you get here you have connected to the WiFi
     Serial.println("connected...yeey :)");
         client.subscribe("esp/test");

     
     //client.subscribe(deviceId);
     
     
     
     
     
     
}


void reconnect() {
  setRed();
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
      
      Serial.println("connected");
      client.setServer(mqttServer, mqttPort);
      client.setCallback(callback);
      client.subscribe(deviceId);
       client.subscribe("esp/test");
       client.subscribe(subscribe1);
      client.subscribe(subscribe2);
      client.subscribe(subscribe3);
      client.subscribe(subscribe4);
      client.subscribe(deviceId);
 
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void loop(){

    while (activationStatus){
      setBlue(); 
      if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    
    
    Serial.print("Publish message: ");
    Serial.println("Activated");
    client.publish(deviceId, "Activated");
    client.subscribe("esp/test");
}
      
     }

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  setGreen();
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    
    
    Serial.print("Publish message: ");
    Serial.println("I am alive");
    client.publish(deviceId, "I am alive");
    client.subscribe("esp/test");
}
    
    
     
    
    
  
}

void pinChanger(String messageTemp,int pinNumber){
  setBlue();
  if(messageTemp == "on"){
        
        Serial.print("On");
     
        digitalWrite(pinNumber, HIGH);
      }
      else if (messageTemp == "off"){
        
        Serial.print("OFF");
     
        digitalWrite(pinNumber, LOW);
      }

  
}

void callback(String topic, byte* message, unsigned int length) {
  setBlue();
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print("Message:");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  

  
  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic home/office/esp1/gpio2, you check if the message is either 1 or 0. Turns the ESP GPIO according to the message
  if(topic==subscribe1){
      Serial.print("Changing pin1 to ");
      pinChanger(messageTemp,ledPin1);
      
      
      
  }
  if(topic==subscribe2){
      Serial.print("Changing pin2 to ");
      pinChanger(messageTemp,ledPin2);
  }

if(topic==subscribe3){
     Serial.print("Changing pin3 to ");
      pinChanger(messageTemp,ledPin3);
     
  }

if(topic==subscribe4){
      Serial.print("Changing pin4 to ");
      pinChanger(messageTemp,ledPin4);
  }
  
  if(topic == deviceId){
    if (messageTemp == "goAhead"){
      activationStatus = false;
    }
    if (messageTemp == "restart"){
      ESP.restart();
    }
   
  }
  Serial.println();
  

  //println((String)methodName);
 /*
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  char* pay = "";
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    
    Serial.println((char)payload[i]);

  }
   
  Serial.println();
  Serial.println("-----------------------");
  
  if ((char*)payload == (char*)(byte*)'OFF') {
        Serial.println("IN OFF");
        //digitalWrite(ledPin, LOW);
  }
  if ((char*)payload == (char*)'ON') {
        
        digitalWrite(ledPin, HIGH);
  }
  */
}
 


#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

const byte sensorPin = 35;
const byte buzzerPin = 36;
 
int freq = 2000;
int channel = 0;
int resolution = 8;
int dutyCycle = 128;


// WiFi network name and password:
const char * networkName = "TheB3st$$1";
const char * networkPswd = "l0ck0N3$$1";

// Udp Address
const IPAddress udpAddress(192, 168, 1, 26);
const int udpPort = 8010;

//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udp;
 
SemaphoreHandle_t syncSemaphore;
 
void IRAM_ATTR handleInterrupt() {
  xSemaphoreGiveFromISR(syncSemaphore, NULL);
}
 
void setup() {
 
  Serial.begin(115200);
  connectToWiFi(networkName, networkPswd);
  syncSemaphore = xSemaphoreCreateBinary();
 
  pinMode(sensorPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(sensorPin), handleInterrupt, CHANGE);
 
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(buzzerPin, channel);
 
}
 
void loop() {
 
    xSemaphoreTake(syncSemaphore, portMAX_DELAY);
 
    if(digitalRead(sensorPin)){
 
      Serial.println("Motion detected");
      sendOSC();
      ledcWrite(channel, dutyCycle);
 
    }else{
 
      Serial.println("Motion stoped");
      ledcWrite(channel, 0);
 
    }
 
}

void connectToWiFi(const char * ssid, const char * pwd){
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);
  
  //Initiate connection
  WiFi.begin(ssid, pwd);

  Serial.println("Waiting for WIFI connection...");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP:
          //When connected set 
          Serial.print("WiFi connected! IP address: ");
          Serial.println(WiFi.localIP());  
          //initializes the UDP state
          //This initializes the transfer buffer
          udp.begin(WiFi.localIP(),udpPort);
          connected = true;
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          connected = false;
          break;
    }
}

void sendOSC(){

   int input = 1;
   const IPAddress udpAddress(192, 168, 1, 26);
const int udpPort = 8010;
     OSCMessage msg("/medias/Gradient_Color/assign");
     Serial.println("OSCMessage sent");
     msg.add((unsigned int) input);
    udp.beginPacket(udpAddress, udpPort);
    msg.send(udp);
    udp.endPacket();
    msg.empty();
  
}

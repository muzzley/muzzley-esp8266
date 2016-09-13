#include <DNSServer.h>
#include <FS.h>
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <MQTTClient.h>
#include <SPI.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "ESP8266SSDP.h" // for muzzley local discovery
#include "Data.h"
#include "SupportFunctions.h"
#include "MuzzleyRegister.h"
#include <ESP8266HTTPClient.h>
#include "Muzzley.h"


int reset_counter = 0;
String profile = "57d27f37f8073d99a7387f61";

WiFiClientSecure net;
MQTTClient client;
WiFiManager wifiManager;
Muzzley muzzley;

Data data;
MuzzleyRegister muzzleyconf;
SupportFunctions sf;

// ********************* Declaring functions ****************************
void set_status(bool status);
// ********************* WiFi, status and first setup *******************

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
}

void connect(); 

void setup() {
  Serial.begin(115200);
  
  SPIFFS.begin();

  // format the SPIFS
  //Serial.println("formating..."); SPIFFS.format(); Serial.println("formated"); delay(50000);

  //WiFiManager
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("MUZZLEY-DEMO")) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

 
  delay(200);
  // init Muzzley DeviceKey and SerialNumber
  String deviceKey; 
  uint8_t mac[6];
  WiFi.macAddress(mac);
  String serialNumber = sf.macToStr(mac);

  muzzleyconf.getDeviceKey(&deviceKey, &serialNumber);
  if (deviceKey == ""){
    Serial.println("D: 1001");
      
      HTTPClient httpclient;
      httpclient.begin("http://global-manager.muzzley.com/deviceapp/register");
    
      httpclient.POST("{\"profileId\": \"" + profile + "\",\"serialNumber\": \"" + serialNumber + "\"}");
      
      muzzleyconf.registerDeviceKey(serialNumber, httpclient.getString(), &deviceKey);
      httpclient.end();
      ESP.restart();
  }else{
    // found device Key
    Serial.println("Found device key");
  }
 
  // initialize SSD discovery
  muzzley.init_local_discovery(profile, serialNumber, mac, deviceKey);
  muzzley.init_web_server();




  client.begin("geoplatform.muzzley.com", 8883, net); // MQTT brokers usually use port 8883 for secure connections
  //connect();
  
  delay (200);

}



void connect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("\nAttempting MQTT connection...");
    delay (10); 
    if (client.connect("ESP8266Client", "b12f0806-8258-4706-88f4-def89b99a25b", "eae457ab9386babd")) {
      Serial.println("connected");

      uint8_t mac[6];
      WiFi.macAddress(mac);
      String deviceKey;
      String serialNumber = sf.macToStr(mac);
      muzzleyconf.getDeviceKey(&deviceKey, &serialNumber);
      Serial.println(deviceKey);
      
      client.subscribe("v1/iot/profiles/" + profile + "/channels/" + deviceKey + "/#");
    } else {
      Serial.print("failed, rc=");
      Serial.println(" try again in 5 seconds");
      delay(1000);
    }
  }
}


void loop() {
  client.loop();
  delay(20);
  
  if (!client.connected()) {
    connect();
  }

  // handle http server - used for UPNP discovery XML provider
  muzzley.handle_httpserver();
}


void messageReceived(String topic, String payload, char * bytes, unsigned int length) {
  Serial.print("\nIncoming message: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.print(payload);
  Serial.println();

  String properties = sf.getValue(topic, '/', 9);

  // parse json payload
  StaticJsonBuffer<400> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(payload);

  // check if the following operations are actions to perform on the device
  if ((strcmp (root["io"],"w") == 0)){
  
    // Set status (on/off)
    if ((properties == "status")){
      bool status = root["data"]["value"];
      set_status(status);
    }
  } // end of Muzzley IO Write actions
  
}


void set_status(bool status){
 pinMode(5,OUTPUT);
 if (status==true)
     digitalWrite(5,HIGH);
 if (status==false)
    digitalWrite(5,LOW);
}
















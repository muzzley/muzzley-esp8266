#include "config.h"
#include <DNSServer.h>
#include <FS.h>
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "ESP8266SSDP.h" // for muzzley local discovery
#include "Data.h"
#include "SupportFunctions.h"
#include "MuzzleyRegister.h"
#include "Muzzley.h"
#include <MQTTClient.h>



int reset_counter = 0;

WiFiClientSecure net;
WiFiManager wifiManager;
Muzzley muzzley;
Data data;
MuzzleyRegister muzzleyconf;
SupportFunctions sf;
MQTTClient client;

void set_status(bool status);

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

  //SPIFFS.format(); // for testing
  
  //WiFiManager
  WiFiManager wifiManager;
  //wifiManager.resetSettings(); // reset settings for testing

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  // try to connect. If not available open hotspot mode.
  if (!wifiManager.autoConnect(wifi_hotspot_ssid)) {
    Serial.println("failed to connect and hit timeout");
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
   Serial.println("My DeviceKey after:" + deviceKey + "\n");
  
  // initialize SSD discovery
  muzzley.init_local_discovery(profile, serialNumber, mac, deviceKey);
  muzzley.init_web_server();

  client.begin(muzzley_host, muzzley_port, net); // MQTT brokers usually use port 8883 for secure connections
  //connect();
  
  delay (200);

}






void loop() {
  client.loop();
  delay(20);
  
  if (!client.connected()) {
     
     while (!client.connected()) {
      Serial.print("\nAttempting MQTT connection...");
      delay (10); 
      if (client.connect("ESP8266Client", muzzley_uuid, muzzley_app_token)) {
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
















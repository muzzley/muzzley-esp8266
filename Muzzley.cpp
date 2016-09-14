#include <Arduino.h>
#include "Muzzley.h"
#include <FS.h>
#include <ArduinoJson.h>
#include "ESP8266SSDP.h" // for muzzley local discovery
#include "SupportFunctions.h"
#include <ESP8266WebServer.h>
#include "MuzzleyRegister.h"
#include <MQTTClient.h>
#include "config.h"
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

SupportFunctions sfc;
ESP8266WebServer HTTP(80);
MuzzleyRegister muzzleyconfig;

Muzzley::Muzzley()
{
}


// ************** SSDP METHODS ***************** //
void Muzzley::init_local_discovery(String profile, String serialNumber, uint8_t mac[6], String deviceKey) {
  // initialize SSD discovery
  //Serial.println("Starting SSDP...\n");
  SSDP.setDeviceType("urn:Muzzley:device:" + profile + ":1");
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort(80);
  SSDP.setName("Muzzley");
  SSDP.setSerialNumber(serialNumber);
  SSDP.setMACAddress(sfc.macToStr(mac));
  SSDP.setURL("index.html");
  SSDP.setModelName("Muzzley");
  SSDP.setModelURL("http://www.muzzley.com");
  SSDP.setManufacturer("Muzzley");
  SSDP.setManufacturerURL("http://muzzley.com");
  SSDP.setDeviceKey(deviceKey);
  SSDP.begin();
}

void Muzzley::init_web_server(){
  // initiliaze web server
  HTTP.on("/index.html", HTTP_GET, [](){
    Serial.println("HTTP index.html request");
    HTTP.send(200, "text/plain", "Future Muzzley configuration tool!");
  });
  HTTP.on("/description.xml", HTTP_GET, [](){
    //Serial.println("\ndescription.xml requested");
    SSDP.schema(HTTP.client());
  });
  HTTP.begin();
  
}

void Muzzley::handle_httpserver(){
  HTTP.handleClient();
}


// ** MUZZLEY MQTT CONNECTION ** 
void Muzzley::connect(MQTTClient mqttclient, WiFiClientSecure net) {
  Serial.println("Entered Muzzley::Connect");
  mqttclient.begin(muzzley_host, muzzley_port, net); // MQTT brokers usually use port 8883 for secure connections

  while (!mqttclient.connected()) {
    Serial.print("\nAttempting MQTT connection...");
    delay (10); 
    if (mqttclient.connect("ESP8266Client", muzzley_uuid, muzzley_app_token)) {
      Serial.println("connected");

      uint8_t mac[6];
      WiFi.macAddress(mac);
      String deviceKey;
      String serialNumber = sfc.macToStr(mac);
      muzzleyconfig.getDeviceKey(&deviceKey, &serialNumber);
      Serial.println(deviceKey);
      
      mqttclient.subscribe("v1/iot/profiles/" + profile + "/channels/" + deviceKey + "/#");
    } else {
      Serial.print("failed, rc=");
      Serial.println(" try again in 5 seconds");
      delay(1000);
    }
  }
}





#include <Arduino.h>
#include "Muzzley.h"
#include <FS.h>
#include <ArduinoJson.h>
#include "ESP8266SSDP.h" // for muzzley local discovery
#include "SupportFunctions.h"
#include <ESP8266WebServer.h>

SupportFunctions sfc;
ESP8266WebServer HTTP(80);

Muzzley::Muzzley()
{
}

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



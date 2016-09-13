#include <Arduino.h>
#include "Data.h"
#include <FS.h>
#include <ArduinoJson.h>

Data::Data()
{

}


void Data::printconfig() {

    if (SPIFFS.exists("/bulbdata.json")) {
      File configFile = SPIFFS.open("/bulbdata.json", "r");

      while (configFile.available()) {
        //Lets read line by line from the file
        String line = configFile.readString();
        Serial.println(line);
      }
      configFile.close();
    } else {
      Serial.println("printconfig: failed to open bulbdata.json");
    }

 
  //end read
}


void Data::mountbulbdata() {
  //clean FS, for testing
  //SPIFFS.format();

  //read configuration from FS json
  //Serial.println("mounting Buµlb FS...");


    //Serial.println("  . starting");
    if (SPIFFS.exists("/bulbdata.json")) {
      File configFile = SPIFFS.open("/bulbdata.json", "r");

      printconfig();

      if (configFile) {
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        if (json.success()) {
          //Serial.println("\nbulbdata parsed json");
        } else {
          Serial.println("failed to load bulb data file");
        }
      }
    }

  //end read
}

String Data::get_config() {
  if (SPIFFS.exists("/bulbdata.json")) {
    File configFile = SPIFFS.open("/bulbdata.json", "r");

    if (configFile) {
      size_t size = configFile.size();
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);

      configFile.readBytes(buf.get(), size);
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.parseObject(buf.get());
      //json.printTo(Serial);
      if (json.success()) {
        //Serial.println("\nbulbdata parsed json");
        String tmpjson;
        json.printTo(tmpjson);
        configFile.close();
        return tmpjson;
      } else {
        Serial.println("Data::get_config(): failed to parse bulb data file");
        return "";
      }
    }
  } else {
    Serial.println("Data::get_config(): File does not exist");
    return "";
  }
}



void Data::save_config(JsonObject& root)
{
  File configFile = SPIFFS.open("/bulbdata.json", "w");

  if (!configFile) {
    Serial.println("failed to open config file for writing");
  }

  root.printTo(configFile);
  configFile.close();
}




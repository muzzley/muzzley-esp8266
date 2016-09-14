#ifndef Muzzley_h
#define Muzzley_h

#include <ESP8266WiFi.h>
#include <MQTTClient.h>


class Muzzley
{
  public:
    Muzzley();
    void init_local_discovery(String profile, String serialNumber, uint8_t mac[6], String deviceKey);
    void init_web_server();
    void handle_httpserver();
    void connect(MQTTClient mqttclient, WiFiClientSecure net);
    void loop();
    bool connected();
  private:
};

#endif


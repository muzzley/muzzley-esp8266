#ifndef Muzzley_h
#define Muzzley_h

class Muzzley
{
  public:
    Muzzley();
    void init_local_discovery(String profile, String serialNumber, uint8_t mac[6], String deviceKey);
    void init_web_server();
    void handle_httpserver();
  private:
};

#endif


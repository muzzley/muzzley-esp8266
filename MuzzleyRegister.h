#ifndef MuzzleyRegister_h
#define MuzzleyRegister_h

class MuzzleyRegister
{
  public:
    MuzzleyRegister();
    void getDeviceKey(String *d1, String *d2);
    void SaveDeviceKeyToLocal(String serialNumber, String payload, String *deviceKey);
    void save_config(String deviceKey, String serialNumber);
    String request_devicekey(String serialNumber);
    

  private:

};

#endif


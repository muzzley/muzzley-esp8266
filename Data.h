#ifndef Data_h
#define Data_h

#include <ArduinoJson.h>


class Data
{
  public:
    Data();
    void printconfig();
    void save_config(JsonObject& root);
    void mountbulbdata();
    String get_config();
  private:
};

#endif


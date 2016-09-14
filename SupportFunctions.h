#ifndef SupportFunctions_h
#define SupportFunctions_h

class SupportFunctions
{
  public:
    SupportFunctions();
    String getValue(String data, char separator, int index);
    String macToStr(const uint8_t* mac);
  private:
};

#endif


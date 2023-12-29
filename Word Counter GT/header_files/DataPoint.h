#ifndef DATA_POINT
#define DATA_POINT

#include <string>

class DataPoint {
public:
  DataPoint(std::string labl, int val) : label(labl), value(val){};

  const std::string getLabel() { return label; }
  const int getValue() { return value; }

private:
  std::string label;
  int value;
};

#endif
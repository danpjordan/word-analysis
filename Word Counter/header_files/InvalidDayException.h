#ifndef INVALID_DAY_EXCEPTION
#define INVALID_DAY_EXCEPTION

#include <string>

class InvalidDayException : std::exception {
public:
  InvalidDayException(int day, std::string funct) : dayOfException(day), function(funct) {}

  int getDayOfException() { return dayOfException; }
  std::string getFunction() { return function; }

  std::string data() {
    return "Day: " + std::to_string(dayOfException) + "\nFunction: " + function +
           "\n";
  }

  const char *what() const noexcept override {
    return "Exception thrown: InvalidDay\n";
  }

private:
  int dayOfException;
  std::string function;
};

#endif
#ifndef WORD_DATA_SET
#define WORD_DATA_SET

#include "DataPoint.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

class WordDataSet {
public:
  WordDataSet(std::map<int, std::vector<std::string>> dMap, std::string target,
              char pLength, int numPeriods);

  WordDataSet(std::map<int, std::vector<std::string>> dMap, std::string target,
              char pLength);
  void print();
  std::string getData();
  char getPeriodLength() { return periodLength; }
  std::string getTarget() { return target; }
  int getNumberOfPeriods() { return numberOfPeriods; }

  std::vector<DataPoint> getDataPoints() {return dataPoints;}

  static std::vector<DataPoint>
  generateDataPoints(const std::map<int, std::vector<std::string>> &dayMap,
                     const std::string &target, const char periodLength,
                     const int numberOfPeriods);
  static int
  countOccurence(const std::map<int, std::vector<std::string>> &dayMap,
                 const std::string &target, const int start, const int end);

  static bool strcmpIgnoreCase(const std::string &a, const std::string &b);

  static bool
  preTargetCheck(const std::vector<std::string> &vec,
                 const std::vector<std::vector<std::string>> &pretargetWords,
                 const int index);

  static bool
  postTargetCheck(const std::vector<std::string> &vec,
                  const std::vector<std::vector<std::string>> &posttargetWords,
                  int index, int offset);

  static bool targetCheck(const std::vector<std::string> &vec,
                          const std::vector<std::string> &targetWords,
                          int targetIndex);

  static std::string generateLabel(int startDay, int endDay, char periodLength);

  static int getEndOfMonth(int startDay, int numMonths);

  static int getDateFromDay(int day);
  static int getMonthFromDay(int day);

private:
  std::vector<DataPoint> dataPoints;
  std::map<int, std::vector<std::string>> dayMap;
  std::string target;
  int numberOfPeriods;
  char periodLength;
};

#endif
#include "header_files/WordDataSet.h"
#include <algorithm>
#include <exception>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <chrono>
#include <vector>

WordDataSet::WordDataSet(std::map<int, std::vector<std::string>> dMap,
                         std::string trgt, char pLength, int numPeriods)
    : dayMap(dMap), target(trgt), periodLength(pLength),
      numberOfPeriods(numPeriods) {
  dataPoints =
      generateDataPoints(dayMap, target, periodLength, numberOfPeriods);
}

WordDataSet::WordDataSet(std::map<int, std::vector<std::string>> dMap,
                         std::string trgt, char pLength)
    : dayMap(dMap), target(trgt), periodLength(pLength), numberOfPeriods(1) {
  dataPoints =
      generateDataPoints(dayMap, target, periodLength, numberOfPeriods);
}

int WordDataSet::getEndOfMonth(int startDay, int numMonths) {
  int curMonth = getMonthFromDay(startDay);
  int dayPointer = startDay;
  for (int i = 0; i < numMonths; i++) {
    while (getMonthFromDay(dayPointer) == curMonth) {
      dayPointer++;
    }
    curMonth = getMonthFromDay(dayPointer);
  }
  dayPointer--;

  return dayPointer;
}

std::vector<DataPoint> WordDataSet::generateDataPoints(
    const std::map<int, std::vector<std::string>> &dayMap,
    const std::string &target, char periodLength, int numberOfPeriods) {

  std::vector<DataPoint> dataPoints;
  int mapSize = dayMap.size() - 1;

  switch (periodLength) {

  case 'd': {
    for (int i = 0; i < dayMap.size() - 1; i += numberOfPeriods) {
      int startDay = i;
      int endDay = std::min(i + numberOfPeriods - 1, mapSize);

      int value = countOccurence(dayMap, target, startDay, endDay);

      std::string label = generateLabel(startDay, endDay, periodLength);

      DataPoint *dp = new DataPoint(label, value);
      dataPoints.push_back(*dp);
    }
    break;
  }
  case 'w': {
    for (int i = 0; i < dayMap.size() - 1; i += numberOfPeriods * 7) {
      int startDay = i;
      int endDay = std::min(i + numberOfPeriods * 7 - 1, mapSize);

      int value = countOccurence(dayMap, target, startDay, endDay);

      std::string label = generateLabel(startDay, endDay, periodLength);

      DataPoint *dp = new DataPoint(label, value);
      dataPoints.push_back(*dp);
    }
    break;
  }

  case 'm': {
    int startDay = 0;
    while (startDay < dayMap.size()) {
      int endDay = std::min(mapSize, getEndOfMonth(startDay, numberOfPeriods));

      int value = countOccurence(dayMap, target, startDay, endDay);
      std::string label = generateLabel(startDay, endDay, periodLength);

      DataPoint *dp = new DataPoint(label, value);
      dataPoints.push_back(*dp);

      startDay = endDay + 1;
    }

    break;
  }

  default: {
    std::cout << "Invalid Period" << std::endl;
    break;
  }
  }
  return dataPoints;
}

int WordDataSet::getDateFromDay(int day) {
  const int FIRST_DAY_AS_UNIX_TIME = 1612198800;
  const int SECONDS_IN_DAY = 86400;
  time_t dateTime = FIRST_DAY_AS_UNIX_TIME + SECONDS_IN_DAY * day;
  struct tm *dateTimeInfo = localtime(&dateTime);

  return dateTimeInfo->tm_mday;
}

int WordDataSet::getMonthFromDay(int day) {
  const int FIRST_DAY_AS_UNIX_TIME = 1612198800;
  const int SECONDS_IN_DAY = 86400;
  time_t dateTime = FIRST_DAY_AS_UNIX_TIME + SECONDS_IN_DAY * day;
  struct tm *dateTimeInfo = localtime(&dateTime);

  return dateTimeInfo->tm_mon;
}

std::string WordDataSet::generateLabel(int startDay, int endDay, char periodLength) {
  std::string returnLabel = "";

  std::map<int, std::string> monthNumToName = {
      {0, "Jan"}, {1, "Feb"}, {2, "Mar"}, {3, "Apr"}, {4, "May"},  {5, "Jun"},
      {6, "Jul"}, {7, "Aug"}, {8, "Sep"}, {9, "Oct"}, {10, "Nov"}, {11, "Dec"}};

  switch (periodLength) {
  case 'd':
  case 'w': {

    if (startDay == endDay) {
      returnLabel = monthNumToName.at(getMonthFromDay(startDay)) + " " +
                    std::to_string(getDateFromDay(startDay));
    } else {

      returnLabel = monthNumToName.at(getMonthFromDay(startDay)) + " " +
                    std::to_string(getDateFromDay(startDay)) + " - " +
                    monthNumToName.at(getMonthFromDay(endDay)) + " " +
                    std::to_string(getDateFromDay(endDay));
    }
    break;
  }
  case 'm': {
    if (abs(startDay - endDay) < 32 &&
        getMonthFromDay(startDay) == getMonthFromDay(endDay)) {
      returnLabel = monthNumToName.at(getMonthFromDay(startDay));
    } else {
      returnLabel = monthNumToName.at(getMonthFromDay(startDay)) + " - " +
                    monthNumToName.at(getMonthFromDay(endDay));
    }
    break;
  }
  default: {
    std::cout << "Invlid case" << std::endl;
  }
  }
  return returnLabel;
}

bool WordDataSet::preTargetCheck(
    const std::vector<std::string> &vec,
    const std::vector<std::vector<std::string>> &pretargetWords,
    const int targetIndex) {

  for (int i = 0; i < pretargetWords.size(); i++) {

    bool flag = true; // if any target word isn't correct, flag is flipped
    for (int j = 0; j < pretargetWords.at(i).size(); j++) {
      // compares the current target prase against the phrase directly before
      // the target phrase
      if (!(targetIndex - j > 0 &&
            strcmpIgnoreCase(vec.at(targetIndex - 1 - j),
                             pretargetWords.at(i).at(
                                 pretargetWords.at(i).size() - j - 1)))) {

        flag = false;
        j = pretargetWords.at(i).size();
      }
    }

    // only if flag remains unflipped for each index in any given target
    // word phrase, then it returns false
    if (flag) {
      return false;
    }
  }
  return true;
}

bool WordDataSet::postTargetCheck(
    const std::vector<std::string> &vec,
    const std::vector<std::vector<std::string>> &posttargetWords,
    int targetIndex, int offset) {

  for (int i = 0; i < posttargetWords.size(); i++) {

    bool flag = true; // if any target word isn't correct, flag is flipped
    for (int j = 0; j < posttargetWords.at(i).size(); j++) {
      // compares the current target prase against the phrase directly after
      // the target phrase
      if (!(targetIndex + offset + j < vec.size() - 1 &&
            strcmpIgnoreCase(vec.at(targetIndex + offset + j + 1),
                             posttargetWords.at(i).at(j)))) {
        flag = false;
        j = posttargetWords.at(i).size();
      }
    }

    // only if flag remains unflipped for each index in any given target
    // word phrase, then it returns false
    if (flag) {
      return false;
    }
  }
  return true;
}

bool WordDataSet::targetCheck(const std::vector<std::string> &vec,
                              const std::vector<std::string> &targetWords,
                              const int targetIndex) {

  for (int i = 0; i < targetWords.size(); i++) {
    if (!strcmpIgnoreCase(vec.at(targetIndex + i), targetWords.at(i))) {
      return false;
    }
  }
  return true;
}

int WordDataSet::countOccurence(
    const std::map<int, std::vector<std::string>> &dayMap,
    const std::string &targetStr, int start, int end) {
  std::string line;
  std::string target;
  std::vector<std::string> targetWords;
  std::vector<std::vector<std::string>> pretargetWords;
  std::vector<std::vector<std::string>> posttargetWords;
  int numOccurences = 0;

  std::stringstream ss(targetStr);
  getline(ss, line, ':');
  target = line;
  while (getline(ss, line, ':')) {
    if (line[0] == '-') {
      std::vector<std::string> pretarget;
      std::stringstream prestream(line.substr(1));
      std::string wordInLine;
      while (getline(prestream, wordInLine, ' ')) {
        pretarget.push_back(wordInLine);
      }
      pretargetWords.push_back(pretarget);
    } else {
      std::vector<std::string> posttarget;
      std::stringstream poststream(line);
      std::string wordInLine;
      while (getline(poststream, wordInLine, ' ')) {
        posttarget.push_back(wordInLine);
      }
      posttargetWords.push_back(posttarget);
    }
  }

  std::stringstream targetstream(target);
  while (getline(targetstream, line, ' ')) {
    targetWords.push_back(line);
  }

  for (int i = start; i <= end; i++) {
    std::vector<std::string> curVector;
    curVector = dayMap.at(i);
    for (int j = 0; j < curVector.size(); j++) {
      int offset = targetWords.size() - 1;

      if (targetCheck(curVector, targetWords, j) &&
          preTargetCheck(curVector, pretargetWords, j) &&
          postTargetCheck(curVector, posttargetWords, j, offset)) {
        numOccurences++;
      }
    }
  }
  return numOccurences;
}

void WordDataSet::print() {
  for (auto kv : dataPoints) {
    std::cout << kv.getLabel() << " : " << kv.getValue() << std::endl;
  }
}

bool WordDataSet::strcmpIgnoreCase(const std::string &a, const std::string &b) {
  return std::equal(a.begin(), a.end(), b.begin(), b.end(),
                    [](char a, char b) { return tolower(a) == tolower(b); });
}

std::string WordDataSet::getData() {
  std::string data;

  for (auto kv : dataPoints) {
    data += kv.getLabel() + " : " + std::to_string(kv.getValue()) + "\n";
  }

  return data;
}
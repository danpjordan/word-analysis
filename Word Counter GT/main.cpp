// g++ -std=c++17 WordDataSet.cpp main.cpp && ./a.out && python3
// graphsFromJson.py

#include "header_files/DataPoint.h"
#include "header_files/InvalidDayException.h"
#include "header_files/WordDataSet.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <set>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <vector>

void openFileStreamIn(std::fstream *FS, const std::string &name) {
  FS->open(name, std::ios_base::in);
  if (!FS->is_open()) {
    std::cout << "Failed to open " << name << std::endl;
  }
}

void openFileStreamOut(std::fstream *FS, const std::string &name) {
  FS->open(name, std::ios_base::out);
  if (!FS->is_open()) {
    std::cout << "Failed to open " << name << std::endl;
  }
}

void makeDirectory(const std::string &name) {
  if (mkdir(name.data(), 0777) == -1) {
    std::cerr << "Directory \"/" << name << "/\" already exists" << std::endl;
  } else {
    std::cout << "Directory \"/" << name << "/\" created" << std::endl;
  }
}

void removePunct(std::string &s) {
  std::string str = "";
  for (char c : s) {
    if (isalnum(c) || c == '-') {
      str += c;
    }
  }
  s = str;
}

bool strcmpIgnoreCase(const std::string &a, const std::string &b) {
  return std::equal(a.begin(), a.end(), b.begin(), b.end(),
                    [](char a, char b) { return tolower(a) == tolower(b); });
}

void cleanWordFiles(const std::vector<std::string> &wordsTxtFiles,
                    const std::string &outputWordsFileName, int firstDay) {
  std::fstream wordsFile;
  openFileStreamOut(&wordsFile, outputWordsFileName);

  for (int i = 0; i < wordsTxtFiles.size(); i++) {
    std::fstream curFile;
    std::string line;

    std::cout << "Scanning file: " << wordsTxtFiles.at(i) << std::endl;
    openFileStreamIn(&curFile, wordsTxtFiles.at(i));

    int state = true; // when true, program looks for Day XXXX, when false
                      // program looks for text

    while (getline(curFile, line)) {
      if (state) {

        while (!strcmpIgnoreCase(line.substr(0, 3), "Day")) {
          getline(curFile, line);
        }

        // keeps track of the expected day
        if (firstDay != stoi(line.substr(4, 4))) {
          throw InvalidDayException(firstDay, "1");
        }
        wordsFile << line; // output line to words file
        state = !state;    // flip the state

      } else {
        while (line.size() < 4) {
          throw InvalidDayException(firstDay, "2");
        }
        wordsFile << line; // output line to words file
        firstDay++;
        state = !state; // flip the state
      }
      wordsFile << "\n";
    }
    curFile.close();
  }
}

struct map_comp {
  template <typename T>

  // Comparator function
  bool operator()(const T &l, const T &r) const {
    if (l.second != r.second) {
      return r.second < l.second;
    }
    return r.first > l.first;
  }
};

struct ci_less {
  // case-independent (ci) compare_less binary function
  struct nocase_compare {
    bool operator()(const unsigned char &c1, const unsigned char &c2) const {
      return tolower(c1) < tolower(c2);
    }
  };
  bool operator()(const std::string &s1, const std::string &s2) const {
    return std::lexicographical_compare(s1.begin(), s1.end(), // source range
                                        s2.begin(), s2.end(), // dest range
                                        nocase_compare());    // comparison
  }
};

// creates a case insensitive wordlist using the data in cleanWordsFileName
std::map<std::string, int, ci_less>
createWordList(const std::string &cleanWordsFileName) {
  std::fstream curFile;

  std::map<std::string, int, ci_less> words_map;
  openFileStreamIn(&curFile, cleanWordsFileName);

  std::string line;
  getline(curFile, line); // throws away the first 'Day' line
  while (getline(curFile, line)) {
    std::stringstream ss(line);
    std::string word;
    while (ss >> word) {
      removePunct(word);
      if (words_map.find(word) != words_map.end()) {
        words_map.at(word)++;
      } else {
        words_map.insert(std::pair(word, 1));
      }
    }
    getline(curFile, line); // throws away the next 'Day' line
  }

  curFile.close();
  return words_map;
}

// sorts the map by key value and outputs it to the wordListFileName file
void outputSortedWordList(
    const std::string &wordListFileName,
    const std::map<std::string, int, ci_less> &words_map) {

  std::fstream curFile;
  openFileStreamOut(&curFile, wordListFileName);

  std::set<std::pair<std::string, int>, map_comp> S(words_map.begin(),
                                                    words_map.end());
  for (auto &it : S) {
    curFile << it.first << " - " << it.second << std::endl;
  }

  curFile.close();
}

std::map<int, std::vector<std::string>>
getDayMap(const std::string &cleanWordsFileName) {

  int dayNum = 0;
  std::fstream curFile;
  std::string line;
  std::map<int, std::vector<std::string>> dayMap;

  openFileStreamIn(&curFile, cleanWordsFileName);

  getline(curFile, line); // throws away the first 'Day' line
  while (getline(curFile, line)) {
    std::stringstream ss(line);
    std::string word;
    std::vector<std::string> wordsInLine;
    while (ss >> word) {
      removePunct(word);
      wordsInLine.push_back(word);
    }

    dayMap.insert(std::pair(dayNum, wordsInLine));
    dayNum++;

    getline(curFile, line); // throws away the next 'Day' line
  }
  curFile.close();
  return dayMap;
}

std::vector<WordDataSet>
getWordDataSetList(const std::map<int, std::vector<std::string>> &dayMap,
                   const std::vector<std::string> &wordsToGetDataAbout,
                   const char periodLength, const int numberOfPeriods = 1) {
  std::vector<WordDataSet> wordDataSetList;

  for (int i = 0; i < wordsToGetDataAbout.size(); i++) {
    wordDataSetList.push_back(WordDataSet(dayMap, wordsToGetDataAbout.at(i),
                                          periodLength, numberOfPeriods));
  }

  return wordDataSetList;
}

void outputWordDataSetList(const std::vector<WordDataSet> &wordDataSetList,
                           const std::string &wordTrendDataFileName) {
  std::fstream curFile;
  openFileStreamOut(&curFile, wordTrendDataFileName);

  for (auto kv : wordDataSetList) {
    curFile << kv.getTarget() << " - ";
    if (kv.getNumberOfPeriods() != 1) {
      curFile << kv.getNumberOfPeriods();
    }
    curFile << kv.getPeriodLength() << std::endl;
    curFile << kv.getData() << std::endl << std::endl;
  }
  curFile.close();
}

void outPutWordsAsCols(std::string cleanWordsFileName,
                       std::string wordsAsColsFileName) {
  std::fstream curFileIn;
  std::fstream curFileOut;
  openFileStreamIn(&curFileIn, cleanWordsFileName);
  openFileStreamOut(&curFileOut, wordsAsColsFileName);

  std::string lineOfDay;
  std::string lineOfEntry;

  while (getline(curFileIn, lineOfDay)) {
    getline(curFileIn, lineOfEntry);
    curFileOut << lineOfDay.substr(0, 8) << "\t" << lineOfEntry << std::endl;
  }
  curFileIn.close();
  curFileOut.close();
}

void generateJson(std::vector<WordDataSet> &wordDataSetList,
                  const std::string &jsonFileName,
                  const std::string &jsonDataName) {
  std::fstream curFile;
  openFileStreamOut(&curFile, jsonFileName);

  curFile << "{" << std::endl;
  curFile << "\t\"name\": \"" << jsonDataName << "\"," << std::endl;
  curFile << "\t\"tickLabels\": [" << std::endl;

  std::vector<DataPoint> dP = wordDataSetList.at(0).getDataPoints();

  for (int i = 0; i < dP.size(); i++) {
    if (i < dP.size() - 1) {
      curFile << "\t\t\"" << dP.at(i).getLabel() << "\"," << std::endl;
    } else {
      curFile << "\t\t\"" << dP.at(dP.size() - 1).getLabel() << "\""
              << std::endl;
    }
  }

  curFile << "\t]," << std::endl;
  curFile << "\t\"dataPoints\": [" << std::endl;

  for (int i = 0; i < wordDataSetList.size(); i++) {
    std::string name;
    std::string fullName = wordDataSetList.at(i).getTarget();
    std::stringstream ss(fullName);
    getline(ss, name, ':');

    curFile << "\t\t{" << std::endl;

    curFile << "\t\t\t\"name\": \"" << name << "\"," << std::endl;
    curFile << "\t\t\t\"fullName\": \"" << fullName << "\"," << std::endl;

    curFile << "\t\t\t\"y-axis\": [";

    dP = wordDataSetList.at(i).getDataPoints();
    for (int j = 0; j < dP.size(); j++) {
      if (j < dP.size() - 1) {
        curFile << dP.at(j).getValue() << ", ";
      } else {
        curFile << dP.at(j).getValue();
      }
    }

    curFile << "]" << std::endl;

    if (i < wordDataSetList.size() - 1) {
      curFile << "\t\t}," << std::endl;
    } else {
      curFile << "\t\t}" << std::endl;
    }
  }
  curFile << "\t]" << std::endl;
  curFile << "}" << std::endl;

  curFile.close();
}

std::string generateFileName(std::string name) {
  std::string fileName;

  for (int i = 0; i < name.size(); i++) {
    if (name.at(i) == ' ') {
      fileName.push_back('_');
    } else {
      fileName.push_back(tolower(name.at(i)));
    }
  }

  return fileName;
}

int main() {
  std::vector<std::string> wordsTxtFiles;
  wordsTxtFiles.push_back("words_files/words1.txt");
  wordsTxtFiles.push_back("words_files/words2.txt");

  std::string dataFolderName = "data";
  std::string trendFolderName = dataFolderName + "/trends";
  std::string jsonFolderName = dataFolderName + "/json";
  std::string wordTextFolderName = dataFolderName + "/word_text";

  std::string cleanWordsFileName = wordTextFolderName + "/words.txt";
  std::string wordsAsColsFileName = wordTextFolderName + "/words_as_cols.txt";

  std::string wordListFileName = dataFolderName + "/word_list.txt";

  std::map<int, std::vector<std::string>> dayMap;
  std::map<std::string, int, ci_less> wordList;

  std::vector<std::string> wordsRegardingThing{"Lorem", "Suspendisse", "Vivamus", "Etiam", "Maecenas"};

  std::vector<std::string> wordsRegardingOtherThing{"consequat", "ligula", "habitant", "senectus"};

  std::map<std::string, std::vector<std::string>> dataSets = {
      {"Thing", wordsRegardingThing},
      {"OtherThing", wordsRegardingOtherThing},
  };

  int firstDay = 6478;

  makeDirectory(dataFolderName);
  makeDirectory(trendFolderName);
  makeDirectory(jsonFolderName);
  makeDirectory(wordTextFolderName);

  std::cout << "Cleaning words files:" << std::endl;
  try {
    cleanWordFiles(wordsTxtFiles, cleanWordsFileName, firstDay);
  } catch (InvalidDayException &e) {
    std::cerr << e.what() << e.data();
    return 1;
  }

  std::cout << "Output words as columns" << std::endl;
  outPutWordsAsCols(cleanWordsFileName, wordsAsColsFileName);

  std::cout << "Creating word list" << std::endl;
  wordList = createWordList(cleanWordsFileName);

  std::cout << "Outputing sorted word list" << std::endl;
  outputSortedWordList(wordListFileName, wordList);

  std::cout << "Creating day map" << std::endl;
  dayMap = getDayMap(cleanWordsFileName);

  std::cout << "Creating and outputing word data sets:" << std::endl;

  for (auto kv : dataSets) {
    char periodLength = 'm';
    int numberOfPeriods = 2;
    std::string name = kv.first;
    std::string fileName = generateFileName(name);
    std::vector<std::string> words = kv.second;
    std::string trendsFileName =
        trendFolderName + "/trends_" + fileName + ".txt";
    std::string jsonFileName = jsonFolderName + "/" + fileName + ".json";

    std::cout << "Word data set " << name << std::endl;
    std::vector<WordDataSet> wordDataSetList =
        getWordDataSetList(dayMap, words, periodLength, numberOfPeriods);
    outputWordDataSetList(wordDataSetList, trendsFileName);
    generateJson(wordDataSetList, jsonFileName, name);
  }

  return 0;
}
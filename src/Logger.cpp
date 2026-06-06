#include "../include/Logger.h"

Logger::Logger(string file) {
  filename = file;
  errorCount = 0;

  ifstream inputFile(file, ios::in);
  string line;

  if (inputFile.is_open()) {
    while (getline(inputFile, line)) {
      source.append(line + '\n');
    }
    inputFile.close();
  } else {
    cout << "Unable to open file\n";
  }
}

string Logger::getSource() { return source; }

int Logger::getErrorCount() { return errorCount; }

bool Logger::hasErrors() { return errorCount > 0; }

string Logger::getLineContent(int lineNumber) {
  int current = 1;
  int start = 0;
  for (int i = 0; i < (int)source.length(); i++) {
    if (current == lineNumber) {
      int end = i;
      while (end < (int)source.length() && source[end] != '\n') {
        end++;
      }
      return source.substr(start, end - start);
    }
    if (source[i] == '\n') {
      current++;
      start = i + 1;
    }
  }
  return "";
}

void Logger::printDiagnostic(int errLine, int errCol, string msg, int len) {
  cout << "\033[1m" << filename << ":" << errLine << ":" << errCol << ": "
       << "\033[1;31merro: " << "\033[0m" << msg << "\n";

  string sourceLine = getLineContent(errLine);
  if (sourceLine.empty() && errLine > 1) {
    errLine--;
    sourceLine = getLineContent(errLine);
    errCol = sourceLine.length() + 1;
    if (errCol < 1)
      errCol = 1;
  }

  if (!sourceLine.empty()) {
    int startIdx = errCol > 0 ? errCol - 1 : 0;
    int endIdx = startIdx + len;
    if (endIdx > (int)sourceLine.length())
      endIdx = sourceLine.length();
    if (startIdx > endIdx)
      startIdx = endIdx;

    string prefix = sourceLine.substr(0, startIdx);
    string errorText = sourceLine.substr(startIdx, endIdx - startIdx);
    string suffix = sourceLine.substr(endIdx);

    cout << "   " << errLine << " | " << prefix << "\033[1;31m" << errorText
         << "\033[0m" << suffix << "\n";

    string spacePrefix(to_string(errLine).length(), ' ');
    string tils = (len > 1 && endIdx - startIdx > 1)
                      ? string(endIdx - startIdx - 1, '~')
                      : "";
    cout << "   " << spacePrefix << " | " << string(startIdx, ' ')
         << "\033[1;31m^" << tils << "\033[0m\n";
  }

  errorCount++;
}

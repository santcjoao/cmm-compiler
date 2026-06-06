#ifndef __Logger_h
#define __Logger_h

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

class Logger {
private:
  string filename;
  string source;
  int errorCount;

public:
  Logger(string file);

  string getSource();
  string getLineContent(int lineNumber);

  void printDiagnostic(int errLine, int errCol, string msg, int len = 1);

  int getErrorCount();
  bool hasErrors();
};

#endif

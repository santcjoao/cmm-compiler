#ifndef __Logger_h
#define __Logger_h

#include <string>

using namespace std;

class SourceFile;

enum DiagLevel { ERROR, WARNING, NOTE };

class Logger {
public:
  const SourceFile &sourceFile;
  int errorCount;
  int warningCount;

  Logger(const SourceFile &sourceFile);

  void printDiagnostic(DiagLevel level, int line, int col, string msg, int len = 1);

  void error(int line, int col, string msg, int len = 1);
  void warning(int line, int col, string msg, int len = 1);
  void note(int line, int col, string msg, int len = 1);

  void printSummary();

  int getErrorCount();
  int getWarningCount();
  bool hasErrors();
};

#endif

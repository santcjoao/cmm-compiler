#ifndef __Logger_h
#define __Logger_h

#include <string>

class SourceFile;

enum class DiagLevel { ERROR, WARNING, NOTE };

class Logger {
public:
  Logger(const SourceFile &sourceFile);

  void printDiagnostic(DiagLevel level, int line, int col,
                       const std::string &msg, int len = 1);

  void error(int line, int col, const std::string &msg, int len = 1);
  void warning(int line, int col, const std::string &msg, int len = 1);
  void note(int line, int col, const std::string &msg, int len = 1);

  void printSummary();

  int getErrorCount() const;
  int getWarningCount() const;
  bool hasErrors() const;

private:
  const SourceFile &sourceFile;
  int errorCount;
  int warningCount;
};

#endif

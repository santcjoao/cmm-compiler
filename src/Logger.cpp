#include "../include/Logger.h"
#include "../include/SourceFile.h"

#include <iostream>

using namespace std;

Logger::Logger(const SourceFile &src)
    : sourceFile(src), errorCount(0), warningCount(0) {}

int Logger::getErrorCount() const { return errorCount; }

int Logger::getWarningCount() const { return warningCount; }

bool Logger::hasErrors() const { return errorCount > 0; }

void Logger::error(int line, int col, const string &msg, int len) {
  printDiagnostic(DiagLevel::ERROR, line, col, msg, len);
}

void Logger::warning(int line, int col, const string &msg, int len) {
  printDiagnostic(DiagLevel::WARNING, line, col, msg, len);
}

void Logger::note(int line, int col, const string &msg, int len) {
  printDiagnostic(DiagLevel::NOTE, line, col, msg, len);
}

void Logger::printDiagnostic(DiagLevel level, int errLine, int errCol,
                             const string &msg, int len) {
  string levelStr;

  switch (level) {
  case DiagLevel::ERROR:
    levelStr = "erro";
    errorCount++;
    break;
  case DiagLevel::WARNING:
    levelStr = "aviso";
    warningCount++;
    break;
  case DiagLevel::NOTE:
    levelStr = "nota";
    break;
  }

  cout << sourceFile.getFilename() << ":" << errLine << ":" << errCol << ": "
       << levelStr << ": " << msg << "\n";

  string sourceLine = sourceFile.getLineContent(errLine);
  if (sourceLine.empty() && errLine > 1) {
    errLine--;
    sourceLine = sourceFile.getLineContent(errLine);
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

    cout << "   " << errLine << " | " << prefix << errorText << suffix << "\n";

    string spacePrefix(to_string(errLine).length(), ' ');
    string tils = (len > 1 && endIdx - startIdx > 1)
                      ? string(endIdx - startIdx - 1, '~')
                      : "";
    cout << "   " << spacePrefix << " | " << string(startIdx, ' ') << "^"
         << tils << "\n";
  }
}

void Logger::printSummary() {
  if (errorCount > 0) {
    cout << "Compilação falhou com " << errorCount << " erro(s)";
    if (warningCount > 0)
      cout << " e " << warningCount << " aviso(s)";
    cout << ".\n";
  } else if (warningCount > 0) {
    cout << "Compilação encerrada com " << warningCount << " aviso(s).\n";
  } else {
    cout << "Compilação encerrada com sucesso.\n";
  }
}

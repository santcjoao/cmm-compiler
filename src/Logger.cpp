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
  string colorCode;

  switch (level) {
  case DiagLevel::ERROR:
    levelStr = "erro";
    colorCode = "\033[1;31m"; // vermelho
    errorCount++;
    break;
  case DiagLevel::WARNING:
    levelStr = "aviso";
    colorCode = "\033[1;35m"; // magenta
    warningCount++;
    break;
  case DiagLevel::NOTE:
    levelStr = "nota";
    colorCode = "\033[1;36m"; // ciano
    break;
  }

  cout << "\033[1m" << sourceFile.getFilename() << ":" << errLine << ":"
       << errCol << ": " << colorCode << levelStr << ": "
       << "\033[0m" << msg << "\n";

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

    cout << "   " << errLine << " | " << prefix << colorCode << errorText
         << "\033[0m" << suffix << "\n";

    string spacePrefix(to_string(errLine).length(), ' ');
    string tils = (len > 1 && endIdx - startIdx > 1)
                      ? string(endIdx - startIdx - 1, '~')
                      : "";
    cout << "   " << spacePrefix << " | " << string(startIdx, ' ') << colorCode
         << "^" << tils << "\033[0m\n";
  }
}

void Logger::printSummary() {
  if (errorCount > 0) {
    cout << "\033[1;31mCompilação falhou com " << errorCount << " erro(s)";
    if (warningCount > 0)
      cout << " e " << warningCount << " aviso(s)";
    cout << ".\033[0m\n";
  } else if (warningCount > 0) {
    cout << "\033[1;33mCompilação encerrada com " << warningCount
         << " aviso(s).\033[0m\n";
  } else {
    cout << "\033[1;32mCompilação encerrada com sucesso.\033[0m\n";
  }
}

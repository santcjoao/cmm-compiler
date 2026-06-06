#include "include/Logger.h"
#include "include/Parser.h"
#include "include/Scanner.h"
#include "include/SourceFile.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

int main(int argc, char *argv[]) {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
#endif

  if (argc != 2) {
    cout << "Uso: ./compiler nome_arquivo.cmm\n";
    return 1;
  }

  SourceFile source(argv[1]);
  if (!source.isValid()) {
    return 1;
  }

  Logger logger{source};
  Scanner scanner{source, &logger};
  Parser parser{&scanner, &logger};
  parser.run();

  if (logger.hasErrors()) {
    return 1;
  }
  return 0;
}
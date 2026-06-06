#include "include/Parser.h"
#include "include/Scanner.h"
#include "include/Logger.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cout << "Uso: ./compiler nome_arquivo.cmm\n";
    return 1;
  }

  Logger logger(argv[1]);
  Scanner scanner(&logger);
  Parser parser(&scanner, &logger);
  parser.run();

  if (logger.hasErrors()) {
    return 1;
  }
  return 0;
}
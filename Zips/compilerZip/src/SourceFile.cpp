#include "../include/SourceFile.h"

#include <fstream>
#include <iostream>

using namespace std;

SourceFile::SourceFile(const string &file) : filename(file), valid(false) {
  if (file.length() < 4 || file.substr(file.length() - 4) != ".cmm") {
    cerr << "Erro: O arquivo de entrada deve ter a extensão '.cmm': " << file
         << "\n";
    return;
  }

  ifstream inputFile(file, ios::in);
  string line;

  if (inputFile.is_open()) {
    while (getline(inputFile, line)) {
      source.append(line + '\n');
    }
    inputFile.close();
    valid = true;
  } else {
    cerr << "Não foi possível abrir o arquivo: " << file << "\n";
  }
}

const string &SourceFile::getSource() const { return source; }

const string &SourceFile::getFilename() const { return filename; }

bool SourceFile::isValid() const { return valid; }

string SourceFile::getLineContent(int lineNumber) const {
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

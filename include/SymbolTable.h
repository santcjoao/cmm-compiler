#ifndef __SymbolTable_h
#define __SymbolTable_h

#include "Token.h"
#include <map>
#include <string>

using namespace std;

class STEntry {
public:
  Token *token;
  bool reserved;

  STEntry() {
    token = 0;
    reserved = false;
  }
  STEntry(Token *t) {
    token = t;
    reserved = false;
  }
  STEntry(Token *t, bool r) {
    token = t;
    reserved = r;
  }
};

class SymbolTable {
private:
  SymbolTable *parent;
  map<string, STEntry *> symbols;

public:
  SymbolTable();
  SymbolTable(SymbolTable *);

  bool add(STEntry *);
  bool remove(string);
  void clear();
  bool isEmpty();

  STEntry *get(string);
  SymbolTable *getParent();
};

#endif
#ifndef __SymbolTable_h
#define __SymbolTable_h

#include "Token.h"

#include <map>
#include <string>

class STEntry {
public:
  Token token;
  bool reserved;

  STEntry(const Token &t, bool r = false) : token(t), reserved(r) {}
};

class SymbolTable {
public:
  SymbolTable();
  SymbolTable(SymbolTable *parent);

  bool add(STEntry entry);
  bool remove(const std::string &name);
  void clear();
  bool isEmpty() const;

  STEntry *get(const std::string &name);
  SymbolTable *getParent();

private:
  SymbolTable *parent;
  std::map<std::string, STEntry> symbols;
};

#endif
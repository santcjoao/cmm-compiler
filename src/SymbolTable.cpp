#include "../include/SymbolTable.h"

using namespace std;

SymbolTable::SymbolTable() : parent(nullptr) {}

SymbolTable::SymbolTable(SymbolTable *p) : parent(p) {}

bool SymbolTable::add(STEntry entry) {
  string key = entry.token.lexeme;
  if (symbols.find(key) != symbols.end())
    return false;
  symbols.insert(make_pair(key, entry));
  return true;
}

bool SymbolTable::remove(const string &name) {
  return symbols.erase(name) != 0;
}

void SymbolTable::clear() { symbols.clear(); }

bool SymbolTable::isEmpty() const { return symbols.empty(); }

STEntry *SymbolTable::get(const string &name) {
  auto it = symbols.find(name);
  if (it != symbols.end()) {
    return &it->second;
  }
  if (parent) {
    return parent->get(name);
  }
  return nullptr;
}

SymbolTable *SymbolTable::getParent() { return parent; }

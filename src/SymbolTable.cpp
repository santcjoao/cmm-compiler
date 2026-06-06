#include "../include/SymbolTable.h"

SymbolTable::SymbolTable() { parent = nullptr; }

SymbolTable::SymbolTable(SymbolTable *p) { parent = p; }

bool SymbolTable::add(STEntry *t) {
  if (symbols.find(t->token->lexeme) != symbols.end())
    return false;
  symbols.insert({t->token->lexeme, t});
  return true;
}

bool SymbolTable::remove(string name) { return symbols.erase(name) != 0; }

void SymbolTable::clear() { symbols.clear(); }

bool SymbolTable::isEmpty() { return symbols.empty(); }

STEntry *SymbolTable::get(string name) {
  auto it = symbols.find(name);
  if (it != symbols.end()) {
    return it->second;
  }
  if (parent) {
    return parent->get(name);
  }
  return nullptr;
}

SymbolTable *SymbolTable::getParent() { return parent; }

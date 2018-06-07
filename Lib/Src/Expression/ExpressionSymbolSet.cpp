#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

ExpressionSymbolSet::ExpressionSymbolSet() : BitSet(ParserTree::getTerminalCount()) {
}

// terminate list with EOI
ExpressionSymbolSet::ExpressionSymbolSet(ExpressionInputSymbol s1,...) : BitSet(ParserTree::getTerminalCount()) {
  va_list argptr;
  va_start(argptr, s1);
  init(s1,argptr);
  va_end(argptr);
}

ExpressionSymbolSet::ExpressionSymbolSet(ExpressionInputSymbol s1, va_list argptr) : BitSet(ParserTree::getTerminalCount()) {
  init(s1, argptr);
}

void ExpressionSymbolSet::init(ExpressionInputSymbol s1, va_list argptr) {
  add(s1);
  for(ExpressionInputSymbol s=va_arg(argptr, ExpressionInputSymbol); s != EOI; s = va_arg(argptr, ExpressionInputSymbol)) {
    add(s);
  }
}

class SymbolStringifier : public AbstractStringifier<ExpressionInputSymbol> {
public:
  String toString(const ExpressionInputSymbol &e) {
    return ExpressionNode::getSymbolName(e);
  }
};

class SymbolSetStringifier : public AbstractStringifier<size_t> {
public:
  String toString(const size_t &e) {
    const ExpressionInputSymbol symbol = (ExpressionInputSymbol)e;
    return ExpressionNode::getSymbolName(symbol);
  }
};

String ExpressionSymbolSet::toString() const {
  return __super::toString(&SymbolSetStringifier());
}

}; // namespace Expr
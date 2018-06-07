#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

NodeTypeSet::NodeTypeSet(ExpressionNodeType t1,...) { // terminate argumentlist with -1
  va_list argptr;
  va_start(argptr, t1);
  init(t1,argptr);
  va_end(argptr);
}

void NodeTypeSet::init(ExpressionNodeType t1, va_list argptr) {
  add(t1);
  for(ExpressionNodeType t=va_arg(argptr, ExpressionNodeType); (int)t != -1; t = va_arg(argptr, ExpressionNodeType)) {
    add(t);
  }
  UPDATEDEBUGSTRING(*this);
}

class NodeTypeSetStringifier : public AbstractStringifier<UINT> {
public:
  String toString(const UINT &e) {
    const ExpressionNodeType type = (ExpressionNodeType)e;
    return ExpressionNode::getNodeTypeName(type);
  }
};

String NodeTypeSet::toString() const {
  return __super::toString(&NodeTypeSetStringifier());
}

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
  UPDATEDEBUGSTRING(*this);
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
#include "pch.h"
#include <AbstractParserTables.h>

String AbstractParserTables::getRightString(UINT prod) const {
  const UINT length = getProductionLength(prod);
  if(length == 0) {
    return _T("epsilon");
  } else {
    UINT symbolArray[1000], *symbols = symbolArray;
    if(length > ARRAYSIZE(symbolArray)) { // just in case....
      symbols = new UINT[length]; TRACE_NEW(symbols);
    }
    getRightSide(prod, symbols);
    String result;
    for(UINT i = 0; i < length; i++) {
      if(i > 0) {
        result += _T(" ");
      }
      result += getSymbolName(symbols[i]);
    }
    if(symbols != symbolArray) {
      SAFEDELETEARRAY(symbols);
    }
    return result;
  }
}

  UINT AbstractParserTables::getNewState(UINT symbol, UINT state  ) const {
    assert(symbol < getSymbolCount());
    assert(state  < getStateCount());

    const UINT termCount = getTermCount();
    if(symbol < termCount) { // symbol is a terminal
      const int action = getAction(state, symbol);
      return (action > 0) ? action : _ParserError;
    } else {
      return getSuccessor(state, symbol);
    }
  }

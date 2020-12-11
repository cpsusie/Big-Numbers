#include "pch.h"
#include <AbstractParserTables.h>

namespace LRParsing {

String Action::toString() const {
  switch(getType()) {
  case PA_SHIFT   : return format(_T("Shift to  %4u"), getNewState());
  case PA_NEWSTATE: return format(_T("Goto  %4u"    ), getNewState());
  case PA_REDUCE  : return isAcceptAction()
                         ?        _T("Accept"       )
                         : format(_T("Reduce by %4u"), getReduceProduction());
  case PA_ERROR : return          _T("Error"        );
  }
  throwException(_T("%s:Unknown type:%u"), __TFUNCTION__, getType());
  return EMPTYSTRING;
}

String AbstractParserTables::getRightString(UINT prod) const {
  assert(prod < getProductionCount());
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

int AbstractParserTables::getNewState(UINT state, UINT symbol) const {
  assert(state  < getStateCount());
  assert(symbol < getSymbolCount());

  if(isTerminal(symbol)) {
    const Action action = getAction(state, symbol);
    return action.isShiftAction() ? action.getNewState() : -1;
  } else {
    return getSuccessor(state, symbol);
  }
}

}; // namespace LRParsing

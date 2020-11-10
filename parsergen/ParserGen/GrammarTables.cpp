#include "stdafx.h"
#include "GrammarTables.h"

GrammarTables::GrammarTables(const Grammar &grammar, const String &tablesClassName, const String &parserClassName)
: m_terminalCount(       grammar.getTerminalCount()  )
, m_symbolCount(         grammar.getSymbolCount()    )
, m_productionCount(     grammar.getProductionCount())
, m_stateCount(          grammar.getStateCount()     )
, m_compressibleStateSet(grammar.getStateCount()     )
, m_tablesClassName(     tablesClassName             )
, m_parserClassName(     parserClassName             )
{
  m_countTableBytes.clear();

  for(UINT p = 0; p < m_productionCount; p++) {
    const Production &prod   = grammar.getProduction(p);
    const int         length = prod.getLength();
    m_productionLength.add(length);
    m_left.add(prod.m_leftSide);
    m_rightSide.add(CompactIntArray());
    CompactIntArray &rightside = m_rightSide.last();
    for(int i = 0; i < length; i++) {
      rightside.add(prod.m_rightSide[i]);
    }
  }
  for(UINT s = 0; s < m_symbolCount; s++) {
    const GrammarSymbol &symbol = grammar.getSymbol(s);
    m_symbolNameArray.add(symbol.m_name);
  }

  const UINT stateCount = getStateCount();
  m_stateActions.setCapacity(stateCount);
  m_stateSucc.setCapacity(   stateCount);
  for(UINT s = 0; s < stateCount; s++) {
    m_stateActions.add(grammar.m_result[s].m_actions);
    m_stateSucc.add(grammar.m_result[s].m_succs);
  }
  initCompressibleStateSet();
}

UINT GrammarTables::getMaxInputCount() const {
  UINT m = 0;
  for(UINT s = 0; s < getStateCount(); s++) {
    const UINT count = getLegalInputCount(s);
    if(count > m) {
      m = count;
    }
  }
  return m;
}

BitSet GrammarTables::getNTOffsetSet(UINT state) const {
  const UINT         terminalCount = getTerminalCount();
  BitSet             result(getSymbolCount() - terminalCount);
  const ActionArray &ssa = m_stateSucc[state];
  for(const ParserAction &pa : ssa) {
    result.add((UINT)pa.m_token - terminalCount);
  }
  return result;
}

SuccesorArray GrammarTables::getSuccessorArray(UINT state) const {
  const ActionArray &ssa = m_stateSucc[state];
  SuccesorArray      result(ssa.size());
  for(const ParserAction &pa : ssa) {
    result.add(pa.m_action);
  }
  return result;
}

void GrammarTables::initCompressibleStateSet() {
  const UINT stateCount = getStateCount();
  for(UINT state = 0; state < stateCount; state++) {
    if(calcIsCompressibleState(state)) {
      m_compressibleStateSet.add(state);
    }
  }
}

// Returns true if actionArray.size == 1 or all actions in the specified state is reduce by the same production
bool GrammarTables::calcIsCompressibleState(UINT state) const {
  const ActionArray &aa    = m_stateActions[state];
  const UINT         count = (UINT)aa.size();
  switch(count) {
  case 0 :
    throwException(_T("actionArray for state %u has size 0"), state);
  case 1 :
    return true;
  default:
    { const int action = aa[0].m_action;
      if(action >= 0) {
        return false;
      }
      for(UINT i = 1; i < count; i++) {
        if(aa[i].m_action != action) {
          return false;
        }
      }
      return true;
    }
  }
}

ByteCount GrammarTables::getTotalSizeInBytes() const {
  return ByteCount(getTableByteCount(PLATFORM_X86), getTableByteCount(PLATFORM_X64));
}

UINT GrammarTables::getSuccessor(UINT state, UINT nt) const {
  const ActionArray &ssa = m_stateSucc[state];
  for(const ParserAction &pa : ssa) {
    if(pa.m_token == nt) {
      return pa.m_action;
    }
  }
  return _ParserError;
}

int GrammarTables::getAction(UINT state, UINT input) const {
  const ActionArray &aa = m_stateActions[state];
  for(const ParserAction &pa : aa) {
    if(pa.m_token == input) {
      return pa.m_action;
    }
  }
  return _ParserError;
}

void GrammarTables::getLegalInputs(UINT state, UINT *symbols) const {
  const ActionArray &aa = m_stateActions[state];
  for(const ParserAction &pa : aa) {
    *(symbols++) = pa.m_token;
  }
}

void GrammarTables::getRightSide(UINT prod, UINT *dst) const {
  const CompactIntArray &r = m_rightSide[prod];
  for(INT s : r) {
    *(dst++) = s;
  }
}

void GrammarTables::print(MarginFile &output) const {
  const Options &options = Options::getInstance();
  switch(options.m_language) {
  case CPP :
    printCpp(output);
    break;
  case JAVA:
    printJava(output);
    break;
  }
}

#include "stdafx.h"
#include "GrammarCode.h"
#include "GrammarTables.h"

GrammarTables::GrammarTables(const GrammarCode &grammarCode)
: m_grammarCode(         grammarCode)
, m_grammar(             grammarCode.getGrammar())
, m_grammarResult(       grammarCode.getGrammar().getResult())
, m_types(               grammarCode.getGrammar())
, m_compressibleStateSet(grammarCode.getGrammar().getStateCount())
{
  m_countTableBytes.clear();
  initCompressibleStateSet();
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
  const ParserActionArray &paa   = getGrammarResult().m_stateResult[state].m_actions;
  const UINT               count = paa.getLegalTermCount();
  switch(count) {
  case 0 :
    throwException(_T("ActionArray for state %u has size 0"), state);
  case 1 :
    return true;
  default:
    { const int action = paa[0].m_action;
      if(action >= 0) {
        return false;
      }
      for(UINT i = 1; i < count; i++) {
        if(paa[i].m_action != action) {
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

UINT GrammarTables::getSymbolCount()     const {
  return getGrammar().getSymbolCount();
}
UINT GrammarTables::getTermCount()       const {
  return getGrammar().getTermCount();
}

UINT GrammarTables::getProductionCount() const {
  return getGrammar().getProductionCount();
}

UINT GrammarTables::getStateCount()      const {
  return getGrammar().getStateCount();
}

UINT GrammarTables::getStartState()      const {
  return getGrammar().getStartState();
}

const String &GrammarTables::getSymbolName(UINT symbolIndex) const {
  return getGrammar().getSymbolName(symbolIndex);
}

void GrammarTables::getRightSide(UINT prod, UINT *dst) const {
  const Production &p = getGrammar().getProduction(prod);
  const CompactArray<RightSideSymbol> &rsa = p.m_rightSide;
  for(RightSideSymbol rs : rsa) {
    *(dst++) = rs.m_index;
  }
}

UINT GrammarTables::getProductionLength(UINT prod) const {
  return getGrammar().getProduction(prod).getLength();
}

UINT GrammarTables::getLeftSymbol(UINT prod) const {
  return getGrammar().getProduction(prod).m_leftSide;
}

int GrammarTables::getAction(UINT state, UINT term) const {
  return getGrammarResult().m_stateResult[state].m_actions.getAction(term);
}

UINT GrammarTables::getSuccessor(UINT state, UINT nterm) const {
  return getGrammarResult().m_stateResult[state].m_succs.getSuccessor(nterm);
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

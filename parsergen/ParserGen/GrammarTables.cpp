#include "stdafx.h"
#include "GrammarCode.h"
#include "GrammarTables.h"

GrammarTables::GrammarTables(const GrammarCode &grammarCode)
: m_grammarCode(grammarCode)
, m_compressibleStateSet(grammarCode.getGrammar().getStateCount())
{
  m_countTableBytes.clear();

  const Grammar &grammar = getGrammar();
  const UINT productionCount = getProductionCount();
  for(UINT p = 0; p < productionCount; p++) {
    const Production &prod   = grammar.getProduction(p);
    const UINT        length = prod.getLength();
    m_productionLength.add(length);
    m_left.add(prod.m_leftSide);
    m_rightSide.add(CompactUIntArray(length));
    CompactUIntArray &rightside = m_rightSide.last();
    for(UINT i = 0; i < length; i++) {
      rightside.add(prod.m_rightSide[i]);
    }
  }

  const UINT symbolCount = getSymbolCount();
  m_symbolNameArray.setCapacity(symbolCount);
  for(UINT symbol = 0; symbol < symbolCount; symbol++) {
    m_symbolNameArray.add(grammar.getSymbolName(symbol));
  }

  const UINT stateCount = getStateCount();
  m_actionMatrix.setCapacity(   stateCount);
  m_successorMatrix.setCapacity(stateCount);
  for(UINT s = 0; s < stateCount; s++) {
    m_actionMatrix.add(   grammar.m_result[s].m_actions);
    m_successorMatrix.add(grammar.m_result[s].m_succs);
  }
  initCompressibleStateSet();
}

const Grammar &GrammarTables::getGrammar() const {
  return getGrammarCode().getGrammar();
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
  const ParserActionArray &aa    = m_actionMatrix[state];
  const UINT               count = (UINT)aa.size();
  switch(count) {
  case 0 :
    throwException(_T("ActionArray for state %u has size 0"), state);
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


int GrammarTables::getAction(UINT state, UINT term) const {
  const ParserActionArray &paa = m_actionMatrix[state];
  for(ParserAction pa : paa) {
    if(pa.m_term == term) {
      return pa.m_action;
    }
  }
  return _ParserError;
}

UINT GrammarTables::getSuccessor(UINT state, UINT nterm) const {
  const SuccessorStateArray &ssa = m_successorMatrix[state];
  for(SuccessorState ss : ssa) {
    if(ss.m_nterm == nterm) {
      return ss.m_newState;
    }
  }
  return _ParserError;
}

void GrammarTables::getLegalInputs(UINT state, UINT *symbols) const {
  const ParserActionArray &paa = m_actionMatrix[state];
  for(ParserAction pa : paa) {
    *(symbols++) = pa.m_term;
  }
}

void GrammarTables::getLegalNTerms(UINT state, UINT *symbols) const {
  const SuccessorStateArray &ssa = m_successorMatrix[state];
  for(SuccessorState ss : ssa) {
    *(symbols++) = ss.m_nterm;
  }
};

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

void GrammarTables::getRightSide(UINT prod, UINT *dst) const {
  const CompactUIntArray &r = m_rightSide[prod];
  for(UINT s : r) {
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

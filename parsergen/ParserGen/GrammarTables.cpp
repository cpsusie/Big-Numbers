#include "stdafx.h"
#include "GrammarCode.h"

GrammarTables::GrammarTables(const Grammar &grammar, const String &tablesClassName, const String &parserClassName)
: m_compressibleStateSet(grammar.getStateCount())
, m_tablesClassName(tablesClassName)
, m_parserClassName(parserClassName)
{
  m_terminalCount   = grammar.getTerminalCount();
  m_countTableBytes.reset();

  for(int p = 0; p < grammar.getProductionCount(); p++) {
    const Production &prod = grammar.getProduction(p);
    const int length = prod.getLength();
    m_productionLength.add(length);
    m_left.add(prod.m_leftSide);
    m_rightSide.add(CompactIntArray());
    CompactIntArray &rightside = m_rightSide.last();
    for(int i = 0; i < length; i++) {
      rightside.add(prod.m_rightSide[i]);
    }
  }
  for(int s = 0; s < grammar.getSymbolCount(); s++) {
    const GrammarSymbol &symbol = grammar.getSymbol(s);
    m_symbolName.add(symbol.m_name);
//    m_symboltypes.add(symbol.m_type);
  }

  for(size_t i = 0; i < grammar.m_result.size(); i++) {
    m_stateActions.add(grammar.m_result[i].m_actions);
    m_stateSucc.add(grammar.m_result[i].m_succs);
  }
  initCompressibleStateSet();
}

int GrammarTables::getMaxInputCount() const {
  int m = 0;
  for(unsigned int s = 0; s < getStateCount(); s++) {
    int count = getLegalInputCount(s);
    if(count > m) {
      m = count;
    }
  }
  return m;
}

BitSet GrammarTables::getLookaheadSet(unsigned int state) const {
  BitSet             result(m_terminalCount);
  const ActionArray &actions     = m_stateActions[state];
  const size_t       actionCount = actions.size();
  for(size_t a = 0; a < actionCount; a++) {
    result.add(actions[a].m_token);
  }
  return result;
}

ByteArray GrammarTables::bitSetToByteArray(const BitSet &set) { // static
  ByteArray result;
  const size_t capacity = set.getCapacity();
  BYTE b    = 0;
  BYTE mask = 1;
  for(size_t i = 0; i < capacity; i++) {
    if(set.contains(i)) {
      b |= mask;
    }
    if((mask <<= 1) == 0) {
      result.append(b);
      mask = 1;
      b    = 0;
    }
  }
  if(mask != 1) {
    result.append(b);
  }
  return result;
}

void GrammarTables::initCompressibleStateSet() {
  const int stateCount = getStateCount();
  for(int state = 0; state < stateCount; state++) {
    if(calcIsCompressibleState(state)) {
      m_compressibleStateSet.add(state);
    }
  }
}

// Returns true if actionArray.size == 1 or all actions in the specified state is reduce by the same production
bool GrammarTables::calcIsCompressibleState(unsigned int state) const {
  const ActionArray &actions = m_stateActions[state];
  const size_t       count   = actions.size();
  switch(count) {
  case 0 :
    throwException(_T("actionArray for state %d has size 0"), state);
  case 1 :
    return true;
  default:
    { const int action = actions[0].m_action;
      if(action >= 0) {
        return false;
      }
      for(size_t i = 1; i < count; i++) {
        if(actions[i].m_action != action) {
          return false;
        }
      }
      return true;
    }
  }
}

ByteCount GrammarTables::wordAlignedSize(const ByteCount &c, UINT n) { // static
  return (n *c).getAlignedSize();
}

ByteCount GrammarTables::wordAlignedSize(int size) { // static
  return wordAlignedSize(ByteCount(size,size),1);
}

ByteCount GrammarTables::getTotalSizeInBytes(bool useTableCompression) const {
  if(!m_countTableBytes.isEmpty()) {
    return m_countTableBytes;
  }

  const UINT tableTypeSize = getTableTypeSize();

  const ByteCount compressedSetSize = wordAlignedSize(sizeof(unsigned char) * ((getStateCount()-1)/8+1));

  m_countTableBytes += compressedSetSize + m_compressedLASetBytes + m_uncompressedStateBytes;

  for(size_t s = 0; s < m_stateSucc.size(); s++) {
    m_countTableBytes += wordAlignedSize((2 * (UINT)m_stateSucc[s].size() + 1) * tableTypeSize);
  }

  UINT rightSideItems = 0;
  for(size_t p = 0; p < m_rightSide.size(); p++) {
    rightSideItems += (UINT)m_rightSide[p].size();
  }

  m_countTableBytes += wordAlignedSize(rightSideItems * tableTypeSize); // rightSideTable

  unsigned int countStringBytes = 0;
  for(size_t s = 0; s < m_symbolName.size(); s++) {
    countStringBytes += sizeof(TCHAR)*((UINT)m_symbolName[s].length() + 1);
  }

  m_countTableBytes += 2 * wordAlignedSize(ByteCount::s_pointerSize,getStateCount())  // action + successor
                     + wordAlignedSize(sizeof(char)      * getProductionCount())      // productionLength
                     + wordAlignedSize(tableTypeSize     * getProductionCount())      // leftSide
                     + wordAlignedSize(countStringBytes)                              // symbolname[i]
                     + wordAlignedSize(ByteCount::s_pointerSize    ,getSymbolCount()) // symbolname array
                     + wordAlignedSize(ByteCount::s_pointerSize,    8               ) // PAshorttable/PAchartables + PAtables
                     + wordAlignedSize(sizeof(short)*4);                              // noorterminals, symbolCount, productionCount, stateCount

  return m_countTableBytes;
}

int GrammarTables::getSuccessor(unsigned int state, int nt) const {
  const ActionArray &list = m_stateSucc[state];
  for(size_t i = 0; i < list.size(); i++) {
    if(list[i].m_token == nt) {
      return list[i].m_action;
    }
  }
  return _ParserError;
}

int GrammarTables::getAction(unsigned int state, int input) const {
  const ActionArray &list = m_stateActions[state];
  for(size_t i = 0; i < list.size(); i++) {
    if(list[i].m_token == input) {
      return list[i].m_action;
    }
  }
  return _ParserError;
}

void GrammarTables::getLegalInputs(unsigned int state, unsigned int *symbols) const {
  const ActionArray &list = m_stateActions[state];
  for(size_t i = 0; i < list.size(); i++) {
    symbols[i] = list[i].m_token;
  }
}

void GrammarTables::getRightSide(unsigned int prod, unsigned int *dst) const {
  const CompactIntArray &r = m_rightSide[prod];
  for(size_t i = 0; i < r.size(); i++) {
    dst[i] = r[i];
  }
}

void GrammarTables::print(MarginFile &output, Language language, bool useTableCompression) const {
  switch(language) {
  case CPP :
    printCpp(output, useTableCompression);
    break;
  case JAVA:
    printJava(output, useTableCompression);
    break;
  }
}

ByteCount GrammarTables::printByteArray(MarginFile &output, const String &name, const ByteArray &ba, UINT bytesPerLine, const StringArray *linePrefix) const {
  const UINT nBytes        = (UINT)ba.size();
  const bool hasLinePrefix = (linePrefix != NULL);
  output.setLeftMargin(0);
  output.printf(_T("static const BYTE %s[%u] = {"), name.cstr(), nBytes);
  TCHAR *delim = _T(" ");
  output.setLeftMargin(2);
  if((nBytes > bytesPerLine) || !hasLinePrefix) {
    output.printf(_T("\n"));
  }
  bool printLinePrefix = hasLinePrefix;
  size_t lineCount = 0;
  for(UINT i = 0; i < nBytes; i++, delim = _T(",")) {
    if(printLinePrefix && (lineCount < linePrefix->size())) {
      output.printf(_T("/* %s */ "), (*linePrefix)[lineCount++].cstr());
      printLinePrefix = false;
    }
    output.printf(_T("%s0x%02x"),delim, ba[i]);
    if(((i % bytesPerLine) == (bytesPerLine-1)) && (i < (nBytes-1))) {
      output.printf(_T("\n"));
      printLinePrefix = hasLinePrefix;
    }
  }
  output.setLeftMargin(0);
  const ByteCount byteCount = wordAlignedSize(nBytes*sizeof(unsigned char));
  output.printf(_T("\n}; // Size of table:%s.\n"), byteCount.toString().cstr());
  return byteCount;
}

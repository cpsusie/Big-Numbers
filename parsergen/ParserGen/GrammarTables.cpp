#include "stdafx.h"
#include <limits>
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

BitSet GrammarTables::getLookaheadSet(UINT state) const {
  BitSet             result(m_terminalCount);
  const ActionArray &actions     = m_stateActions[state];
  const size_t       actionCount = actions.size();
  for(size_t a = 0; a < actionCount; a++) {
    result.add(actions[a].m_token);
  }
  return result;
}

const TCHAR *GrammarTables::getTableTypeName(bool isShort) { // static
  return isShort ? _T("short") : _T("char");
}

IndexType GrammarTables::findIndexType(UINT maxValue) { // static
  if(maxValue < UCHAR_MAX-1) {
    return INDEXTYPE_BYTE;
  } else if(maxValue < USHRT_MAX-1) {
    return INDEXTYPE_USHORT;
  } else {
    return INDEXTYPE_UINT;
  }
}
const TCHAR *GrammarTables::getIndexTypeName(IndexType type) { // static
  switch(type) {
  case INDEXTYPE_BYTE  : return _T("unsigned char" );
  case INDEXTYPE_USHORT: return _T("unsigned short");
  default              : return _T("unsigned int"  );
  }
}
UINT GrammarTables::getIndexTypeSize(IndexType type) { // static
  switch(type) {
  case INDEXTYPE_BYTE  : return sizeof(BYTE  );
  case INDEXTYPE_USHORT: return sizeof(USHORT);
  default              : return sizeof(UINT  );
  }
}

ByteArray GrammarTables::bitSetToByteArray(const BitSet &set) { // static
  const size_t capacity = set.getCapacity();
  ByteArray    result((capacity-1)/8+1);
  BYTE         b    = 0;
  BYTE         mask = 1;
  for(size_t i = 0; i < capacity; i++) {
    if(set.contains(i)) {
      b |= mask;
    }
    if((mask <<= 1) == 0) {
      result.add(b);
      mask = 1;
      b    = 0;
    }
  }
  if(mask != 1) {
    result.add(b);
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

ByteCount GrammarTables::wordAlignedSize(UINT size) { // static
  return wordAlignedSize(ByteCount(size,size),1);
}

ByteCount GrammarTables::getTotalSizeInBytes() const {
  return ByteCount(getTableByteCount(PLATFORM_X86), getTableByteCount(PLATFORM_X64));
}

int GrammarTables::getSuccessor(UINT state, UINT nt) const {
  const ActionArray &list = m_stateSucc[state];
  for(size_t i = 0; i < list.size(); i++) {
    if(list[i].m_token == nt) {
      return list[i].m_action;
    }
  }
  return _ParserError;
}

int GrammarTables::getAction(UINT state, UINT input) const {
  const ActionArray &list = m_stateActions[state];
  for(size_t i = 0; i < list.size(); i++) {
    if(list[i].m_token == input) {
      return list[i].m_action;
    }
  }
  return _ParserError;
}

void GrammarTables::getLegalInputs(UINT state, UINT *symbols) const {
  const ActionArray &list = m_stateActions[state];
  for(size_t i = 0; i < list.size(); i++) {
    symbols[i] = list[i].m_token;
  }
}

UINT GrammarTables::getTableByteCount(Platform platform) const {
  return m_countTableBytes.getByteCount(platform);
}

void GrammarTables::getRightSide(UINT prod, UINT *dst) const {
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

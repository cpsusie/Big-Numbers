#include "pch.h"
#include "DFA.h"

DFATables::DFATables(const DFATables &src) {
  init();
  copy(src);
}

DFATables &DFATables::operator=(const DFATables &src) {
  clear();
  copy(src);
  return *this;
}

DFATables::~DFATables() {
  clear();
}

void DFATables::init() {
  m_stateCount         = 0;
  m_rowCount           = 0;
  m_columnCount        = 0;
  m_charMapSize        = 0;
  m_charMap            = nullptr;
  m_stateMap           = nullptr;
  m_transitionMatrix   = nullptr;
  m_acceptStates       = nullptr;
}

void DFATables::copy(const DFATables &src) {
  if(!src.isEmpty()) {
    m_stateCount       = src.m_stateCount;
    m_rowCount         = src.m_rowCount;
    m_columnCount      = src.m_columnCount;
    m_charMapSize      = src.m_charMapSize;
    m_charMap          = src.m_charMap->clone();
    m_stateMap         = src.m_stateMap->clone();
    m_transitionMatrix = src.m_transitionMatrix->clone();
    m_acceptStates     = src.m_acceptStates->clone();
  }
}

void DFATables::clear() {
  SAFEDELETE(m_charMap         );
  SAFEDELETE(m_stateMap        );
  SAFEDELETE(m_transitionMatrix);
  SAFEDELETE(m_acceptStates    );
  init();
}


#if defined(_DEBUG)
#define MEMUSAGE(a) ((a)?a->getMemoryUsage():0)
UINT DFATables::getMemoryUsage() const {
  return (UINT)(sizeof(DFATables) + MEMUSAGE(m_charMap) + MEMUSAGE(m_stateMap) + MEMUSAGE(m_transitionMatrix) + MEMUSAGE(m_acceptStates));
}

static CharacterFormater &getCharFormater() {
  return *CharacterFormater::hexEscapedExtendedAsciiFormater;
}

static String thinCharMapToString(const FixedIntArray &a) {
  String result = _T("EOI = 0\n");
  const UINT size = a.size();
  CharacterFormater &charFormater = getCharFormater();
  for(UINT ch = 0; ch < size; ch++) {
    const int v = a[ch];
    if(v) {
      result += format(_T("'%s' = %d\n"), charFormater.toString(ch).cstr(), v);
    }
  }
  return result;
}

static String thinMapToString(const FixedIntArray &a) {
  const UINT sz = a.size();
  BitSet     tmp(sz);
  for(UINT i = 0; i < sz; i++) {
    const int v = a[i];
    if(v) {
      tmp.add(i);
    }
  }
  return tmp.toString();
}

#define KB 1024
#define MB (KB*KB)

static String byteCountToString(size_t byteCount) {
  if(byteCount < KB) {
    return format(_T("%zu bytes"), byteCount);
  } else if(byteCount < MB) {
    return format(_T("%.2lfKb"), (double)byteCount / KB);
  } else {
    return format(_T("%.2lfMb"), (double)byteCount / MB);
  }
}

String DFATables::toString() const {
  if(isEmpty()) {
    return EMPTYSTRING;
  }
  String result;
  if(m_charMap->countNonZeroes() <= 40) {
    result = format(_T("Character map:\n%s\n"), indentString(thinCharMapToString(*m_charMap),2).cstr());
  } else {
    result = format(_T("Character map:\n%s\n"), indentString(m_charMap->toString(40),2).cstr());
  }
  result += format(_T("State map:\n%s\n"), indentString(m_stateMap->toString(10),2).cstr());
  result += _T("TransitionMatrix:\n");

  String matStr = _T("    ");
  for(UINT c = 0; c < m_columnCount; c++) matStr += format(_T("%4u"), c);
  matStr += _T("\n");
  for(UINT i = 0, trIndex = 0; i < m_rowCount; i++) {
    String line  = format(_T("(%2zu)"),i);
    const TCHAR *delim = _T(" ");
    for(UINT j = 0; j < m_columnCount; j++, trIndex++, delim = _T(",")) {
      line += format(_T("%s%3d"), delim, (*m_transitionMatrix)[trIndex]);
    }
    line   += _T("\n");
    matStr += line;
  }
  result += indentString(matStr, 2);
  result += NEWLINE;
  result += format(_T("Accept states:\n%s"), indentString(acceptStatesToString(),2).cstr());
  return format(_T("DFATables(stateCount:%zu, memoryUsage:%s):\n%s\n")
               ,m_stateCount
               ,byteCountToString(getMemoryUsage()).cstr()
               ,indentString(result, 2).cstr());
}

String DFATables::acceptStatesToString() const {
  String s1 = _T("State:"), s2 = _T("Value:");
  const TCHAR *delim = nullptr;
  const UINT   size  = m_acceptStates->size();
  for(UINT s = 0; s < size; s++) {
    const int v = (*m_acceptStates)[s];
    if(v >= 0) {
      if(delim) {
        s1 += delim;
        s2 += delim;
      } else {
        delim = _T(",");
      }
      s1 += format(_T("%3zu"), s);
      s2 += format(_T("%3u" ), v);
    }
  }
  return format(_T("%s\n%s\n"), s1.cstr(), s2.cstr());
}
#endif // _DEBUG

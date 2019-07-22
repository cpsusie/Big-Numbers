#include "stdafx.h"
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
  m_memoryUsage      = sizeof(DFATables);
  m_stateCount       = 0;
  m_rowCount         = 0;
  m_columnCount      = 0;
  m_charMap          = NULL;
  m_stateMap         = NULL;
  m_transitionMatrix = NULL;
  m_acceptStates     = NULL;
}

void DFATables::copy(const DFATables &src) {
  if(src.m_stateCount) {
    m_memoryUsage = src.m_memoryUsage;
    allocate(      src.m_stateCount);
    allocateMatrix(src.m_rowCount, src.m_columnCount);
    memcpy(m_charMap         , src.m_charMap         , MAX_CHARS                     * sizeof(m_charMap[0])         );
    memcpy(m_stateMap        , src.m_stateMap        , m_stateCount                  * sizeof(m_stateMap[0])        );
    memcpy(m_transitionMatrix, src.m_transitionMatrix, m_rowCount * m_columnCount    * sizeof(m_transitionMatrix[0]));
    memcpy(m_acceptStates    , src.m_acceptStates    , m_stateCount                  * sizeof(m_acceptStates[0])    );
  }
}

void DFATables::allocate(size_t stateCount) {
  clear();
  m_stateCount   = stateCount;
  m_charMap      = new short[MAX_CHARS  ]; TRACE_NEW(m_charMap     );
  m_stateMap     = new short[stateCount ]; TRACE_NEW(m_stateMap    );
  m_acceptStates = new short[stateCount ]; TRACE_NEW(m_acceptStates);
  m_memoryUsage += MAX_CHARS * sizeof(m_charMap[0])
                + stateCount * sizeof(m_stateMap[0])
                + stateCount * sizeof(m_acceptStates[0]);
}

void DFATables::allocateMatrix(size_t rowCount, size_t columnCount) {
  m_rowCount         = rowCount;
  m_columnCount      = columnCount;
  const size_t elemCount = m_rowCount * m_columnCount;
  m_transitionMatrix = new short[elemCount]; TRACE_NEW(m_transitionMatrix);
  m_memoryUsage += elemCount * sizeof(m_transitionMatrix[0]);
}

void DFATables::clear() {
  SAFEDELETEARRAY(m_charMap         );
  SAFEDELETEARRAY(m_stateMap        );
  SAFEDELETEARRAY(m_transitionMatrix);
  SAFEDELETEARRAY(m_acceptStates    );
  init();
}

template<class T> int countNonZeroes(const T *a, size_t size) {
  int count = 0;
  while(size--) {
    if(*(a++)) count++;
  }
  return count;
}

template<class T> String arrayToString(const T *a, size_t size, size_t maxPerLine) {
  String result;
  for(size_t i = 0, j = 1; i < size; i++, j++) {
    if((size > maxPerLine) && (j == 1)) {
      result += format(_T("(%3u) "), (UINT)i);
    }
    result += format(_T("%3d"), a[i]);
    if (i < size - 1) {
      result += _T(",");
    }
    if((j == maxPerLine) || (i == size - 1)) {
      result += NEWLINE;
      j = 0;
    }
  }
  return result;
}

static CharacterFormater &getCharFormater() {
  return *CharacterFormater::hexEscapedExtendedAsciiFormater;
}

static String thinCharMapToString(const short *a) { // size = MAX_CHARS
  String result = _T("EOI = 0\n");
  CharacterFormater &charFormater = getCharFormater();
  for(int ch = 0; ch < MAX_CHARS; ch++, a++) {
    if(*a) {
      result += format(_T("'%s' = %d\n"), charFormater.toString(ch).cstr(), *a);
    }
  }
  return result;
}

template<class T> String thinMapToString(const T *a, size_t size) {
  String result;
  BitSet tmp(size);
  for(size_t i = 0; i < size; i++, a++) {
    if(*a) {
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
  if(countNonZeroes(m_charMap, MAX_CHARS) <= 40) {
    result = format(_T("Character map:\n%s\n"), indentString(thinCharMapToString(m_charMap),2).cstr());
  } else {
    result = format(_T("Character map:\n%s\n"), indentString(arrayToString(m_charMap, MAX_CHARS, 40),2).cstr());
  }
  result += format(_T("State map:\n%s\n"), indentString(arrayToString(m_stateMap, m_stateCount, 10),2).cstr());
  result += _T("TransitionMatrix:\n");
  String matStr = _T("    ");
  for(UINT c = 0; c < m_columnCount; c++) {
    matStr += format(_T("%4u"), c);
  }
  matStr += _T("\n");
  for(size_t i = 0; i < m_rowCount; i++) {
    matStr += format(_T("(%2zu) %s"), i, arrayToString(&transition((UINT)i, 0), m_columnCount, 50).cstr());
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
  const TCHAR *delim = NULL;
  for(size_t s = 0; s < m_stateCount; s++) {
    const short v = m_acceptStates[s];
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

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
  m_stateCount        = 0;
  m_rowCount          = 0;
  m_columnCount       = 0;
  m_charMap           = NULL;
  m_stateMap          = NULL;
  m_transitionMatrix  = NULL;
  m_acceptTable       = NULL;
}

void DFATables::copy(const DFATables &src) {
  if(src.m_stateCount) {
    allocate(src.m_stateCount);
    memcpy(m_charMap    , src.m_charMap    , MAX_CHARS    * sizeof(m_charMap[0])    );
    memcpy(m_stateMap   , src.m_stateMap   , m_stateCount * sizeof(m_stateMap[0])   );
    memcpy(m_acceptTable, src.m_acceptTable, m_stateCount * sizeof(m_acceptTable[0]));

    allocateMatrix(src.m_rowCount, src.m_columnCount);
    memcpy(m_transitionMatrix, src.m_transitionMatrix, sizeof(m_transitionMatrix[0]) * m_rowCount * m_columnCount);
  }
}

void DFATables::allocate(size_t stateCount) {
  clear();
  m_stateCount  = stateCount;
  m_charMap     = new short[MAX_CHARS ]; TRACE_NEW(m_charMap    );
  m_stateMap    = new short[stateCount]; TRACE_NEW(m_stateMap   );
  m_acceptTable = new BYTE[stateCount ]; TRACE_NEW(m_acceptTable);
}

void DFATables::allocateMatrix(size_t rowCount, size_t columnCount) {
  m_rowCount         = rowCount;
  m_columnCount      = columnCount;
  m_transitionMatrix = new short[m_rowCount * m_columnCount]; TRACE_NEW(m_transitionMatrix);
}

void DFATables::clear() {
  SAFEDELETEARRAY(m_charMap);
  SAFEDELETEARRAY(m_stateMap);
  SAFEDELETEARRAY(m_acceptTable);
  SAFEDELETEARRAY(m_transitionMatrix);
  init();
}

#ifdef _DEBUG

template<class T> int countNonZeroes(const T *a, size_t size) {
  int count = 0;
  while(size--) {
    if(*(a++)) count++;
  }
  return count;
}

template<class T> String arrayToString(const T *a, size_t size, size_t maxPerLine) {
  String result;
  for(size_t i = 0, j = 1; i < size; i++,j++) {
    if((size > maxPerLine) && (j == 1)) {
      result += format(_T("(%3u) "), (UINT)i);
    }
    result += format(_T("%3d"), a[i]);
    if(i < size-1) {
      result += _T(",");
    }
    if((j == maxPerLine) || (i == size-1)) {
      result += NEWLINE;
      j = 0;
    }
  }
  return result;
}

static String thinCharMapToString(const short *a) { // size = MAX_CHARS
  String result = _T("EOI = 0\n");
  for(int ch = 0; ch < MAX_CHARS; ch++,a++) {
    if(*a) {
      result += format(_T("'%s' = %d\n"), NFAState::getFormater()->toString(ch).cstr(), *a);
    }
  }
  return result;
}

template<class T> String thinMapToString(const T *a, size_t size) {
  String result;
  BitSet tmp(size);
  for(size_t i = 0; i < size; i++,a++) {
    if(*a) {
      tmp.add(i);
    }
  }
  return tmp.toString();
}

String DFATables::toString() const {
  if(m_stateCount == 0) {
    return EMPTYSTRING;
  }
  String result;
  if(countNonZeroes(m_charMap, MAX_CHARS) <= 40) {
    result = format(_T("Character map:\n%s\n"), thinCharMapToString(m_charMap).cstr());
  } else {
    result = format(_T("Character map:\n%s\n"), arrayToString(m_charMap , MAX_CHARS, 40).cstr());
  }
  result += format(_T("State map:\n%s\n"), arrayToString(m_stateMap, m_stateCount, 10).cstr());
  result += _T("TransitionMatrix:\n    ");
  for(UINT c = 0; c < m_columnCount; c++) {
    result += format(_T("%4u"),c);
  }
  result += _T("\n");
  for(size_t i = 0; i < m_rowCount; i++) {
    result += format(_T("(%2u) %s"), (UINT)i, arrayToString(&transition((UINT)i,0), m_columnCount, 50).cstr());
  }
  result += NEWLINE;
  if(countNonZeroes(m_acceptTable, m_stateCount) <= 20) {
    result += format(_T("Accept states:\n%s"), thinMapToString(m_acceptTable, m_stateCount).cstr());
  } else {
    result += format(_T("AcceptTable:\n%s"), arrayToString(m_acceptTable, m_stateCount, 10).cstr());
  }
  return result;
}
#endif

  // Compress and output transitionmatrix eliminating equal columns and rows
void DFA::getDFATables(DFATables &tables) const {
  tables.clear();

  if(m_states.isEmpty()) {
    return;
  }
  tables.allocate(m_states.size());
  BitSet  columnSave(MAX_CHARS);       // columns that will remain in table
  BitSet  rowSave(m_states.size());    // rows    that will remain in table

  reduce(tables, rowSave, columnSave); // Compress the tables

  getTransitionMatrix(tables, rowSave, columnSave);
  getAcceptTable(tables);

//  redirectDebugLog();
//  debugLog(_T("DFA-tables:\n%s"), tables.toString().cstr());
}

// Return true if two columns in transitionmatrix are equal, else return false
bool DFA::columnsEqual(size_t col1, size_t col2) const {
  for(size_t i = 0; i < m_states.size(); i++) {
    if(m_states[i].m_transition[col1] != m_states[i].m_transition[col2]) {
      return false;
    }
  }
  return true;
}

  // Return true if two rows in transitionmatrix are equal, else return false
bool DFA::rowsEqual(size_t row1, size_t row2) const {
  return memcmp(m_states[row1].m_transition, m_states[row2].m_transition, sizeof(DFAtrans)) == 0;
}

void DFA::reduce(DFATables &tables, BitSet &rowSave, BitSet &columnSave) const {
  size_t i;
  for(i = 0; i < MAX_CHARS; i++) {
    tables.m_charMap[i] = -1;
  }
  for(int r_ncols = 0;;r_ncols++) {
    for(i = r_ncols; (i < MAX_CHARS) && (tables.m_charMap[i] != -1); i++);
    if(i >= MAX_CHARS) {
      break;
    }
    columnSave.add(i);
    tables.m_charMap[i] = r_ncols;

    for(size_t j = i + 1; j < MAX_CHARS; j++) {
      if(tables.m_charMap[j] == -1 && columnsEqual(i, j)) {
        tables.m_charMap[j] = r_ncols;
      }
    }
  }
  const size_t stateCount = m_states.size();
  for(i = 0; i < stateCount; i++) {
    tables.m_stateMap[i] = -1;
  }
  for(int r_nrows = 0;; r_nrows++ ) {
    for(i = r_nrows; (i < stateCount) && (tables.m_stateMap[i] != -1); i++);
    if(i >= stateCount) {
      break;
    }
    rowSave.add(i);
    tables.m_stateMap[i] = r_nrows;
    for(size_t j = i+1; j < stateCount; j++) {
      if(tables.m_stateMap[j] == -1 && rowsEqual(i, j)) {
        tables.m_stateMap[j] = r_nrows;
      }
    }
  }
}

void DFA::getTransitionMatrix(DFATables &tables, const BitSet &rowSave, const BitSet &columnSave) const {
  const size_t rowCount = rowSave.size();
  const size_t colCount = columnSave.size();

  tables.allocateMatrix(rowCount, colCount);
  CompactIntArray columnsIndex;
  for(Iterator<size_t> cit = ((BitSet&)columnSave).getIterator(); cit.hasNext();) {
    columnsIndex.add((int)cit.next());
  }

  int r = 0;
  for(Iterator<size_t> rit = ((BitSet&)rowSave).getIterator(); rit.hasNext(); r++) {
    const int *DFAtransitions = m_states[rit.next()].m_transition;
    short     *row            = &tables.transition(r, 0);
    for(UINT c = 0; c < colCount; c++) {
      *(row++) = DFAtransitions[columnsIndex[c]];
    }
  }
}

void DFA::getAcceptTable(DFATables &tables) const {
  for(size_t s = 0; s < m_states.size(); s++) {
    tables.m_acceptTable[s] = m_states[s].m_accept.m_acceptAttribute;
  }
}

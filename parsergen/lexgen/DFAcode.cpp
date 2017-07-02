#include "stdafx.h"
#include "DFA.h"

  // Compress and output transitionmatrix eliminating equal columns and rows
void DFA::printTables(MarginFile &f) const {
  int    *columnMap = new int[MAX_CHARS];
  int    *rowMap    = new int[m_states.size()];
  BitSet  columnSave(MAX_CHARS);       // columns that will remain in table
  BitSet  rowSave(m_states.size());    // rows    that will remain in table

  reduce(rowMap, columnMap, rowSave, columnSave); // Compress the tables

  printCharMap( f, columnMap);
  printStateMap(f, rowMap);

  printTransitionMatrix(f, rowSave, columnSave);
  printAcceptTable(f);
  delete[] columnMap;
  delete[] rowMap;
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

void DFA::reduce(int *rowMap, int *columnMap, BitSet &rowSave, BitSet &columnSave) const {
  unsigned int i;

  for(i = 0; i < MAX_CHARS; i++) {
    columnMap[i] = -1;
  }

  for(int r_ncols = 0;;r_ncols++) {
    for(i = r_ncols;  columnMap[i] != -1  && i < MAX_CHARS; i++ );

    if(i >= MAX_CHARS) {
      break;
    }

    columnSave.add(i);
    columnMap[i] = r_ncols;

    for(int j = i + 1; j < MAX_CHARS; j++) {
      if(columnMap[j] == -1 && columnsEqual(i, j)) {
        columnMap[j] = r_ncols;
      }
    }
  }

  for(i = 0; i < m_states.size(); i++) {
    rowMap[i] = -1;
  }

  for(int r_nrows = 0 ;; r_nrows++ ) {
    for(i = r_nrows; rowMap[i] != -1  && i < m_states.size(); i++ );

    if(i >= m_states.size()) {
      break;
    }

    rowSave.add(i);
    rowMap[i] = r_nrows;

    for(size_t j = i+1; j < m_states.size(); j++) {
      if(rowMap[j]== -1 && rowsEqual(i, j)) {
        rowMap[j] = r_nrows;
      }
    }
  }
}

static int maxElement(const int *map, int size) {
  int m = 0;
  for(int i = 0; i < size; i++) {
    if(map[i] > m) {
      m = map[i];
    }
  }
  return m;
}

void DFA::printCharMap(MarginFile &f, const int *map) const {
  const TCHAR *text =
  _T("\n"
     "// The lexCharMap[] and lexStateMap arrays are used as follows:\n"
     "//\n"
     "// nextState = lexNext[lexStateMap[currentState]][lexCharMap[inputChar]];\n"
     "//\n"
     "// Character positions in the lexCharMap Array are:\n"
     "//\n");

  f.printf(_T("%s"), text);
  printCharMap(f);
  TCHAR *tableType;
  switch(m_language) {
  case CPP :
    tableType = (maxElement(map, MAX_CHARS) > 255) ? _T("unsigned short") : _T("unsigned char");
    f.printf(_T("static %s lexCharMap[%d] = {\n"), tableType, MAX_CHARS);
    break;
  case JAVA:
    tableType = (maxElement(map, MAX_CHARS) > 255) ? _T("short") : _T("byte");
    f.printf(_T("private static final %s lexCharMap[] = {\n"), tableType);
    break;
  }
  int oldMargin = f.getLeftMargin();
  f.setLeftMargin(oldMargin+4);
  for(int i = 0; i < MAX_CHARS; i++) {
    f.printf(_T("%4d"), map[i]);
    if(i < MAX_CHARS - 1) {
      f.printf(_T(","));
    }
    if(i % 16 == 15 || i == MAX_CHARS - 1) {
      f.printf(_T("\n"));
    }
  }
  f.setLeftMargin(oldMargin);
  f.printf(_T("};\n\n"));
}

void DFA::printCharMap(MarginFile &f) const {
  f.printf(_T("//  "));
  for(UINT i = 0; i < MAX_CHARS; i++) {
    f.printf(((i % 16 == 15) || (i == MAX_CHARS-1)) ? _T("%s") : _T("%-4s "), binToAscii(i).cstr());
    if((i % 16 == 15) && (i < MAX_CHARS-1)) {
      f.printf(_T("\n//  "));
    }
  }
  f.printf(_T("\n\n"));
}

void DFA::printStateMap(MarginFile &f, const int *map) const {
  TCHAR *tableType;
  switch(m_language) {
  case CPP  :
    tableType = (maxElement(map, (int)m_states.size()) > 255) ? _T("unsigned short") : _T("unsigned char");
    f.printf(_T("static const %s lexStateMap[%d] = {\n"), tableType, m_states.size());
    break;
  case JAVA :
    tableType = (maxElement(map, (int)m_states.size()) > 255) ? _T("short") : _T("byte");
    f.printf(_T("private static final %s lexStateMap[] = {\n"), tableType);
    break;
  }

  int oldMargin = f.getLeftMargin();
  f.setLeftMargin(oldMargin+4);
  for(size_t i = 0; i < m_states.size(); i++) {
    if(i % 10 == 0) {
      f.printf(_T("/* %3d */"), (int)i);
    }
    f.printf(_T("%4d"), map[i]);
    if(i < m_states.size()-1) {
      f.printf(_T(","));
    }
    if(i % 10 == 9 || i == m_states.size()-1) {
      f.printf(_T("\n"));
    }
  }
  f.setLeftMargin(oldMargin);
  f.printf(_T("};\n\n"));
}

void DFA::minmaxElement(BitSet &rowSave, BitSet &columnSave, int &minElement, int &maxElement) const {
  minElement = maxElement = 0;
  for(Iterator<size_t> rit = rowSave.getIterator(); rit.hasNext();) {
    const int r = (int)rit.next();
    for(Iterator<size_t> cit = columnSave.getIterator(); cit.hasNext();) {
      const int c = (int)cit.next();
      int e = m_states[r].m_transition[c];
      if(e > maxElement) maxElement = e;
      if(e < minElement) minElement = e;
    }
  }
}

const TCHAR *DFA::findTransisitionType(BitSet &rowSave, BitSet &columnSave) const {
  int minElement, maxElement;
  minmaxElement(rowSave, columnSave, minElement, maxElement);
  if(minElement < -128 || maxElement > 127) {
    return _T("short");
  } else {
    return m_language == CPP ? _T("char") : _T("byte");
  }
}

void DFA::printTransitionMatrix(MarginFile &f, BitSet &rowSave, BitSet &columnSave) const {
  int rowCount = (int)rowSave.size();
  int colCount = (int)columnSave.size();

  const TCHAR *tableType = findTransisitionType(rowSave, columnSave);

  switch(m_language) {
  case CPP :
    f.printf(_T("static const %s lexNext[%d][%d] = {\n"), tableType, rowCount, colCount);
    break;
  case JAVA:
    f.printf(_T("private static final %s lexNext[][] = {\n"), tableType);
    break;
  }
  int oldMargin = f.getLeftMargin();
  f.setLeftMargin(oldMargin+4);

  if(colCount < 40) {
    int rCount = 0;
    for(Iterator<size_t> rit = rowSave.getIterator(); rit.hasNext(); rCount++) {
      int r = (int)rit.next();
      f.printf(_T("/* %3d */ {"), rCount);
      int cCount = 0;
      for(Iterator<size_t> cit = columnSave.getIterator(); cit.hasNext(); cCount++) {
        int c = (int)cit.next();
        if(cCount > 0) {
          f.printf(_T(","));
        }
        f.printf(_T("%2d"), m_states[r].m_transition[c]);
      }
      f.printf(_T("}"));
      if(rCount < rowCount-1) {
        f.printf(_T(",\n"));
      } else {
        f.printf(_T("\n"));
      }
    }
  } else {
    int rCount = 0;
    for(Iterator<size_t> rit = rowSave.getIterator(); rit.hasNext(); rCount++) {
      int r = (int)rit.next();
      f.printf(_T("/* %3d */ {"), rCount);
      int cCount = 0;
      for(Iterator<size_t> cit = columnSave.getIterator(); cit.hasNext(); cCount++) {
        int c = (int)cit.next();
        f.printf(_T("%4d"), m_states[r].m_transition[c]);
        if(cCount < colCount-1) {
          f.printf(_T(","));
        }
        if(cCount % 16 == 15) {
          f.printf(_T("\n           "));
        }
      }
      f.printf(_T("}"));
      if(rCount < rowCount-1) {
        f.printf(_T(",\n"));
      } else {
        f.printf(_T("\n"));
      }
    }
  }
  f.setLeftMargin(oldMargin);
  f.printf(_T("};\n\n"));
}

void DFA::printAcceptTable(MarginFile &f) const {
  switch(m_language) {
  case CPP:
    f.printf(_T("static const char lexAccept[] = {\n"));
    break;
  case JAVA:
    f.printf(_T("private static final byte lexAccept[] = {\n"));
    break;
  }

  int oldMargin = f.getLeftMargin();
  f.setLeftMargin(oldMargin+4);
  const size_t stateCount = m_states.size();
  for(size_t i = 0; i < stateCount; i++) {
    const DFAstate &state = m_states[i];
    if(i % 10 == 0) {
      f.printf(_T("/* %3d */"), i);
    }
    if(state.m_accept == NULL) {
      f.printf(_T("  0"));
    } else {
      f.printf(_T("  %d"), state.m_accept->m_anchor ? state.m_accept->m_anchor : 4);
    }
    if(i < stateCount-1) {
      f.printf(_T(","));
    }
    if((i % 10 == 9) || (i == stateCount - 1)) {
      f.printf(_T("\n"));
    }
  }
  f.setLeftMargin(oldMargin);
  f.printf(_T("};\n\n"));
}

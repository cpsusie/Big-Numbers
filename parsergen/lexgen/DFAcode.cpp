#include "stdafx.h"
#include <ByteCount.h>
#include "DFA.h"

void DFA::printTables(MarginFile &f) const {
  int    *columnMap = new int[MAX_CHARS];       TRACE_NEW(columnMap);
  int    *rowMap    = new int[getStateCount()]; TRACE_NEW(rowMap   );
  BitSet  columnSave(MAX_CHARS);       // columns that will remain in table
  BitSet  rowSave(getStateCount());    // rows    that will remain in table

  reduce(rowMap, columnMap, rowSave, columnSave); // Compress the tables

  printCharMap( f, columnMap);
  printStateMap(f, rowMap);

  printTransitionMatrix(f, rowSave, columnSave);
  printAcceptTable(f);
  SAFEDELETEARRAY(columnMap);
  SAFEDELETEARRAY(rowMap   );
}

bool DFA::columnsEqual(size_t col1, size_t col2) const {
  const UINT stateCount = getStateCount();
  for(UINT i = 0; i < stateCount; i++) {
    if(m_states[i].m_transition[col1] != m_states[i].m_transition[col2]) {
      return false;
    }
  }
  return true;
}

bool DFA::rowsEqual(size_t row1, size_t row2) const {
  return memcmp(m_states[row1].m_transition, m_states[row2].m_transition, sizeof(DFAtrans)) == 0;
}

void DFA::reduce(int *rowMap, int *columnMap, BitSet &rowSave, BitSet &columnSave) const {
  const UINT stateCount = getStateCount();

  for(UINT i = 0; i < MAX_CHARS; i++) {
    columnMap[i] = -1;
  }


  for(UINT r_ncols = 0;;r_ncols++) {
    UINT i;
    for(i = r_ncols; (columnMap[i] != -1) && (i < MAX_CHARS); i++ );

    if(i >= MAX_CHARS) {
      break;
    }

    columnSave.add(i);
    columnMap[i] = r_ncols;

    for(UINT j = i + 1; j < MAX_CHARS; j++) {
      if(columnMap[j] == -1 && columnsEqual(i, j)) {
        columnMap[j] = r_ncols;
      }
    }
  }

  for(UINT i = 0; i < stateCount; i++) {
    rowMap[i] = -1;
  }

  for(UINT r_nrows = 0;;r_nrows++) {
    UINT i;
    for(i = r_nrows; (rowMap[i] != -1) && (i < stateCount); i++);

    if(i >= stateCount) {
      break;
    }

    rowSave.add(i);
    rowMap[i] = r_nrows;

    for(UINT j = i+1; j < stateCount; j++) {
      if((rowMap[j] == -1) && rowsEqual(i, j)) {
        rowMap[j] = r_nrows;
      }
    }
  }
}

static int findMaxValue(const int *map, UINT size) {
  int m = 0;
  for(const int *p = map, *endp = p + size; p < endp; p++) {
    if(*p > m) {
      m = *p;
    }
  }
  return m;
}

void DFA::printCharMap(MarginFile &f, const int *map) const {
  const Options &options = Options::getInstance();
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
  const IntegerType tableType = findIntType(0,findMaxValue(map, MAX_CHARS));
  switch(options.m_language) {
  case CPP :
    f.printf(_T("static const %s lexCharMap[%u] = {\n"), getTypeName(tableType), MAX_CHARS);
    break;
  case JAVA:
    f.printf(_T("private static final %s lexCharMap[] = {\n"), getTypeName(tableType));
    break;
  }
  const int oldMargin = f.getLeftMargin();
  f.setLeftMargin(oldMargin+2);
  for(UINT i = 0; i < MAX_CHARS; i++) {
    f.printf(_T("%4u"), map[i]);
    if(i < MAX_CHARS - 1) {
      f.printf(_T(","));
    }
    if((i % 16 == 15) || (i == MAX_CHARS - 1)) {
      f.printf(_T("\n"));
    }
  }
  f.setLeftMargin(oldMargin);
  f.printf(_T("};\n\n"));
}

void DFA::printCharMap(MarginFile &f) const {
  f.printf(_T("//"));
  for(UINT i = 0; i < MAX_CHARS; i++) {
    f.printf(((i % 16 == 15) || (i == MAX_CHARS-1)) ? _T("%s") : _T("%-4s "), binToAscii(i).cstr());
    if((i % 16 == 15) && (i < MAX_CHARS-1)) {
      f.printf(_T("\n//"));
    }
  }
  f.printf(_T("\n\n"));
}

void DFA::printStateMap(MarginFile &f, const int *map) const {
  const Options    &options    = Options::getInstance();
  const UINT        stateCount = getStateCount();
  const int         maxValue   = findMaxValue(map, stateCount);
  const IntegerType tableType  = findIntType(0,maxValue);

  f.setLeftMargin(0);
  switch(options.m_language) {
  case CPP  :
    f.printf(_T("static const %s lexStateMap[%u] = {\n"), getTypeName(tableType), stateCount);
    break;
  case JAVA :
    f.printf(_T("private static final %s lexStateMap[] = {\n"), getTypeName(tableType));
    break;
  }
  const UINT elementWidth = (UINT)format(_T("%d"), maxValue).length();
  const int  oldMargin    = f.setLeftMargin(2);
  for(UINT s = 0; s < stateCount; s++) {
    if(s % 20 == 0) {
      f.printf(_T("/* %3u */"), s);
    }
    f.printf(_T("%*d"), elementWidth, map[s]);
    if(s < stateCount-1) {
      f.printf(_T(","));
    }
    if(s % 20 == 19 || s == stateCount-1) {
      f.printf(_T("\n"));
    }
  }
  f.setLeftMargin(oldMargin);
  f.printf(_T("};\n\n"));
}

IntInterval DFA::minmaxElement(const BitSet &rowSave, const BitSet &columnSave) const {
  int minValue, maxValue;
  bool firstTime = true;
  for(auto rit = rowSave.getIterator(); rit.hasNext();) {
    const DFAtrans &transitions = m_states[(UINT)rit.next()].m_transition;
    for(auto cit = columnSave.getIterator(); cit.hasNext();) {
      const int e = transitions[(UINT)cit.next()];
      if(firstTime) {
        minValue  = maxValue = e;
        firstTime = false;
      } else if(e > maxValue) {
        maxValue = e;
      } else if(e < minValue) {
        minValue = e;
      }
    }
  }
  return IntInterval(minValue, maxValue);
}

void DFA::printTransitionMatrix(MarginFile &f, const BitSet &rowSave, const BitSet &columnSave) const {
  const Options    &options        = Options::getInstance();
  const IntInterval trInterval     = minmaxElement(rowSave, columnSave);
  const IntegerType tableType      = findIntType(trInterval.getMin(), trInterval.getMax());
  const UINT        minStrLen      = (UINT)format(_T("%d"), trInterval.getMin()).length(), maxStrLen = (UINT)format(_T("%d"), trInterval.getMax()).length();
  const UINT        elementWidth   = max(minStrLen, maxStrLen);

  const UINT        rowCount       = (UINT)rowSave.size(), colCount = (UINT)columnSave.size();
  const UINT        rowCountWidth  = (UINT)format(_T("%u"), rowCount - 1).length();
  const UINT        lineHeaderSize = (UINT)format(_T("/* %*u */ {"), rowCountWidth, 1).length();
  const UINT        lineLength     = colCount * (elementWidth + 1) + lineHeaderSize + 2; // line-header + leftMargin + 1 command-seprator for each element

  switch(options.m_language) {
  case CPP :
    f.printf(_T("static const %s lexNext[%u][%u] = {\n"), getTypeName(tableType), rowCount, colCount);
    break;
  case JAVA:
    f.printf(_T("private static final %s lexNext[][] = {\n"), getTypeName(tableType));
    break;
  }
  const int oldMargin = f.getLeftMargin();
  f.setLeftMargin(oldMargin+2);

  if(lineLength <= 156) {
    UINT rCount = 0;
    for(auto rit = rowSave.getIterator(); rit.hasNext(); rCount++) {
      const UINT      r           = (UINT)rit.next();
      const DFAtrans &transitions = m_states[r].m_transition;
      f.printf(_T("/* %*u */ {"), rowCountWidth, rCount);
      UINT cCount = 0;
      for(auto cit = columnSave.getIterator(); cit.hasNext(); cCount++) {
        const UINT c = (UINT)cit.next();
        if(cCount > 0) {
          f.printf(_T(","));
        }
        f.printf(_T("%*d"), elementWidth, transitions[c]);
      }
      f.printf(_T("}"));
      if(rCount < rowCount-1) {
        f.printf(_T(",\n"));
      } else {
        f.printf(_T("\n"));
      }
    }
  } else {
    UINT rCount = 0;
    TCHAR delim = ' ';
    for(auto rit = rowSave.getIterator(); rit.hasNext(); rCount++, delim = ',') {
      const UINT      r           = (UINT)rit.next();
      const DFAtrans &transitions = m_states[r].m_transition;
      f.printf(_T("/* %*u */ %c{"), rowCountWidth, rCount, delim);
      const UINT oldLeftMargin = f.setLeftMargin(f.getCurrentLineLength());
      UINT cCount = 1;
      for(auto cit = columnSave.getIterator(); cit.hasNext(); cCount++) {
        UINT c = (UINT)cit.next();
        f.printf(_T("%*d"), elementWidth, transitions[c]);
        if(cCount < colCount) {
          f.printf(_T(","));
        }
        if(cCount % 20 == 0) {
          f.printf(_T("\n"));
        }
      }
      f.printf(_T("}\n"));
      f.setLeftMargin(oldLeftMargin);
    }
  }
  f.setLeftMargin(oldMargin);
  f.printf(_T("};\n\n"));
}

void DFA::printAcceptTable(MarginFile &f) const {
  const Options &options    = Options::getInstance();
  const UINT     stateCount = getStateCount();
  f.setLeftMargin(0);
  switch(options.m_language) {
  case CPP:
    f.printf(_T("static const char lexAccept[%u] = {\n"), stateCount);
    break;
  case JAVA:
    f.printf(_T("private static final byte lexAccept[] = {\n"));
    break;
  }

  int oldMargin = f.getLeftMargin();
  f.setLeftMargin(oldMargin+2);
  TCHAR delim = ' ';
  for(UINT i = 0; i < stateCount; i++, delim = ',') {
    const DFAstate &state = m_states[i];
    if(i % 20 == 0) {
      f.printf(_T("/* %3u */"), i);
    }
    if(state.m_accept == nullptr) {
      f.printf(_T("%c0"), delim);
    } else {
      f.printf(_T("%c%d"), delim, state.m_accept->m_anchor ? state.m_accept->m_anchor : 4);
    }
    if((i % 20 == 19) || (i == stateCount - 1)) {
      f.printf(_T("\n"));
    }
  }
  f.setLeftMargin(oldMargin);
  f.printf(_T("};\n\n"));
}

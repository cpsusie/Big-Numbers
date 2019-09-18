#include "pch.h"
#include <BitSet.h>
#include "DFA.h"

// Compress and output transitionmatrix eliminating equal columns and rows
void DFA::getDFATables(DFATables &tables) const {
  tables.clear();

  if(m_states.isEmpty()) {
    return;
  }
  tables.allocate(m_states.size(), MAX_CHARS);
  BitSet  columnSave(MAX_CHARS);       // columns that will remain in table
  BitSet  rowSave(m_states.size());    // rows    that will remain in table

  reduce(tables, rowSave, columnSave); // Compress the tables

  getTransitionMatrix(tables, rowSave, columnSave);
  getAcceptTable(tables);
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
  const UINT charMapSize = tables.m_charMapSize;
  for(size_t i = 0; i < charMapSize; i++) {
    tables.m_charMap[i] = -1;
  }
  for(int r_ncols = 0;;r_ncols++) {
    UINT i;
    for(i = r_ncols; (i < charMapSize) && (tables.m_charMap[i] != -1); i++);
    if(i >= charMapSize) {
      break;
    }
    columnSave.add(i);
    tables.m_charMap[i] = r_ncols;
    for(UINT j = i + 1; j < charMapSize; j++) {
      if(tables.m_charMap[j] == -1 && columnsEqual(i, j)) {
        tables.m_charMap[j] = r_ncols;
      }
    }
  }
  UINT maxUsedCharMapIndex = 0;
  for(UINT i = 0; i < charMapSize; i++) {
    if(tables.m_charMap[i]) {
      maxUsedCharMapIndex = i;
    }
  }
  const UINT newCharMapSize = maxUsedCharMapIndex + 1;
  if(newCharMapSize < charMapSize) {
    tables.setCharMapSize(newCharMapSize);
  }
  const size_t stateCount = m_states.size();
  for(size_t i = 0; i < stateCount; i++) {
    tables.m_stateMap[i] = -1;
  }
  for(int r_nrows = 0;; r_nrows++ ) {
    size_t i;
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
  const size_t n = m_states.size();
  short *a = tables.m_acceptStates;
  for(size_t s = 0; s < m_states.size(); s++) {
    a[s] = m_states[s].m_acceptIndex;
  }
}

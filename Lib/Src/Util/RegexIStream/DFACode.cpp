#include "pch.h"
#include <BitSet.h>
#include "DFA.h"

class TmpDFATables {
public:
  UINT m_rowCount, m_columnCount;
  CompactIntArray m_charMap;          // size = m_charMapSize
  CompactIntArray m_stateMap;         // size = m_stateCount
  CompactIntArray m_transitionMatrix; // size = m_rowCount * m_columnCount
  CompactIntArray m_acceptStates;     // size = m_stateCount. if m_acceptState[i]>=0, then state i is an accepting state, and the returnvalue is the indexed element

  TmpDFATables() {
    m_rowCount = m_columnCount = 0;
  }
  void allocate(      UINT stateCount, UINT charMapSize);
  void allocateMatrix(UINT rowCount  , UINT columnCount);
  void reduceCharMapSize(UINT newCharMapSize);
  inline bool isEmpty() const {
    return m_stateMap.isEmpty();
  }
  inline int &transition(UINT r, UINT c) {
    return m_transitionMatrix[m_columnCount*r + c];
  }
  void saveToDFATable(DFATables &t) const;
};

#define ZERO ((int)0)

void TmpDFATables::allocate(UINT stateCount, UINT charMapSize) {
  m_charMap.insert(     0, (int)-1, charMapSize );
  m_stateMap.insert(    0, (int)-1, stateCount  );
  m_acceptStates.insert(0, ZERO, stateCount  );
}

void TmpDFATables::reduceCharMapSize(UINT newCharMapSize) {
  const int deleteCount = (int)m_charMap.size() - (int)newCharMapSize;
  if(newCharMapSize) {
    m_charMap.remove(newCharMapSize, deleteCount);
  }
}

void TmpDFATables::allocateMatrix(UINT rowCount, UINT columnCount) {
  m_rowCount    = rowCount;
  m_columnCount = columnCount;
  const size_t elemCount = rowCount * m_columnCount;
  m_transitionMatrix.insert(0, ZERO, elemCount);
}

void TmpDFATables::saveToDFATable(DFATables &t) const {
  t.m_stateCount        = (UINT)m_stateMap.size();
  t.m_rowCount          = m_rowCount;
  t.m_columnCount       = m_columnCount;
  t.m_charMapSize       = (UINT)m_charMap.size();
  t.m_charMap           = FixedIntArray::allocateFixedArray(m_charMap         );
  t.m_stateMap          = FixedIntArray::allocateFixedArray(m_stateMap        );
  t.m_transitionMatrix  = FixedIntArray::allocateFixedArray(m_transitionMatrix);
  t.m_acceptStates      = FixedIntArray::allocateFixedArray(m_acceptStates    );
}

// Compress and output transitionmatrix eliminating equal columns and rows
void DFA::getDFATables(DFATables &tables) const {
  tables.clear();

  if(m_states.isEmpty()) {
    return;
  }
  TmpDFATables ttabs;
  ttabs.allocate((UINT)m_states.size(), MAX_CHARS);
  BitSet  columnSave(MAX_CHARS);       // columns that will remain in table
  BitSet  rowSave(m_states.size());    // rows    that will remain in table

  reduce(             ttabs, rowSave, columnSave); // Compress the tables
  getTransitionMatrix(ttabs, rowSave, columnSave);
  getAcceptTable(     ttabs);
  ttabs.saveToDFATable(tables);
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

void DFA::reduce(TmpDFATables &ttabs, BitSet &rowSave, BitSet &columnSave) const {
  const UINT charMapSize = (UINT)ttabs.m_charMap.size();
  for(int r_ncols = 0;;r_ncols++) {
    UINT i;
    for(i = r_ncols; (i < charMapSize) && (ttabs.m_charMap[i] != -1); i++);
    if(i >= charMapSize) {
      break;
    }
    columnSave.add(i);
    ttabs.m_charMap[i] = r_ncols;
    for(UINT j = i + 1; j < charMapSize; j++) {
      if(ttabs.m_charMap[j] == -1 && columnsEqual(i, j)) {
        ttabs.m_charMap[j] = r_ncols;
      }
    }
  }
  UINT maxUsedCharMapIndex = 0;
  for(UINT i = 0; i < charMapSize; i++) {
    if(ttabs.m_charMap[i]) {
      maxUsedCharMapIndex = i;
    }
  }
  const UINT newCharMapSize = maxUsedCharMapIndex + 1;
  if(newCharMapSize < charMapSize) {
    ttabs.reduceCharMapSize(newCharMapSize);
  }
  const size_t stateCount = m_states.size();
  for(int r_nrows = 0;; r_nrows++ ) {
    size_t i;
    for(i = r_nrows; (i < stateCount) && (ttabs.m_stateMap[i] != -1); i++);
    if(i >= stateCount) {
      break;
    }
    rowSave.add(i);
    ttabs.m_stateMap[i] = r_nrows;
    for(size_t j = i+1; j < stateCount; j++) {
      if(ttabs.m_stateMap[j] == -1 && rowsEqual(i, j)) {
        ttabs.m_stateMap[j] = r_nrows;
      }
    }
  }
}

void DFA::getTransitionMatrix(TmpDFATables &ttabs, const BitSet &rowSave, const BitSet &columnSave) const {
  const UINT rowCount = (UINT)rowSave.size();
  const UINT colCount = (UINT)columnSave.size();

  ttabs.allocateMatrix(rowCount, colCount);
  CompactIntArray columnsIndex(colCount);
  for(ConstIterator<size_t> it = columnSave.getIterator(); it.hasNext();) {
    columnsIndex.add((int)it.next());
  }

  int r = 0;
  for(ConstIterator<size_t> it = rowSave.getIterator(); it.hasNext(); r++) {
    const int *DFAtransitions = m_states[it.next()].m_transition;
    int       *row            = &ttabs.transition(r, 0);
    for(UINT c = 0; c < colCount; c++) {
      *(row++) = DFAtransitions[columnsIndex[c]];
    }
  }
}

void DFA::getAcceptTable(TmpDFATables &ttabs) const {
  const size_t n = m_states.size();
  CompactIntArray &a = ttabs.m_acceptStates;
  for(size_t s = 0; s < m_states.size(); s++) {
    a[s] = m_states[s].m_acceptIndex;
  }
}

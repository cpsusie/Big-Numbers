#include "stdafx.h"
#include "NTindexNodeArray.h"

namespace TransSuccMatrixCompression {

NTindexNodeArray::NTindexNodeArray(const AbstractParserTables &tables) : m_tables(tables) {
  TransposeSuccessorMatrix tsm(tables);

  const UINT NTermCount = tables.getNTermCount();
  setCapacity(NTermCount);
  redirectDebugLog();
  for(UINT NTindex = 0; NTindex < NTermCount; NTindex++) {
    const StatePairArray &row = tsm[NTindex];
    add(row.isEmpty() ? nullptr : NTindexNode::allocateNTindexNode(NTindex, tables, tsm[NTindex]));
    debugLog(_T("%s"), (last() == nullptr) ? _T("null\n") : last()->toString().cstr());
  }
}

NTindexNodeArray::~NTindexNodeArray() {
  clear();
}

void NTindexNodeArray::clear() {
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    SAFEDELETE((*this)[i]);
  }
  __super::clear();
}

String NTindexNodeArray::toString() const {
  String result;
  for(auto it = getIterator(); it.hasNext();) {
    result += it.next()->toString();
    result += '\n';
  }
  return result;
}

TransposeSuccessorMatrix::TransposeSuccessorMatrix(const AbstractParserTables &tables) {
  const UINT termCount  = tables.getTermCount();
  const UINT NTermCount = tables.getNTermCount();
  const UINT stateCount = tables.getStateCount();

  setCapacity(NTermCount);
  for(UINT ntIndex = 0; ntIndex < NTermCount; ntIndex++) {
    add(StatePairArray());
  }
  for(UINT state = 0; state < stateCount; state++) {
    const UINT n = tables.getLegalNTermCount(state);
    if(n > 0) {
      CompactUIntArray NTermArray(n);
      NTermArray.insert(0, (UINT)0, n);
      tables.getLegalNTerms(state, NTermArray.begin());
      for(UINT nt : NTermArray) {
        (*this)[nt - termCount].add(StatePair(state, tables.getSuccessor(state, nt)));
      }
    }
  }
  for(auto it = getIterator(); it.hasNext();) {
    it.next().sortByNewState();
  }
}

}; // namespace TransSuccMatrixCompression

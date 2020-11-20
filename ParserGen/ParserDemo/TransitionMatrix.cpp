#include "StdAfx.h"
#include "TransitionMatrix.h"

#define MATRIX_ERROR SHORT_MAX

ActionMatrix::ActionMatrix(const AbstractParserTables &tables)
: m_termCount(  tables.getTermCount()    )
, m_symbolCount(tables.getSymbolCount()  )
, m_NTermCount( tables.getNTermCount()   )
, m_stateCount( tables.getStateCount()   )
{
  setDimension(m_stateCount, m_termCount);
  for(UINT state = 0; state < m_stateCount; state++) {
    for(UINT term = 0; term < m_termCount; term++) {
      const int action = tables.getAction(state, term);
      (*this)(state, term) = (action == AbstractParserTables::_ParserError) ? MATRIX_ERROR : action;
    }
  }
}

String ActionMatrix::toString() const {
  String              result;
  result = _T("     | ");
  for(UINT term = 0; term < m_termCount; term++) {
    result += format(_T("%4u "), term);
  }

  result += '\n';
  result += spaceString(result.length(), _T('_'));
  result += '\n';

  for(UINT state = 0; state < m_stateCount; state++) {
    String line;
    line = format(_T("%4u | "), state);
    for(UINT term = 0; term < m_termCount; term++) {
      const int action = (*this)(state, term);
      if(action == MATRIX_ERROR) {
        line += _T("   E ");
      } else {
        line += format(_T("%4d "), action);
      }
    }
    result += line;
    result += '\n';
  }
  return result;
}

SuccessorMatrix::SuccessorMatrix(const AbstractParserTables &tables)
: m_termCount(  tables.getTermCount()    )
, m_symbolCount(tables.getSymbolCount()  )
, m_NTermCount( tables.getNTermCount()   )
, m_stateCount( tables.getStateCount()   )
{
  setDimension(m_stateCount, m_NTermCount).setValue(0,0,m_stateCount,m_NTermCount, MATRIX_ERROR);
  for(UINT state = 0; state < m_stateCount; state++) {
    const UINT n = tables.getLegalNTermCount(state);
    if(n > 0) {
      CompactUIntArray row(n);
      row.insert(0, (UINT)0, n);
      tables.getLegalNTerms(state, row.begin());
      for(UINT nt : row) {
        (*this)(state, nt-m_termCount) = tables.getSuccessor(state, nt);
      }
    }
  }
}

String SuccessorMatrix::toString() const {
  String              result;
  result = _T("     | ");
  for(UINT nt = m_termCount; nt < m_symbolCount; nt++) {
    result += format(_T("%4u "), nt);
  }
  result += '\n';
  result += spaceString(result.length(), _T('_'));
  result += '\n';
  for(UINT state = 0; state < m_stateCount; state++) {
    String line;
    line = format(_T("%4u | "), state);
    for(UINT NTindex = 0; NTindex < m_NTermCount; NTindex++) {
      const USHORT v = (*this)(state, NTindex);
      if(v == MATRIX_ERROR) {
        line += _T("   E ");
      } else {
        line += format(_T("%4u "), v);
      }
    }
    result += line;
    result += '\n';
  }
  return result;
}

// ------------------------------------------ Transpose successor matrix -------------------------------------------------

String StateSuccArray::toString() const {
  String result;
  if(!isEmpty()) {
    BitSet fromStateSet(m_stateCount);
    UINT   flushCount   =  0;
    UINT   lastNewState = -1;
    TCHAR  delim        = ':';
    for(auto it = getIterator(); it.hasNext();) {
      const StateSucc &ss = it.next();
      if(ss.m_newState != lastNewState) {
        if(!fromStateSet.isEmpty()) {
          result += format(_T("%csucc=%4u on %s"), delim, lastNewState, fromStateSet.toRangeString().cstr());
          flushCount++;
          delim = ',';
          fromStateSet.clear();
        }
        lastNewState = ss.m_newState;
      }
      fromStateSet.add(ss.m_state);
    }
    if(!fromStateSet.isEmpty()) {
      if(flushCount == 0) {
        result += format(_T("%csucc=%4u from %4zu states"), delim, lastNewState, fromStateSet.size());
      } else {
        result += format(_T("%csucc=%4u on %s"), delim, lastNewState, fromStateSet.toRangeString().cstr());
      }
    }
  }
  return result;
}

TransposeSuccessorMatrix::TransposeSuccessorMatrix(const AbstractParserTables &tables)
: m_symbolCount(tables.getSymbolCount()  )
, m_termCount(  tables.getTermCount()    )
, m_NTermCount( tables.getNTermCount()   )
, m_stateCount( tables.getStateCount()   )
{
  setCapacity(m_NTermCount);
  for(UINT ntIndex = 0; ntIndex < m_NTermCount; ntIndex++) {
    add(StateSuccArray(m_stateCount));
  }
  for(UINT state = 0; state < m_stateCount; state++) {
    const UINT n = tables.getLegalNTermCount(state);
    if(n > 0) {
      CompactUIntArray ntArray(n);
      ntArray.insert(0, (UINT)0, n);
      tables.getLegalNTerms(state, ntArray.begin());
      for(UINT i = 0; i < n; i++) {
        const UINT nt       = ntArray[i];
        const UINT newState = tables.getSuccessor(state, nt);
        (*this)[nt - m_termCount].add(StateSucc(state, newState));
      }
    }
  }
  for(auto it = getIterator(); it.hasNext();) {
    it.next().sortByNewState();
  }
}

String TransposeSuccessorMatrix::toString() const {
  UINT NTindex = 0;
  String result;
  for(auto it = getIterator(); it.hasNext();) {
    result += format(_T("NTIndex:%4u:%s\n"), NTindex++, it.next().toString().cstr());
  }
  return result;
}

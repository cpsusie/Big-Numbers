#include "pch.h"
#include <ParserTransitionMatrix.h>

FullActionMatrix::FullActionMatrix(const AbstractParserTables &tables)
: ParserTransitionMatrix(tables, MatrixDimension(tables.getStateCount(), tables.getTermCount()))
{
  for(UINT state = 0; state < m_stateCount; state++) {
    for(UINT term = 0; term < m_termCount; term++) {
      const int action = tables.getAction(state, term);
      (*this)(state, term) = action;
    }
  }
}

String FullActionMatrix::toString() const {
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
      if(action == AbstractParserTables::_ParserError) {
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

FullSuccessorMatrix::FullSuccessorMatrix(const AbstractParserTables &tables)
: ParserTransitionMatrix(tables, MatrixDimension(tables.getStateCount(), tables.getNTermCount()))
{
  for(UINT state = 0; state < m_stateCount; state++) {
    for(UINT NTindex = 0; NTindex < m_ntermCount; NTindex++) {
      const UINT nterm    = m_termCount + NTindex;
      const UINT newState = tables.getSuccessor(state, nterm);
      (*this)(state, NTindex) = newState;
    }
  }
}

String FullSuccessorMatrix::toString() const {
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
    for(UINT NTindex = 0; NTindex < m_ntermCount; NTindex++) {
      const USHORT v = (*this)(state, NTindex);
      if(v == AbstractParserTables::_ParserError) {
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

String StatePairArray::toString(bool forceAllStates) const {
  String result;
  if(!isEmpty()) {
    BitSet fromStateSet(m_stateCount);
    UINT   flushCount   =  0;
    UINT   lastNewState = -1;
    TCHAR  delim        = ':';
    for(auto it = getIterator(); it.hasNext();) {
      const StatePair &ss = it.next();
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
      if(!forceAllStates && (flushCount == 0)) {
        result += format(_T("%csucc=%4u from %4zu states"), delim, lastNewState, fromStateSet.size());
      } else {
        result += format(_T("%csucc=%4u on %s"), delim, lastNewState, fromStateSet.toRangeString().cstr());
      }
    }
  }
  return result;
}

AbstractStatePairMatrix::AbstractStatePairMatrix(const AbstractParserTables &tables, UINT rowCount)
: m_nameContainer(tables                 )
, m_termCount(    tables.getTermCount()  )
, m_symbolCount(  tables.getSymbolCount())
, m_ntermCount(   tables.getNTermCount() )
, m_stateCount(   tables.getStateCount() )
{
  setCapacity(rowCount);
  for(UINT r = 0; r < rowCount; r++) {
    add(StatePairArray(m_stateCount));
  }
}

void AbstractStatePairMatrix::sortAllRows() {
  for(auto it = getIterator(); it.hasNext();) {
    it.next().sortByNewState();
  }
}

TransposedShiftMatrix::TransposedShiftMatrix(const AbstractParserTables &tables) : AbstractStatePairMatrix(tables, tables.getTermCount()) {
  const FullActionMatrix am(tables);
  for(UINT state = 0; state < m_stateCount; state++) {
    for(UINT term = 0; term < m_termCount; term++) {
      const int action = am(state, term);
      if((action > 0) && (action != AbstractParserTables::_ParserError)) { // shift action
        const UINT newState = action;
        (*this)[term].add(StatePair(state, newState));
      }
    }
  }
  sortAllRows();
}

String TransposedShiftMatrix::toString() const {
  const UINT maxNameLength = m_nameContainer.getMaxTermNameLength();
  UINT       term          = 0;
  String     result;
  for(auto it = getIterator(); it.hasNext(); term++) {
    result += format(_T("Term:%4u:%-*s:%s\n")
                    ,term
                    ,maxNameLength, m_nameContainer.getSymbolName(term).cstr()
                    ,it.next().toString(true).cstr()
                    );
  }
  return result;
}

TransposedSuccessorMatrix::TransposedSuccessorMatrix(const AbstractParserTables &tables): AbstractStatePairMatrix(tables,tables.getNTermCount()) {
  const FullSuccessorMatrix sm(tables);
  for(UINT state = 0; state < m_stateCount; state++) {
    for(UINT NTindex = 0; NTindex < m_ntermCount; NTindex++) {
      const int newState = sm(state, NTindex);
      if(newState != AbstractParserTables::_ParserError) {
        (*this)[NTindex].add(StatePair(state, newState));
      }
    }
  }
  sortAllRows();
}

String TransposedSuccessorMatrix::toString() const {
  const UINT maxNameLength = m_nameContainer.getMaxNTermNameLength();
  UINT       ntIndex       = 0;
  String     result;
  for(auto it = getIterator(); it.hasNext(); ntIndex++) {
    result += format(_T("NTIndex:%4u %-*s:%s\n")
                    ,ntIndex
                    ,maxNameLength, m_nameContainer.getSymbolName(ntIndex+m_termCount).cstr()
                    ,it.next().toString(false).cstr()
                    );
  }
  return result;
}

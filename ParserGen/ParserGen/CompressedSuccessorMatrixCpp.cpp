#include "stdafx.h"
#include <CompactHashMap.h>
#include "GrammarCode.h"
#include "CompressEncoding.h"
#include "CompressedSuccessorMatrixCpp.h"

namespace SuccessorMatrixCompression {

CompressedSuccessorMatrix::CompressedSuccessorMatrix(const GrammarTables &tables)
  : MacroMap(          tables                  )
  , m_tables(          tables                  )
  , m_stateCount(      tables.getStateCount()  )
  , m_NTindexType(     tables.getNTindexType() )
  , m_stateType(       tables.getStateType()   )
  , m_definedStateSet( tables.getStateCount()  )
{
  generateCompressedForm();
}

void CompressedSuccessorMatrix::generateCompressedForm() {
  m_currentNTindexListSize = 0;
  m_currentStateListSize   = 0;

  const SuccessorMatrix &sm = m_tables.getSuccessorMatrix();

  for(UINT state = 0; state < getStateCount(); state++) {
    const SuccessorStateArray &succList  = sm[state];
    const UINT                 succCount = (UINT)succList.size();
    if(succCount == 0) {
      continue;
    }
    Macro macro = doSuccList(state, succList);
    addMacro(macro.setIndex(state).setName(format(_T("_su%04u"), state)));
  }
}

Macro CompressedSuccessorMatrix::doSuccList(UINT state, const SuccessorStateArray &succList) {
  m_definedStateSet.add(state);
  const UINT succCount = (UINT)succList.size();
  if(succCount == 1) {
    return doOneSuccessorState(state, succList[0]);
  } else {
    return doNTindexListState(state, succList);
  }
}

Macro CompressedSuccessorMatrix::doNTindexListState(UINT state, const SuccessorStateArray &succList) {
  const NTindexSet    ntIndexSet = succList.getNTindexSet(getTerminalCount(), getSymbolCount());
  IndexMapValue      *imvp       = m_NTindexMap.get(ntIndexSet);
  UINT                ntListIndex, ntListCount;

  if(imvp != nullptr) {
    ntListIndex = imvp->m_arrayIndex;
    ntListCount = imvp->m_commentIndex;
    imvp->addState(state);
  } else {
    ntListIndex = m_currentNTindexListSize;
    ntListCount = m_NTindexMap.getCount();
    m_NTindexMap.put(ntIndexSet, IndexMapValue(getStateCount(), state, ntListIndex));
    m_currentNTindexListSize += (UINT)ntIndexSet.size() + 1;
  }

  const StateArray sa   = succList.getStateArray();
  imvp                  = m_stateListMap.get(sa);
  UINT                 stateListIndex, stateListCount;
  if(imvp != nullptr) {
    stateListIndex = imvp->m_arrayIndex;
    stateListCount = imvp->m_commentIndex;
    imvp->addState(state);
  } else {
    stateListIndex = m_currentStateListSize;
    stateListCount = m_stateListMap.getCount();
    m_stateListMap.put(sa, IndexMapValue(getStateCount(), state, stateListIndex));
    m_currentStateListSize += (UINT)sa.size();
  }
  const String macroValue = encodeMacroValue(ParserTables::CompCodeTermList, stateListIndex, ntListIndex);
  const String comment    = format(_T("NTindexList %3u, stateList %3u"), ntListCount, stateListCount);
  return Macro(getStateCount(), state, macroValue, comment);
}

Macro CompressedSuccessorMatrix::doOneSuccessorState(UINT state, const SuccessorState &ss) {
  const UINT    NT         = ss.m_nt;
  const UINT    newState   = ss.m_newState;
  const UINT    NTindex    = NT - getTerminalCount();
  const String  macroValue = encodeMacroValue(ParserTables::CompCodeOneItem, newState, NTindex);
  const String  comment    = format(_T("Goto %u on %s"), newState, getSymbolName(NT));
  return Macro(getStateCount(), state, macroValue, comment);
}

ByteCount CompressedSuccessorMatrix::print(MarginFile &output) const {
  ByteCount byteCount;
  byteCount += printMacroesAndSuccessorCode(output);
  byteCount += printNTindexAndStateList(    output);
  return byteCount;
}

ByteCount CompressedSuccessorMatrix::printMacroesAndSuccessorCode(MarginFile &output) const {
  const UINT   macroCount = getMacroCount();
  Array<Macro> macroes(getMacroArray());
  if(macroCount > 0) {
    macroes.sort(macroCmpByName);
    for(auto it = macroes.getIterator(); it.hasNext();) {
      it.next().print(output);
    }
    output.printf(_T("\n"));
  }
  output.printf(_T("#define nil (unsigned int)-1\n"));
  outputBeginArrayDefinition(output, _T("successorCode"), TYPE_UINT, getStateCount());

  TCHAR delim     = ' ';

  for(UINT state = 0; state < getStateCount(); state++, delim=',') {
    if(!m_definedStateSet.contains(state)) {
      output.printf(_T("%c%-7s"), delim, _T("nil"));
    } else {
      output.printf(_T("%c_su%04d"), delim, state);
    }
    if((state % 10 == 9) && (state != getStateCount()-1)) {
      output.printf(_T("\n"));
    }
  }
  return outputEndArrayDefinition(output, TYPE_UINT, getStateCount(), true);
}

ByteCount CompressedSuccessorMatrix::printNTindexAndStateList(MarginFile &output) const {
  ByteCount byteCount;
  if(m_currentNTindexListSize == 0) {
    output.printf(_T("#define NTindexListTable nullptr\n"));
    output.printf(_T("#define stateListTable   nullptr\n\n"));
    return byteCount;
  }
  { const NTindexSetIndexArray            ntSetArray        = m_NTindexMap.getEntryArray();
    UINT                                  tableSize         = 0;
    TCHAR                                 delim             = ' ';
    outputBeginArrayDefinition(output, _T("NTindexListTable"), m_NTindexType , ntSetArray.getElementCount(true));
    for(auto it = ntSetArray.getIterator();      it.hasNext();) {
      const IndexArrayEntry<NTindexSet>  &e                 = it.next();
      String                              comment           = format(_T("%3u %s"), e.m_commentIndex, e.getComment().cstr());
      const UINT                          n                 = (UINT)e.m_key.size();
      UINT                                counter           = 0;
      output.setLeftMargin(2);
      output.printf(_T("%c%3u"), delim, n); delim = ',';
      output.setLeftMargin(6);
      for(auto it1 = e.m_key.getIterator(); it1.hasNext(); counter++) {
        output.printf(_T(",%4zu"), it1.next());
        if((counter % 20 == 19) && (counter != n - 1)) {
          newLine(output, comment, 108);
        }
      }
      newLine(output, comment, 108);
      tableSize += n + 1;
    }
    byteCount += outputEndArrayDefinition(output, m_NTindexType , tableSize);
  }
  { const StateArrayIndexArray            stateListArray    = m_stateListMap.getEntryArray();
    UINT                                  tableSize         = 0;
    TCHAR                                 delim             = ' ';
    outputBeginArrayDefinition(output, _T("stateListTable"  ), m_stateType , stateListArray.getElementCount(false));
    for(auto it = stateListArray.getIterator();  it.hasNext();) {
      const IndexArrayEntry<StateArray>  &e                 = it.next();
      String                              comment           = format(_T("%3u %s"), e.m_commentIndex, e.getComment().cstr());
      const UINT                          n                 = (UINT)e.m_key.size();
      UINT                                counter           = 0;
      for(auto it1 = e.m_key.getIterator(); it1.hasNext(); counter++, delim = ',') {
        output.printf(_T("%c%4u"), delim, it1.next());
        if((counter % 20 == 19) && (counter != n - 1)) {
          newLine(output, comment, 108);
        }
      }
      newLine(output, comment, 108);
      tableSize += n;
    }
    byteCount += outputEndArrayDefinition(output, m_stateType , tableSize);
  }
  return byteCount;
}

}; // namespace SuccessorMatrixCompression

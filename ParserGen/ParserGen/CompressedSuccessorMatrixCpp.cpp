#include "stdafx.h"
#include "GrammarCode.h"
#include "CompressEncoding.h"
#include "CompressedSuccessorMatrixCpp.h"

namespace SuccessorMatrixCompression {

CompressedSuccessorMatrix::CompressedSuccessorMatrix(const Grammar &grammar)
  : MacroMap(          grammar                             )
  , m_grammar(         grammar                             )
  , m_usedByParam(     grammar.getBitSetParam(STATE_BITSET))
  , m_definedStateSet( grammar.getStateCount()             )
{
  generateCompressedForm();
}

void CompressedSuccessorMatrix::generateCompressedForm() {
  m_NTindexArraySize  = 0;
  m_newStateArraySize = 0;

  SuccessorMatrix sm;
  m_grammar.getResult().getSuccessorMatrix(sm);

  for(UINT state = 0; state < getStateCount(); state++) {
    const SuccessorStateArray &succArray = sm[state];
    const UINT                 succCount = succArray.getLegalNTermCount();
    if(succCount == 0) {
      continue;
    }
    Macro macro = doSuccessorArray(state, succArray);
    addMacro(macro.setIndex(state).setName(format(_T("_su%04u"), state)));
  }
}

Macro CompressedSuccessorMatrix::doSuccessorArray(UINT state, const SuccessorStateArray &succArray) {
  m_definedStateSet.add(state);
  const UINT succCount = succArray.getLegalNTermCount();
  if(succCount == 1) {
    return doOneSuccessorState(state, succArray.first());
  } else {
    return doNTindexArrayState(state, succArray);
  }
}

Macro CompressedSuccessorMatrix::doNTindexArrayState(UINT state, const SuccessorStateArray &succArray) {
  const NTindexSet    ntIndexSet = succArray.getNTindexSet(getTermCount(), getSymbolCount());
  IndexMapValue      *imvp       = m_NTindexArrayMap.get(ntIndexSet);
  UINT                ntArrayIndex, ntArrayCount;

  if(imvp != nullptr) {
    ntArrayIndex = imvp->m_arrayIndex;
    ntArrayCount = imvp->m_commentIndex;
    imvp->addUsedByValue(state);
  } else {
    ntArrayIndex = m_NTindexArraySize;
    ntArrayCount = m_NTindexArrayMap.getCount();
    m_NTindexArrayMap.put(ntIndexSet, IndexMapValue(m_usedByParam, state, ntArrayIndex));
    m_NTindexArraySize += (UINT)ntIndexSet.size() + 1;
  }

  const StateArray sa   = succArray.getStateArray();
  imvp                  = m_newStateArrayMap.get(sa);
  UINT                 newStateArrayIndex, newStateArrayCount;
  if(imvp != nullptr) {
    newStateArrayIndex = imvp->m_arrayIndex;
    newStateArrayCount = imvp->m_commentIndex;
    imvp->addUsedByValue(state);
  } else {
    newStateArrayIndex = m_newStateArraySize;
    newStateArrayCount = m_newStateArrayMap.getCount();
    m_newStateArrayMap.put(sa, IndexMapValue(m_usedByParam, state, newStateArrayIndex));
    m_newStateArraySize += (UINT)sa.size();
  }
  const String macroValue = encodeMacroValue(AbstractParserTables::CompCodeBinSearch, newStateArrayIndex, ntArrayIndex);
  const String comment    = format(_T("NTindexArray %3u, newStateArray %3u"), ntArrayCount, newStateArrayCount);
  return Macro(m_usedByParam, state, macroValue, comment);
}

Macro CompressedSuccessorMatrix::doOneSuccessorState(UINT state, const SuccessorState &ss) {
  const UINT    nterm      = ss.m_nterm;
  const UINT    newState   = ss.m_newState;
  const UINT    NTindex    = nterm - getTermCount();
  const String  macroValue = encodeMacroValue(AbstractParserTables::CompCodeImmediate, newState, NTindex);
  const String  comment    = format(_T("Goto %u on %s"), newState, getSymbolName(nterm).cstr());
  return Macro(m_usedByParam, state, macroValue, comment);
}

ByteCount CompressedSuccessorMatrix::print(MarginFile &output) const {
  ByteCount byteCount;
  byteCount += printMacroesAndSuccessorCodeArray(output);
  byteCount += printNTindexAndNewStateArray(     output);
  return byteCount;
}

ByteCount CompressedSuccessorMatrix::printMacroesAndSuccessorCodeArray(MarginFile &output) const {
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
  outputBeginArrayDefinition(output, _T("successorCodeArray"), TYPE_UINT, getStateCount());

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

ByteCount CompressedSuccessorMatrix::printNTindexAndNewStateArray(MarginFile &output) const {
  ByteCount byteCount;
  if(m_NTindexArraySize == 0) {
    output.printf(_T("#define NTindexArrayTable  nullptr\n"));
    output.printf(_T("#define newStateArrayTable nullptr\n\n"));
    return byteCount;
  }

  const AllTemplateTypes types(m_grammar);

  { const NTindexSetIndexArray            ntSetArray        = m_NTindexArrayMap.getEntryArray();
    UINT                                  tableSize         = 0;
    TCHAR                                 delim             = ' ';
    outputBeginArrayDefinition(output, _T("NTindexArrayTable"), types.getNTindexType(), ntSetArray.getElementCount(true));
    for(auto it = ntSetArray.getIterator();      it.hasNext();) {
      const IndexArrayEntry<NTindexSet>  &e                 = it.next();
      String                              comment           = format(_T("%3u %s"), e.m_commentIndex, e.getUsedByComment().cstr());
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
    byteCount += outputEndArrayDefinition(output, types.getNTindexType(), tableSize);
  }
  { const StateArrayIndexArray            stateArrayTable   = m_newStateArrayMap.getEntryArray();
    UINT                                  tableSize         = 0;
    TCHAR                                 delim             = ' ';
    outputBeginArrayDefinition(output, _T("newStateArrayTable"), types.getStateType(), stateArrayTable.getElementCount(false));
    for(auto it = stateArrayTable.getIterator();  it.hasNext();) {
      const IndexArrayEntry<StateArray>  &e                 = it.next();
      String                              comment           = format(_T("%3u %s"), e.m_commentIndex, e.getUsedByComment().cstr());
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
    byteCount += outputEndArrayDefinition(output, types.getStateType(), tableSize);
  }
  return byteCount;
}

}; // namespace SuccessorMatrixCompression

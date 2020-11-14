#include "stdafx.h"
#include <CompactHashMap.h>
#include "GrammarCode.h"
#include "CompressedActionMatrixCpp.h"

String generateStateSetComment(const BitSet &set) {
  const size_t n = set.size();
  return format(_T("Used by %s %s")
               ,(n == 1) ? _T("state ") : _T("states")
               ,set.getIterator().rangesToString(SizeTStringifier()).cstr());
}

String IndexMapValue::getComment() const {
  return generateStateSetComment(m_stateSet);
}

namespace ActionMatrix {

String Macro::getComment() const {
  if((getStateSetSize() == 1) && m_stateSet.contains(m_index)) {
    return m_comment;
  }
  return format(_T("%s %s")
               ,m_comment.cstr()
               ,generateStateSetComment(getStateSet()).cstr()
               );
}

CompressedActionMatrix::CompressedActionMatrix(const GrammarTables &tables)
  : m_tables(           tables                                )
  , m_stateCount(       tables.getStateCount()                )
  , m_terminalCount(    tables.getTerminalCount()             )
  , m_laSetSizeInBytes((tables.getTerminalCount() - 1) / 8 + 1)
  , m_terminalType(     tables.getTerminalType()              )
  , m_actionType(       tables.getActionType()                )
  , m_stateInfoArray(   tables                                )
{
  generateCompressedForm();
}

void CompressedActionMatrix::generateCompressedForm() {
  m_currentTermListSize   = 0;
  m_currentActionListSize = 0;
  m_currentLASetArraySize = 0;
  m_currentSplitNodeCount = 0;

  for(UINT state = 0; state < getStateCount(); state++) {
    const StateActionInfo *stateInfo = m_stateInfoArray[state];
    Macro                  macro     = doStateActionInfo(*stateInfo);
    addMacro(macro.setIndex(state).setName(format(_T("_ac%04u"), state)));
  }
}

Macro CompressedActionMatrix::doStateActionInfo(const StateActionInfo &stateInfo) {
  switch(stateInfo.getCompressionMethod()) {
  case ParserTables::CompCodeTermList : return doTermListState( stateInfo);
  case ParserTables::CompCodeSplitNode: return doSplitNodeState(stateInfo);
  case ParserTables::CompCodeOneItem  : return doOneItemState(  stateInfo);
  case ParserTables::CompCodeTermSet  : return doTermSetState(  stateInfo);
  default                         :
    throwException(_T("%s:Unknown compressionMethod for state %u"), __TFUNCTION__, stateInfo.getState());
    break;
  }
  return Macro(getStateCount(), 0, EMPTYSTRING, EMPTYSTRING); // should not come here
}

const Macro *CompressedActionMatrix::findMacroByValue(const String &macroValue) const {
  const UINT *indexp = m_macroMap.get(macroValue);
  return indexp ? &m_macroArray[*indexp] : nullptr;
}

void CompressedActionMatrix::addMacro(const Macro &m) {
  assert(m.getIndex() >= 0);
  const UINT index = (UINT)m_macroArray.size();
  m_macroArray.add(m);
  m_macroMap.put(m.getValue(), index);
}

Macro CompressedActionMatrix::doTermListState(const StateActionInfo &stateInfo) {
  const UINT              state      = stateInfo.getState();
  const ActionArray      &termList   = stateInfo.getTermList();
  const SymbolSet         laSet      = termList.getLookaheadSet(getTerminalCount());
  IndexMapValue          *imvp       = m_termListMap.get(laSet);
  UINT                    termListIndex, laCount;

  if(imvp != nullptr) {
    termListIndex = imvp->m_arrayIndex;
    laCount       = imvp->m_commentIndex;
    imvp->addState(state);
  } else {
    termListIndex = m_currentTermListSize;
    laCount       = m_termListMap.getCount();
    IndexMapValue nv(getStateCount(), state, termListIndex);
    m_termListMap.put(laSet, nv);
    m_currentTermListSize += (UINT)laSet.size() + 1;
  }

  const RawActionArray    actionList = termList.getRawActionArray();
  imvp = m_raaMap.get(actionList);
  UINT                    actionListIndex, raCount;
  if(imvp != nullptr) {
    actionListIndex = imvp->m_arrayIndex;
    raCount         = imvp->m_commentIndex;
    imvp->addState(state);
  } else {
    actionListIndex = m_currentActionListSize;
    raCount         = m_raaMap.getCount();
    IndexMapValue nv(getStateCount(), state, actionListIndex);
    m_raaMap.put(actionList, nv);
    m_currentActionListSize += (UINT)actionList.size();
  }
  const String            macroValue = encodeMacroValue(ParserTables::CompCodeTermList, actionListIndex, termListIndex);
  const String            comment    = format(_T("termList %4u, actionList %4u"), laCount, raCount);
  return Macro(getStateCount(), state, macroValue, comment);
}

Macro CompressedActionMatrix::doSplitNodeState(const StateActionInfo &stateInfo) {
  const UINT              state      = stateInfo.getState();
  const StateActionInfo  &left       = stateInfo.getChild(0);
  const StateActionInfo  &right      = stateInfo.getChild(1);

  UINT                    indexL, indexR;
  Macro                   macroL     = doStateActionInfo(left);
  const Macro            *mpL        = findMacroByValue(macroL.getValue());
  if(mpL != nullptr) {
    mpL->addState(state);
    indexL = mpL->getIndex();
  } else {
    const String          name       = format(_T("_sn%04u"), m_currentSplitNodeCount);
    indexL = getStateCount() + m_currentSplitNodeCount++;
    addMacro(macroL.setIndex(indexL).setName(name));
    mpL = &macroL;
  }

  Macro                   macroR     = doStateActionInfo(right);
  const Macro            *mpR        = findMacroByValue(macroR.getValue());
  if(mpR != nullptr) {
    mpR->addState(state);
    indexR = mpR->getIndex();
  } else {
    const String          name       = format(_T("_sn%04u"), m_currentSplitNodeCount);
    indexR = getStateCount() + m_currentSplitNodeCount++;
    addMacro(macroR.setIndex(indexR).setName(name));
    mpR = &macroR;
  }

  const String            macroValue = encodeMacroValue(ParserTables::CompCodeSplitNode, indexL, indexR);
  const String            comment    = format(_T("Split(%s,%s)"), mpL->getName().cstr(), mpR->getName().cstr());
  return Macro(getStateCount(), state, macroValue, comment);
}

Macro CompressedActionMatrix::doOneItemState(const StateActionInfo &stateInfo) {
  const UINT              state      = stateInfo.getState();
  const ParserAction      pa         = stateInfo.getOneItemAction();
  const int               action     = pa.m_action;                    // positive or negative
  const UINT              token      = pa.m_token;
  const String            macroValue = encodeMacroValue(ParserTables::CompCodeOneItem, action, token);
  const String            comment    = (action <= 0)
                                     ? format(_T("Reduce by %3u on %s"), -action, getSymbolName(pa.m_token))
                                     : format(_T("Shift  to %3u on %s"),  action, getSymbolName(pa.m_token));
  return Macro(getStateCount(), state, macroValue, comment);
}

Macro CompressedActionMatrix::doTermSetState(const StateActionInfo &stateInfo) {
  const UINT              state      = stateInfo.getState();
  const TermSetReduction &tsr        = stateInfo.getTermSetReduction();

  const SymbolSet        &laSet      = tsr.getTermSet();
  IndexMapValue          *vp         = m_laSetMap.get(laSet);
  UINT                    byteIndex, termSetCount;
  if(vp != nullptr) {
    byteIndex    = vp->m_arrayIndex;
    termSetCount = vp->m_commentIndex;
    vp->addState(state);
  } else {
    byteIndex    = m_currentLASetArraySize;
    termSetCount = m_laSetMap.getCount();
    IndexMapValue nv(getStateCount(), state, byteIndex);
    m_laSetMap.put(laSet, nv);
    m_currentLASetArraySize += m_laSetSizeInBytes;
  }
  const int               prod       = tsr.getProduction();
  const int               action     = -prod;
  const String            macroValue = encodeMacroValue(ParserTables::CompCodeTermSet, action, byteIndex);
  const String            comment    = format(_T("Reduce by %3u on tokens in termSet[%u]"), prod, termSetCount);
  return Macro(getStateCount(), state, macroValue, comment);
}

// ------------------------------------ Print ------------------------------------------------

ByteCount CompressedActionMatrix::print(MarginFile &output) const {
  ByteCount byteCount;
  byteCount += printMacroesAndActionCode(output);
  byteCount += printTermAndActionList(   output);
  byteCount += printTermSetTable(        output);
  return byteCount;
}

ByteCount CompressedActionMatrix::printMacroesAndActionCode(MarginFile &output) const {
  const UINT   macroCount = (UINT)m_macroArray.size();
  Array<Macro> macroes(m_macroArray);
  if(macroCount > 0) {
    macroes.sort(macroCmpByName);
    for(auto it = macroes.getIterator(); it.hasNext();) {
      const Macro &m = it.next();
      output.printf(_T("#define %s %-10s /* %-*s*/\n")
                   ,m.getName().cstr(), m.getValue().cstr()
                   ,commentWidth, m.getComment().cstr()
                   );
    }
    output.printf(_T("\n"));
  }

  outputBeginArrayDefinition(output, _T("actionCode"), TYPE_UINT, macroCount);
  macroes.sort(macroCmpByIndex);
  TCHAR delim = ' ';
  UINT  count = 0;
  for(auto it = macroes.getIterator(); it.hasNext(); count++, delim = ',') {
    output.printf(_T("%c%s"), delim, it.next().getName().cstr());
    if((count % 10 == 9) && (count != macroCount - 1)) {
      output.printf(_T("\n"));
    }
  }
  return outputEndArrayDefinition(output, TYPE_UINT, macroCount, true);
}

ByteCount CompressedActionMatrix::printTermAndActionList(MarginFile &output) const {
  ByteCount byteCount;
  if(m_currentTermListSize == 0) {
    output.printf(_T("#define termListTable   nullptr\n"  ));
    output.printf(_T("#define actionListTable nullptr\n\n"));
    return byteCount;
  }

  { const SymbolSetIndexArray termListArray = m_termListMap.getEntryArray();
    UINT                      tableSize     = 0;
    TCHAR                     delim         = ' ';

    outputBeginArrayDefinition(output, _T("termListTable"), m_terminalType, termListArray.getElementCount(true));
    for(auto it = termListArray.getIterator(); it.hasNext();) {
      const IndexArrayEntry<SymbolSet> &e       = it.next();
      String                            comment = format(_T(" %3u %s"), e.m_commentIndex, e.getComment().cstr());
      const UINT                        n       = (UINT)e.m_key.size();
      UINT                              counter = 0;
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
    byteCount += outputEndArrayDefinition(output, m_terminalType, tableSize);
  }

  { const RawActionArrayIndexArray raaArray   = m_raaMap.getEntryArray();
    UINT                           tableSize  = 0;
    TCHAR                          delim      = ' ';

    outputBeginArrayDefinition(output, _T("actionListTable"), m_actionType, raaArray.getElementCount(false));
    for(auto it = raaArray.getIterator(); it.hasNext();) {
      const IndexArrayEntry<RawActionArray> &e       = it.next();
      String                                 comment = format(_T("%3u %s"), e.m_commentIndex, e.getComment().cstr());
      const UINT                             n       = (UINT)e.m_key.size();
      UINT                                   counter = 0;
      for(auto it1 = e.m_key.getIterator(); it1.hasNext(); counter++, delim = ',') {
        output.printf(_T("%c%4d"), delim, it1.next());
        if((counter % 20 == 19) && (counter != n - 1)) {
          newLine(output, comment, 108);
        }
      }
      newLine(output, comment, 108);
      tableSize += n;
    }
    byteCount += outputEndArrayDefinition(output, m_actionType, tableSize);
  }
  return byteCount;
}

ByteCount CompressedActionMatrix::printTermSetTable(MarginFile &output) const {
  ByteCount byteCount;
  if(m_laSetMap.size() == 0) {
    output.printf(_T("#define termSetTable nullptr\n\n"));
  } else {
    const SymbolSetIndexArray laSetArray = m_laSetMap.getEntryArray();
    outputBeginArrayDefinition(output, _T("termSetTable"), TYPE_UCHAR, m_currentLASetArraySize);
    TCHAR delim = ' ';
    for(auto it = laSetArray.getIterator(); it.hasNext();) {
      const IndexArrayEntry<SymbolSet> &e  = it.next();
      const ByteArray                   ba = symbolSetToByteArray(e.m_key);
      const UINT                        n  = (UINT)ba.size();
      String                            comment = format(_T("%3u %3u tokens %s"), e.m_commentIndex, (UINT)e.m_key.size(), e.getComment().cstr());
      for(const BYTE *cp = ba.getData(), *last = cp + n; cp < last; delim = ',') {
        output.printf(_T("%c0x%02x"), delim, *(cp++));
      }
      newLine(output, comment);
    }
    byteCount = outputEndArrayDefinition(output, TYPE_UCHAR, m_currentLASetArraySize);
  }
  return byteCount;
}

}; // namespace ActionMatrix

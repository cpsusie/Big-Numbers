#include "stdafx.h"
#include <CompactHashMap.h>
#include "GrammarCode.h"
#include "CompressedActionMatrixCpp.h"

namespace ActionMatrixCompression {

CompressedActionMatrix::CompressedActionMatrix(const GrammarTables &tables)
  : MacroMap(             tables                                )
  , m_tables(             tables                                )
  , m_stateCount(         tables.getStateCount()                )
  , m_termSetSizeInBytes((tables.getTerminalCount() - 1) / 8 + 1)
  , m_terminalType(       tables.getTerminalType()              )
  , m_actionType(         tables.getActionType()                )
  , m_stateInfoArray(     tables                                )
{
  generateCompressedForm();
}

void CompressedActionMatrix::generateCompressedForm() {
  m_currentTermListSize     = 0;
  m_currentActionListSize   = 0;
  m_currentTermSetArraySize = 0;
  m_currentSplitNodeCount   = 0;

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

Macro CompressedActionMatrix::doTermListState(const StateActionInfo &stateInfo) {
  const UINT              state      = stateInfo.getState();
  const ParserActionArray &termList  = stateInfo.getTermList();
  const TermSet           termSet    = termList.getLegalTermSet(getTerminalCount());
  IndexMapValue          *imvp       = m_termListMap.get(termSet);
  UINT                    termListIndex, termListCount;

  if(imvp != nullptr) {
    termListIndex = imvp->m_arrayIndex;
    termListCount = imvp->m_commentIndex;
    imvp->addState(state);
  } else {
    termListIndex = m_currentTermListSize;
    termListCount = m_termListMap.getCount();
    m_termListMap.put(termSet, IndexMapValue(getStateCount(), state, termListIndex));
    m_currentTermListSize += (UINT)termSet.size() + 1;
  }

  const ActionArray actionList = termList.getActionArray();
  imvp = m_actionListMap.get(actionList);
  UINT              actionListIndex, actionListCount;
  if(imvp != nullptr) {
    actionListIndex = imvp->m_arrayIndex;
    actionListCount = imvp->m_commentIndex;
    imvp->addState(state);
  } else {
    actionListIndex = m_currentActionListSize;
    actionListCount = m_actionListMap.getCount();
    m_actionListMap.put(actionList, IndexMapValue(getStateCount(), state, actionListIndex));
    m_currentActionListSize += (UINT)actionList.size();
  }
  const String            macroValue = encodeMacroValue(ParserTables::CompCodeTermList, actionListIndex, termListIndex);
  const String            comment    = format(_T("termList %4u, actionList %4u"), termListCount, actionListCount);
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
  const UINT              term       = pa.m_term;
  const String            macroValue = encodeMacroValue(ParserTables::CompCodeOneItem, action, term);
  const String            comment    = (action <= 0)
                                     ? format(_T("Reduce by %3u on %s"), -action, getSymbolName(pa.m_term))
                                     : format(_T("Shift  to %3u on %s"),  action, getSymbolName(pa.m_term));
  return Macro(getStateCount(), state, macroValue, comment);
}

Macro CompressedActionMatrix::doTermSetState(const StateActionInfo &stateInfo) {
  const UINT              state      = stateInfo.getState();
  const TermSetReduction &tsr        = stateInfo.getTermSetReduction();

  const TermSet          &termSet    = tsr.getTermSet();
  IndexMapValue          *vp         = m_termSetMap.get(termSet);
  UINT                    byteIndex, termSetCount;
  if(vp != nullptr) {
    byteIndex    = vp->m_arrayIndex;
    termSetCount = vp->m_commentIndex;
    vp->addState(state);
  } else {
    byteIndex    = m_currentTermSetArraySize;
    termSetCount = m_termSetMap.getCount();
    m_termSetMap.put(termSet, IndexMapValue(getStateCount(), state, byteIndex));
    m_currentTermSetArraySize += m_termSetSizeInBytes;
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
  const UINT   macroCount = getMacroCount();
  Array<Macro> macroes(getMacroArray());
  if(macroCount > 0) {
    macroes.sort(macroCmpByName);
    for(auto it = macroes.getIterator(); it.hasNext();) {
      it.next().print(output);
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

  { const TermSetIndexArray termListArray = m_termListMap.getEntryArray();
    UINT                    tableSize     = 0;
    TCHAR                   delim         = ' ';

    outputBeginArrayDefinition(output, _T("termListTable"), m_terminalType, termListArray.getElementCount(true));
    for(auto it = termListArray.getIterator(); it.hasNext();) {
      const IndexArrayEntry<TermSet> &e       = it.next();
      String                          comment = format(_T(" %3u %s"), e.m_commentIndex, e.getComment().cstr());
      const UINT                      n       = (UINT)e.m_key.size();
      UINT                            counter = 0;
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

  { const ActionArrayIndexArray actionListArray = m_actionListMap.getEntryArray();
    UINT                        tableSize       = 0;
    TCHAR                       delim           = ' ';

    outputBeginArrayDefinition(output, _T("actionListTable"), m_actionType, actionListArray.getElementCount(false));
    for(auto it = actionListArray.getIterator(); it.hasNext();) {
      const IndexArrayEntry<ActionArray> &e       = it.next();
      String                              comment = format(_T("%3u %s"), e.m_commentIndex, e.getComment().cstr());
      const UINT                          n       = (UINT)e.m_key.size();
      UINT                                counter = 0;
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
  if(m_termSetMap.size() == 0) {
    output.printf(_T("#define termSetTable nullptr\n\n"));
  } else {
    const TermSetIndexArray termSetArray = m_termSetMap.getEntryArray();
    outputBeginArrayDefinition(output, _T("termSetTable"), TYPE_UCHAR, m_currentTermSetArraySize);
    TCHAR delim = ' ';
    for(auto it = termSetArray.getIterator(); it.hasNext();) {
      const IndexArrayEntry<TermSet> &e       = it.next();
      const ByteArray                 ba      = symbolSetToByteArray(e.m_key);
      const UINT                      n       = (UINT)ba.size();
      String                          comment = format(_T("%3u %3u tokens %s"), e.m_commentIndex, (UINT)e.m_key.size(), e.getComment().cstr());
      for(const BYTE *cp = ba.getData(), *last = cp + n; cp < last; delim = ',') {
        output.printf(_T("%c0x%02x"), delim, *(cp++));
      }
      newLine(output, comment);
    }
    byteCount = outputEndArrayDefinition(output, TYPE_UCHAR, m_currentTermSetArraySize);
  }
  return byteCount;
}

}; // namespace ActionMatrixCompression

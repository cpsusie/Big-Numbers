#include "stdafx.h"
#include <CompactHashMap.h>
#include "GrammarCode.h"
#include "CompressedActionMatrixCpp.h"

namespace ActionMatrixCompression {

CompressedActionMatrix::CompressedActionMatrix(const GrammarTables &tables)
  : MacroMap(              tables                            )
  , m_tables(              tables                            )
  , m_termSetSizeInBytes(( tables.getTermCount() - 1) / 8 + 1)
  , m_termType(            tables.getTermType()              )
  , m_actionType(          tables.getActionType()            )
  , m_stateActionNodeArray(tables                            )
{
  generateCompressedForm();
}

void CompressedActionMatrix::generateCompressedForm() {
  m_currentTermListSize     = 0;
  m_currentActionListSize   = 0;
  m_currentTermSetArraySize = 0;
  m_currentSplitNodeCount   = 0;

  for(UINT state = 0; state < getStateCount(); state++) {
    const StateActionNode *actionNode = m_stateActionNodeArray[state];
    Macro                  macro      = doStateActionNode(*actionNode);
    addMacro(macro.setIndex(state).setName(format(_T("_ac%04u"), state)));
  }
}

Macro CompressedActionMatrix::doStateActionNode(const StateActionNode &actionNode) {
  switch(actionNode.getCompressionMethod()) {
  case AbstractParserTables::CompCodeBinSearch: return doTermListNode( actionNode);
  case AbstractParserTables::CompCodeSplitNode: return doSplitNode(    actionNode);
  case AbstractParserTables::CompCodeImmediate: return doOneItemNode(  actionNode);
  case AbstractParserTables::CompCodeBitset   : return doTermSetNode(  actionNode);
  default                             :
    throwException(_T("%s:Unknown compressionMethod for state %u"), __TFUNCTION__, actionNode.getState());
    break;
  }
  return Macro(getStateCount(), 0, EMPTYSTRING, EMPTYSTRING); // should not come here
}

Macro CompressedActionMatrix::doTermListNode(const StateActionNode &actionNode) {
  const UINT              state      = actionNode.getState();
  const ParserActionArray &termList  = actionNode.getTermList();
  const TermSet           termSet    = termList.getLegalTermSet(getTermCount());
  IndexMapValue          *imvp       = m_termListMap.get(termSet);
  UINT                    termListIndex, termListCount;

  if(imvp != nullptr) {
    termListIndex = imvp->m_arrayIndex;
    termListCount = imvp->m_commentIndex;
    imvp->addUsedByValue(state);
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
    imvp->addUsedByValue(state);
  } else {
    actionListIndex = m_currentActionListSize;
    actionListCount = m_actionListMap.getCount();
    m_actionListMap.put(actionList, IndexMapValue(getStateCount(), state, actionListIndex));
    m_currentActionListSize += (UINT)actionList.size();
  }
  const String            macroValue = encodeMacroValue(AbstractParserTables::CompCodeBinSearch, actionListIndex, termListIndex);
  const String            comment    = format(_T("termList %4u, actionList %4u"), termListCount, actionListCount);
  return Macro(getStateCount(), state, macroValue, comment);
}

Macro CompressedActionMatrix::doSplitNode(const StateActionNode &actionNode) {
  const UINT              state      = actionNode.getState();
  const StateActionNode  &left       = actionNode.getChild(0);
  const StateActionNode  &right      = actionNode.getChild(1);

  UINT                    indexL, indexR;
  Macro                   macroL     = doStateActionNode(left);
  const Macro            *mpL        = findMacroByValue(macroL.getValue());
  if(mpL != nullptr) {
    mpL->addUsedByValue(state);
    indexL = mpL->getIndex();
  } else {
    const String          name       = format(_T("_sn%04u"), m_currentSplitNodeCount);
    indexL = getStateCount() + m_currentSplitNodeCount++;
    addMacro(macroL.setIndex(indexL).setName(name));
    mpL = &macroL;
  }

  Macro                   macroR     = doStateActionNode(right);
  const Macro            *mpR        = findMacroByValue(macroR.getValue());
  if(mpR != nullptr) {
    mpR->addUsedByValue(state);
    indexR = mpR->getIndex();
  } else {
    const String          name       = format(_T("_sn%04u"), m_currentSplitNodeCount);
    indexR = getStateCount() + m_currentSplitNodeCount++;
    addMacro(macroR.setIndex(indexR).setName(name));
    mpR = &macroR;
  }

  const String            macroValue = encodeMacroValue(AbstractParserTables::CompCodeSplitNode, indexL, indexR);
  const String            comment    = format(_T("Split(%s,%s)"), mpL->getName().cstr(), mpR->getName().cstr());
  return Macro(getStateCount(), state, macroValue, comment);
}

Macro CompressedActionMatrix::doOneItemNode(const StateActionNode &actionNode) {
  const UINT              state      = actionNode.getState();
  const ParserAction      pa         = actionNode.getOneItemAction();
  const int               action     = pa.m_action;                    // positive or negative
  const UINT              term       = pa.m_term;
  const String            macroValue = encodeMacroValue(AbstractParserTables::CompCodeImmediate, action, term);
  const String            comment    = (action <= 0)
                                     ? format(_T("Reduce by %3u on %s"), -action, getSymbolName(pa.m_term).cstr())
                                     : format(_T("Shift  to %3u on %s"),  action, getSymbolName(pa.m_term).cstr());
  return Macro(getStateCount(), state, macroValue, comment);
}

Macro CompressedActionMatrix::doTermSetNode(const StateActionNode &actionNode) {
  const UINT              state      = actionNode.getState();
  const TermSetReduction &tsr        = actionNode.getTermSetReduction();

  const TermSet          &termSet    = tsr.getTermSet();
  IndexMapValue          *vp         = m_termSetMap.get(termSet);
  UINT                    byteIndex, termSetCount;
  if(vp != nullptr) {
    byteIndex    = vp->m_arrayIndex;
    termSetCount = vp->m_commentIndex;
    vp->addUsedByValue(state);
  } else {
    byteIndex    = m_currentTermSetArraySize;
    termSetCount = m_termSetMap.getCount();
    m_termSetMap.put(termSet, IndexMapValue(getStateCount(), state, byteIndex));
    m_currentTermSetArraySize += m_termSetSizeInBytes;
  }
  const int               prod       = tsr.getProduction();
  const int               action     = -prod;
  const String            macroValue = encodeMacroValue(AbstractParserTables::CompCodeBitset, action, byteIndex);
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
  { const TermSetIndexArray               termListArray     = m_termListMap.getEntryArray();
    UINT                                  tableSize         = 0;
    TCHAR                                 delim             = ' ';
    outputBeginArrayDefinition(output, _T("termListTable"   ), m_termType, termListArray.getElementCount(true));
    for(auto it = termListArray.getIterator();   it.hasNext();) {
      const IndexArrayEntry<TermSet>     &e                 = it.next();
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
    byteCount += outputEndArrayDefinition(output, m_termType, tableSize);
  }
  { const ActionArrayIndexArray           actionListArray   = m_actionListMap.getEntryArray();
    UINT                                  tableSize         = 0;
    TCHAR                                 delim             = ' ';
    outputBeginArrayDefinition(output, _T("actionListTable") , m_actionType, actionListArray.getElementCount(false));
    for(auto it = actionListArray.getIterator(); it.hasNext();) {
      const IndexArrayEntry<ActionArray> &e                 = it.next();
      String                              comment           = format(_T("%3u %s"), e.m_commentIndex, e.getUsedByComment().cstr());
      const UINT                          n                 = (UINT)e.m_key.size();
      UINT                                counter           = 0;
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
    const TermSetIndexArray              termSetArray       = m_termSetMap.getEntryArray();
    TCHAR                                delim              = ' ';
    outputBeginArrayDefinition(output, _T("termSetTable"), TYPE_UCHAR, m_currentTermSetArraySize);
    for(auto it = termSetArray.getIterator(); it.hasNext();) {
      const IndexArrayEntry<TermSet>     &e                 = it.next();
      String                              comment           = format(_T("%3u %3u tokens %s"), e.m_commentIndex, (UINT)e.m_key.size(), e.getUsedByComment().cstr());
      const ByteArray                     ba                = bitSetToByteArray(e.m_key);
      const UINT                          n                 = (UINT)ba.size();
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

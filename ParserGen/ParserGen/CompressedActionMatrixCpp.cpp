#include "stdafx.h"
#include <CompactHashMap.h>
#include "GrammarCode.h"
#include "CompressedActionMatrixCpp.h"

namespace ActionMatrixCompression {

CompressedActionMatrix::CompressedActionMatrix(const Grammar &grammar)
  : MacroMap(              grammar                                         )
  , m_grammar(             grammar                                         )
  , m_grammarResult(       grammar.getResult()                             )
  , m_templateTypes(       grammar                                         )
  , m_usedByParam(         grammar.getBitSetParam(STATE_BITSET)            )
  , m_sizeofTermBitSet(    getSizeofBitSet(grammar.getTermBitSetCapacity()))
  , m_stateActionNodeArray(grammar                                         )
{
  generateCompressedForm();
  if(!m_grammar.getTermReorderingDone()) {
    TermSet totalTermBitSet(m_grammar.getTermCount());
    for(auto it = m_termBitSetMap.getIterator(); it.hasNext();) {
      totalTermBitSet += it.next().getKey();
    }
    totalTermBitSet.add(0); // make sure, that term 0 (EOI) remains term 0
    m_termBitSetPermutation = OptimizedBitSetPermutation(totalTermBitSet);
  }
}

void CompressedActionMatrix::generateCompressedForm() {
  m_termArraySize     = 0;
  m_actionArraySize   = 0;
  m_splitNodeCount    = 0;

  for(UINT state = 0; state < getStateCount(); state++) {
    const StateActionNode *actionNode = m_stateActionNodeArray[state];
    Macro                  macro      = doStateActionNode(*actionNode);
    addMacro(macro.setIndex(state).setName(format(_T("_ac%04u"), state)));
  }
}

Macro CompressedActionMatrix::doStateActionNode(const StateActionNode &actionNode) {
  switch(actionNode.getCompressionMethod()) {
  case AbstractParserTables::CompCodeBinSearch: return doBinSearchNode(actionNode);
  case AbstractParserTables::CompCodeSplitNode: return doSplitNode(    actionNode);
  case AbstractParserTables::CompCodeImmediate: return doImmediateNode(actionNode);
  case AbstractParserTables::CompCodeBitSet   : return doBitSetNode(   actionNode);
  default                             :
    throwException(_T("%s:Unknown compressionMethod for state %u"), __TFUNCTION__, actionNode.getState());
    break;
  }
  return Macro(m_usedByParam, 0, EMPTYSTRING, EMPTYSTRING); // should not come here
}

Macro CompressedActionMatrix::doBinSearchNode(const StateActionNode &actionNode) {
  const UINT              state      = actionNode.getState();
  const ParserActionArray &termList  = actionNode.getTermList();
  const TermSet           termSet    = termList.getLegalTermSet(getTermCount());
  IndexMapValue          *imvp       = m_termArrayMap.get(termSet);
  UINT                    termListIndex, termListCount;

  if(imvp != nullptr) {
    termListIndex = imvp->m_arrayIndex;
    termListCount = imvp->m_commentIndex;
    imvp->addUsedByValue(state);
  } else {
    termListIndex = m_termArraySize;
    termListCount = m_termArrayMap.getCount();
    m_termArrayMap.put(termSet, IndexMapValue(m_usedByParam, state, termListIndex));
    m_termArraySize += (UINT)termSet.size() + 1;
  }

  const ActionArray actionList = termList.getActionArray();
  imvp = m_actionArrayMap.get(actionList);
  UINT              actionListIndex, actionListCount;
  if(imvp != nullptr) {
    actionListIndex = imvp->m_arrayIndex;
    actionListCount = imvp->m_commentIndex;
    imvp->addUsedByValue(state);
  } else {
    actionListIndex = m_actionArraySize;
    actionListCount = m_actionArrayMap.getCount();
    m_actionArrayMap.put(actionList, IndexMapValue(m_usedByParam, state, actionListIndex));
    m_actionArraySize += (UINT)actionList.size();
  }
  const String            macroValue = encodeMacroValue(AbstractParserTables::CompCodeBinSearch, actionListIndex, termListIndex);
  const String            comment    = format(_T("termArray %4u, actionArray %4u"), termListCount, actionListCount);
  return Macro(m_usedByParam, state, macroValue, comment);
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
    const String          name       = format(_T("_as%04u"), m_splitNodeCount);
    indexL = getStateCount() + m_splitNodeCount++;
    addMacro(macroL.setIndex(indexL).setName(name));
    mpL = &macroL;
  }

  Macro                   macroR     = doStateActionNode(right);
  const Macro            *mpR        = findMacroByValue(macroR.getValue());
  if(mpR != nullptr) {
    mpR->addUsedByValue(state);
    indexR = mpR->getIndex();
  } else {
    const String          name       = format(_T("_as%04u"), m_splitNodeCount);
    indexR = getStateCount() + m_splitNodeCount++;
    addMacro(macroR.setIndex(indexR).setName(name));
    mpR = &macroR;
  }

  const String            macroValue = encodeMacroValue(AbstractParserTables::CompCodeSplitNode, indexL, indexR);
  const String            comment    = format(_T("Split(%s,%s)"), mpL->getName().cstr(), mpR->getName().cstr());
  return Macro(m_usedByParam, state, macroValue, comment);
}

Macro CompressedActionMatrix::doImmediateNode(const StateActionNode &actionNode) {
  const UINT              state      = actionNode.getState();
  const ParserAction      pa         = actionNode.getOneItemAction();
  const int               action     = pa.m_action;                    // positive or negative
  const UINT              term       = pa.m_term;
  const String            macroValue = encodeMacroValue(AbstractParserTables::CompCodeImmediate, action, term);
  const String            comment    = (action <= 0)
                                     ? format(_T("Reduce by %4u on %s"), -action, getSymbolName(pa.m_term).cstr())
                                     : format(_T("Shift  to %4u on %s"),  action, getSymbolName(pa.m_term).cstr());
  return Macro(m_usedByParam, state, macroValue, comment);
}

Macro CompressedActionMatrix::doBitSetNode(const StateActionNode &actionNode) {
  const UINT              state      = actionNode.getState();
  const TermSetReduction &tsr        = actionNode.getTermSetReduction();

  const TermSet          &termSet    = tsr.getTermSet();
  IndexMapValue          *vp         = m_termBitSetMap.get(termSet);
  UINT                    byteIndex, termSetCount;
  if(vp != nullptr) {
    termSetCount = vp->m_commentIndex;
    byteIndex    = vp->m_arrayIndex;
    vp->addUsedByValue(state);
  } else {
    termSetCount = m_termBitSetMap.getCount();
    byteIndex    = termSetCount * m_sizeofTermBitSet;
    m_termBitSetMap.put(termSet, IndexMapValue(m_usedByParam, state, byteIndex));
  }
  const int               prod       = tsr.getProduction();
  const int               action     = -prod;
  const String            macroValue = encodeMacroValue(AbstractParserTables::CompCodeBitSet, action, byteIndex);
  const String            comment    = format(_T("Reduce by %4u on tokens in termBitSet[%u]"), prod, termSetCount);
  return Macro(m_usedByParam, state, macroValue, comment);
}

// ------------------------------------ Print ------------------------------------------------

ByteCount CompressedActionMatrix::print(MarginFile &output) const {
  ByteCount byteCount;
  byteCount += printMacroesAndActionCodeArray(output);
  byteCount += printTermAndActionArrayTable(   output);
  byteCount += printTermBitSetTable(        output);
  return byteCount;
}

ByteCount CompressedActionMatrix::printMacroesAndActionCodeArray(MarginFile &output) const {
  const UINT   macroCount = getMacroCount();
  const UINT   stateCount = getStateCount();
  Array<Macro> macroes(getMacroArray());
  if(macroCount > 0) {
    const UINT commentLen = getMaxCommentLength() + 1;
    macroes.sort(macroCmpByName);
    for(auto it = macroes.getIterator(); it.hasNext();) {
      it.next().print(output, commentLen);
    }
    output.printf(_T("\n"));
  }

  outputBeginArrayDefinition(output, _T("actionCodeArray"), TYPE_UINT, macroCount);
  macroes.sort(macroCmpByIndex);
  TCHAR delim = ' ';
  UINT  count = 0;
  auto it = macroes.getIterator();
  for(;it.hasNext() && (count < stateCount); count++, delim = ',') {
    output.printf(_T("%c%s"), delim, it.next().getName().cstr());
    if((count % 10 == 9) && (count != stateCount - 1)) {
      output.printf(_T("\n"));
    }
  }
  if(it.hasNext()) {
    if(output.getCurrentLineLength() > 0) {
      output.printf(_T("\n"));
      count = 0;
    }
    for(; it.hasNext(); delim = ',') {
      output.printf(_T("%c%s"), delim, it.next().getName().cstr());
      if((count++ % 10 == 9) && it.hasNext()) {
        output.printf(_T("\n"));
      }
    }
  }
  return outputEndArrayDefinition(output, TYPE_UINT, macroCount, true);
}

ByteCount CompressedActionMatrix::printTermAndActionArrayTable(MarginFile &output) const {
  ByteCount byteCount;
  if(m_termArraySize == 0) {
    output.printf(_T("#define termArrayTable   nullptr\n"  ));
    output.printf(_T("#define actionArrayTable nullptr\n\n"));
    return byteCount;
  }

  { const TermSetIndexArray               termArrayTable    = m_termArrayMap.getEntryArray();
    UINT                                  tableSize         = 0;
    TCHAR                                 delim             = ' ';
    outputBeginArrayDefinition(output, _T("termArrayTable"   ), m_templateTypes.getTermType(), termArrayTable.getElementCount(true));
    for(auto it = termArrayTable.getIterator();   it.hasNext();) {
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
    byteCount += outputEndArrayDefinition(output, m_templateTypes.getTermType(), tableSize);
  }
  { const ActionArrayIndexArray           actionArrayTable  = m_actionArrayMap.getEntryArray();
    UINT                                  tableSize         = 0;
    TCHAR                                 delim             = ' ';
    outputBeginArrayDefinition(output, _T("actionArrayTable") , m_templateTypes.getActionType(), actionArrayTable.getElementCount(false));
    for(auto it = actionArrayTable.getIterator(); it.hasNext();) {
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
    byteCount += outputEndArrayDefinition(output, m_templateTypes.getActionType(), tableSize);
  }
  return byteCount;
}

ByteCount CompressedActionMatrix::printTermBitSetTable(MarginFile &output) const {
  ByteCount byteCount;
  if(m_termBitSetMap.size() == 0) {
    output.printf(_T("#define termBitSetTable nullptr\n\n"));
  } else {
    const TermSetIndexArray               termBitSetArray   = m_termBitSetMap.getEntryArray();
    TCHAR                                 delim             = ' ';
    const UINT                            arraySize         = (UINT)m_termBitSetMap.size() * m_sizeofTermBitSet;
    outputBeginArrayDefinition(output, _T("termBitSetTable"), TYPE_UCHAR, arraySize);
    for(auto it = termBitSetArray.getIterator(); it.hasNext();) {
      const IndexArrayEntry<TermSet>     &e                 = it.next();
      String                              comment           = format(_T("%3u %3u tokens %s"), e.m_commentIndex, (UINT)e.m_key.size(), e.getUsedByComment().cstr());
      const ByteArray                     ba                = bitSetToByteArray(e.m_key, m_grammar.getTermBitSetCapacity());
      for(BYTE b : ba) {
        output.printf(_T("%c0x%02x"), delim, b);
        delim = ',';
      }
      newLine(output, comment);
    }
    byteCount = outputEndArrayDefinition(output, TYPE_UCHAR, arraySize);
  }
  return byteCount;
}

}; // namespace ActionMatrixCompression

#include "stdafx.h"
#include <CompactHashMap.h>
#include "GrammarCode.h"
#include "CompressEncoding.h"
#include "CompressedTransSuccMatrixCpp.h"

namespace TransSuccMatrixCompression {

CompressedTransSuccMatrix::CompressedTransSuccMatrix(const GrammarTables &tables)
  : MacroMap(              tables                                             )
  , m_tables(              tables                                             )
  , m_stateSetSizeInBytes((tables.getStateCount() - 1) / 8 + 1                )
  , m_NTindexType(         tables.getNTindexType()                            )
  , m_stateType(           tables.getStateType()                              )
  , m_NTindexArray(        tables                                             )
{
  generateCompressedForm();
}

void CompressedTransSuccMatrix::generateCompressedForm() {
  m_currentFromStateListSize = 0;
  m_currentNewStateListSize  = 0;
  m_currentStateSetArraySize = 0;
  m_currentSplitNodeCount    = 0;
  for(UINT NTindex = 0; NTindex < getNTermCount(); NTindex++) {
    const NTindexNode *node  = m_NTindexArray[NTindex];
    if(node == nullptr) {
      addMacro(Macro(10, 0, _T("0x00000000"), _T("")).setIndex(NTindex).setName(format(_T("_ns%04u"), NTindex)));
    } else {
      Macro macro = doNTindexNode(*node);

      addMacro(macro.setIndex(NTindex).setName(format(_T("_ns%04u"), NTindex)));
    }
  }
}

Macro CompressedTransSuccMatrix::doNTindexNode(const NTindexNode &node) {
  switch(node.getCompressionMethod()) {
  case AbstractParserTables::CompCodeBinSearch: return doStatePairListNode( node);
  case AbstractParserTables::CompCodeSplitNode: return doSplitNode(         node);
  case AbstractParserTables::CompCodeImmediate: return doOneStatePairNode(  node);
  case AbstractParserTables::CompCodeBitset   : return doStatePairSetNode(  node);
  default                             :
    throwException(_T("%s:Unknown compressionMethod for state %u"), __TFUNCTION__, node.getNTindex());
    break;
  }
  return Macro(getStateCount(), 0, EMPTYSTRING, EMPTYSTRING); // should not come here
}

Macro CompressedTransSuccMatrix::doStatePairListNode(const NTindexNode &node) {
  const UINT            NTindex       = node.getNTindex();
  const StatePairArray &statePairList = node.getStatePairList();
  const NTindexSet      ntIndexSet    = statePairList.getFromStateSet(getStateCount());
  IndexMapValue        *imvp          = m_fromStateListMap.get(ntIndexSet);
  UINT                  fromStateListIndex, fromStateListCount;

  if(imvp != nullptr) {
    fromStateListIndex = imvp->m_arrayIndex;
    fromStateListCount = imvp->m_commentIndex;
    imvp->addUsedByValue(NTindex);
  } else {
    fromStateListIndex = m_currentFromStateListSize;
    fromStateListCount = m_fromStateListMap.getCount();
    m_fromStateListMap.put(ntIndexSet, IndexMapValue(getNTermCount(), NTindex, fromStateListIndex));
    m_currentFromStateListSize += (UINT)ntIndexSet.size() + 1;
  }

  const StateArray      newStateList  = statePairList.getNewStateArray();
  imvp = m_newStateListMap.get(newStateList);
  UINT                  newStateListIndex, newStateListCount;
  if(imvp != nullptr) {
    newStateListIndex = imvp->m_arrayIndex;
    newStateListCount = imvp->m_commentIndex;
    imvp->addUsedByValue(NTindex);
  } else {
    newStateListIndex = m_currentNewStateListSize;
    newStateListCount = m_newStateListMap.getCount();
    m_newStateListMap.put(newStateList, IndexMapValue(getNTermCount(), NTindex, newStateListIndex));
    m_currentNewStateListSize += (UINT)newStateList.size();
  }
  const String          macroValue    = encodeMacroValue(AbstractParserTables::CompCodeBinSearch, newStateListIndex, fromStateListIndex);
  const String          comment       = format(_T("stateList %4u, newStateList %4u"), fromStateListCount , newStateListCount );
  return Macro(getStateCount(), NTindex, macroValue, comment);
}

Macro CompressedTransSuccMatrix::doSplitNode(const NTindexNode &node) {
  const UINT            NTindex       = node.getNTindex();
  const NTindexNode    &left          = node.getChild(0);
  const NTindexNode    &right         = node.getChild(1);

  UINT                  indexL, indexR;
  Macro                 macroL        = doNTindexNode(left);
  const Macro          *mpL           = findMacroByValue(macroL.getValue());
  if(mpL != nullptr) {
    mpL->addUsedByValue(NTindex);
    indexL = mpL->getIndex();
  } else {
    const String        name          = format(_T("_nt%04u"), m_currentSplitNodeCount);
    indexL = getNTermCount() + m_currentSplitNodeCount++;
    addMacro(macroL.setIndex(indexL).setName(name));
    mpL = &macroL;
  }

  Macro                 macroR        = doNTindexNode(right);
  const Macro          *mpR           = findMacroByValue(macroR.getValue());
  if(mpR != nullptr) {
    mpR->addUsedByValue(NTindex);
    indexR = mpR->getIndex();
  } else {
    const String        name          = format(_T("_nt%04u"), m_currentSplitNodeCount);
    indexR = getNTermCount() + m_currentSplitNodeCount++;
    addMacro(macroR.setIndex(indexR).setName(name));
    mpR = &macroR;
  }

  const String          macroValue    = encodeMacroValue(AbstractParserTables::CompCodeSplitNode, indexL, indexR);
  const String          comment       = format(_T("Split(%s,%s)"), mpL->getName().cstr(), mpR->getName().cstr());
  return Macro(getNTermCount(), NTindex, macroValue, comment);
}

Macro CompressedTransSuccMatrix::doOneStatePairNode(const NTindexNode &node) {
  const UINT            NTindex       = node.getNTindex();
  const StatePair       sp            = node.getStatePair();
  const UINT            newState      = sp.m_newState;
  const UINT            fromState     = sp.m_fromState;
  const String          macroValue    = encodeMacroValue(AbstractParserTables::CompCodeImmediate, newState, fromState);
  const String          comment       = format(_T("Goto %3u on %3u"), newState, fromState);
  return Macro(getNTermCount(), NTindex, macroValue, comment);
}

Macro CompressedTransSuccMatrix::doStatePairSetNode(const NTindexNode &node) {
  const UINT            NTindex       = node.getNTindex();
  const StatePairSet   &sps           = node.getStatePairSet();

  const StateSet       &stateSet      = sps.getFromStateSet();
  IndexMapValue        *vp            = m_stateSetMap.get(stateSet);
  UINT                  byteIndex, stateSetCount;
  if(vp != nullptr) {
    byteIndex     = vp->m_arrayIndex;
    stateSetCount = vp->m_commentIndex;
    vp->addUsedByValue(NTindex);
  } else {
    byteIndex     = m_currentStateSetArraySize;
    stateSetCount = m_stateSetMap.getCount();
    m_stateSetMap.put(stateSet, IndexMapValue(getNTermCount(), NTindex, byteIndex));
    m_currentStateSetArraySize += m_stateSetSizeInBytes;
  }
  const UINT            newState      = sps.getNewState();
  const String          macroValue    = encodeMacroValue(AbstractParserTables::CompCodeBitset, newState, byteIndex);
  const String          comment       = format(_T("Goto %u on states in stateSet[%u]"), newState, stateSetCount);
  return Macro(getNTermCount(), NTindex, macroValue, comment);
}

// ------------------------------------ Print ------------------------------------------------

ByteCount CompressedTransSuccMatrix::print(MarginFile &output) const {
  ByteCount byteCount;
  byteCount += printMacroesAndSuccCode(output);
  byteCount += printStatePairListTable(output);
  byteCount += printStatePairSetTable( output);
  return byteCount;
}

ByteCount CompressedTransSuccMatrix::printMacroesAndSuccCode(MarginFile &output) const {
  const UINT   macroCount = getMacroCount();
  Array<Macro> macroes(getMacroArray());
  if(macroCount > 0) {
    macroes.sort(macroCmpByName);
    for(auto it = macroes.getIterator(); it.hasNext();) {
      it.next().print(output);
    }
    output.printf(_T("\n"));
  }

  outputBeginArrayDefinition(output, _T("succCode"), TYPE_UINT, macroCount);
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

ByteCount CompressedTransSuccMatrix::printStatePairListTable(MarginFile &output) const {
  ByteCount byteCount;
  if(m_currentFromStateListSize == 0) {
    output.printf(_T("#define stateListTable    nullptr\n"  ));
    output.printf(_T("#define newStateListTable nullptr\n\n"));
    return byteCount;
  }
  { const StateSetIndexArray              stateListArray     = m_fromStateListMap.getEntryArray();
    UINT                                  tableSize          = 0;
    TCHAR                                 delim              = ' ';
    outputBeginArrayDefinition(output, _T("stateListTable"   ), m_stateType, stateListArray.getElementCount(true));
    for(auto it = stateListArray.getIterator();   it.hasNext();) {
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
    byteCount += outputEndArrayDefinition(output, m_stateType, tableSize);
  }
  { const StateArrayIndexArray            newStateListArray = m_newStateListMap.getEntryArray();
    UINT                                  tableSize         = 0;
    TCHAR                                 delim             = ' ';
    outputBeginArrayDefinition(output, _T("newStateListTable") , m_stateType, newStateListArray.getElementCount(false));
    for(auto it = newStateListArray.getIterator(); it.hasNext();) {
      const IndexArrayEntry<StateArray>  &e                 = it.next();
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
    byteCount += outputEndArrayDefinition(output, m_stateType, tableSize);
  }
  return byteCount;
}

ByteCount CompressedTransSuccMatrix::printStatePairSetTable(MarginFile &output) const {
  ByteCount byteCount;
  if(m_stateSetMap.size() == 0) {
    output.printf(_T("#define stateSetTable nullptr\n\n"));
  } else {
    const StateSetIndexArray              stateSetArray     = m_stateSetMap.getEntryArray();
    TCHAR                                 delim             = ' ';
    outputBeginArrayDefinition(output, _T("stateSetTable"), TYPE_UCHAR, m_currentStateSetArraySize);
    for(auto it = stateSetArray.getIterator(); it.hasNext();) {
      const IndexArrayEntry<StateSet>    &e                 = it.next();
      String                              comment           = format(_T("%3u %3u tokens %s"), e.m_commentIndex, (UINT)e.m_key.size(), e.getUsedByComment().cstr());
      const ByteArray                     ba                = bitSetToByteArray(e.m_key);
      const UINT                          n                 = (UINT)ba.size();
      for(const BYTE *cp = ba.getData(), *last = cp + n; cp < last; delim = ',') {
        output.printf(_T("%c0x%02x"), delim, *(cp++));
      }
      newLine(output, comment);
    }
    byteCount = outputEndArrayDefinition(output, TYPE_UCHAR, m_currentStateSetArraySize);
  }
  return byteCount;
}

}; // namespace TransSuccMatrixCompression

#include "stdafx.h"
#include <CompactHashMap.h>
#include "GrammarCode.h"
#include "CompressEncoding.h"
#include "CompressedTransSuccMatrixCpp.h"

namespace TransSuccMatrixCompression {

CompressedTransSuccMatrix::CompressedTransSuccMatrix(const GrammarTables &tables)
  : MacroMap(              tables                                                )
  , m_tables(              tables                                                )
  , m_usedByParam(         tables.getGrammarCode().getBitSetParam(NTINDEX_BITSET))
  , m_stateSetSizeInBytes((tables.getStateCount() - 1) / 8 + 1                   )
  , m_maxNTermNameLength(  tables.getMaxNTermNameLength()                        )
  , m_NTindexType(         tables.getNTindexType()                               )
  , m_stateType(           tables.getStateType()                                 )
  , m_NTindexNodeArray(    tables                                                )
{
  generateCompressedForm();
}

void CompressedTransSuccMatrix::generateCompressedForm() {
  m_fromStateArraySize = 0;
  m_newStateArraySize  = 0;
  m_splitNodeCount     = 0;
  CompactUIntArray undefinedEntries;
  const UINT       termCount = getTermCount();
  for(UINT NTindex = 0; NTindex < getNTermCount(); NTindex++) {
    const NTindexNode *node = m_NTindexNodeArray[NTindex];
    if(node == nullptr) {
      undefinedEntries.add(NTindex);
    } else {
      Macro macro = doNTindexNode(*node);
      const String comment = macro.getComment();
      addMacro(macro.setComment(format(_T("%-*s %s"), m_maxNTermNameLength, getSymbolName(NTindex + termCount).cstr(), comment.cstr()))
                    .setIndex(NTindex)
                    .setName(format(_T("_sc%04u"), NTindex))
              );
    }
  }
  for(UINT NTindex : undefinedEntries) {
    addMacro(Macro(m_usedByParam, NTindex, _T("0x00000000"), _T("")).setIndex(NTindex).setName(format(_T("_sc%04u"), NTindex)));
  }
}

Macro CompressedTransSuccMatrix::doNTindexNode(const NTindexNode &node) {
  switch(node.getCompressionMethod()) {
  case AbstractParserTables::CompCodeBinSearch: return doBinSearchNode( node);
  case AbstractParserTables::CompCodeSplitNode: return doSplitNode(     node);
  case AbstractParserTables::CompCodeImmediate: return doImmediateNode( node);
  case AbstractParserTables::CompCodeBitSet   : return doBitSetNode(    node);
  default                             :
    throwException(_T("%s:Unknown compressionMethod for state %u"), __TFUNCTION__, node.getNTindex());
    break;
  }
  return Macro(m_usedByParam, 0, EMPTYSTRING, EMPTYSTRING); // should not come here
}

Macro CompressedTransSuccMatrix::doBinSearchNode(const NTindexNode &node) {
  const UINT            NTindex        = node.getNTindex();
  const StatePairArray &statePairArray = node.getStatePairArray();
  const NTindexSet      fromStateSet   = statePairArray.getFromStateSet(getStateCount());
  IndexMapValue        *imvp           = m_fromStateArrayMap.get(fromStateSet);
  UINT                  fromStateArrayIndex, fromStateArrayCount;

  if(imvp != nullptr) {
    fromStateArrayIndex = imvp->m_arrayIndex;
    fromStateArrayCount = imvp->m_commentIndex;
    imvp->addUsedByValue(NTindex);
  } else {
    fromStateArrayIndex = m_fromStateArraySize;
    fromStateArrayCount = m_fromStateArrayMap.getCount();
    m_fromStateArrayMap.put(fromStateSet, IndexMapValue(m_usedByParam, NTindex, fromStateArrayIndex));
    m_fromStateArraySize += (UINT)fromStateSet.size() + 1;
  }

  const StateArray      newStateList  = statePairArray.getNewStateArray();
  imvp = m_newStateArrayMap.get(newStateList);
  UINT                  newStateListIndex, newStateArrayCount;
  if(imvp != nullptr) {
    newStateListIndex  = imvp->m_arrayIndex;
    newStateArrayCount = imvp->m_commentIndex;
    imvp->addUsedByValue(NTindex);
  } else {
    newStateListIndex  = m_newStateArraySize;
    newStateArrayCount = m_newStateArrayMap.getCount();
    m_newStateArrayMap.put(newStateList, IndexMapValue(m_usedByParam, NTindex, newStateListIndex));
    m_newStateArraySize += (UINT)newStateList.size();
  }
  const String          macroValue    = encodeMacroValue(AbstractParserTables::CompCodeBinSearch, newStateListIndex, fromStateArrayIndex);
  const String          comment       = format(_T("stateArray %4u, newStateArray %4u"), fromStateArrayCount , newStateArrayCount );
  return Macro(m_usedByParam, NTindex, macroValue, comment);
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
    const String        name          = format(_T("_ss%04u"), m_splitNodeCount);
    indexL = getNTermCount() + m_splitNodeCount++;
    addMacro(macroL.setIndex(indexL).setName(name));
    mpL = &macroL;
  }

  Macro                 macroR        = doNTindexNode(right);
  const Macro          *mpR           = findMacroByValue(macroR.getValue());
  if(mpR != nullptr) {
    mpR->addUsedByValue(NTindex);
    indexR = mpR->getIndex();
  } else {
    const String        name          = format(_T("_ss%04u"), m_splitNodeCount);
    indexR = getNTermCount() + m_splitNodeCount++;
    addMacro(macroR.setIndex(indexR).setName(name));
    mpR = &macroR;
  }

  const String          macroValue    = encodeMacroValue(AbstractParserTables::CompCodeSplitNode, indexL, indexR);
  const String          comment       = format(_T("Split(%s,%s)"), mpL->getName().cstr(), mpR->getName().cstr());
  return Macro(m_usedByParam, NTindex, macroValue, comment);
}

Macro CompressedTransSuccMatrix::doImmediateNode(const NTindexNode &node) {
  const UINT            NTindex       = node.getNTindex();
  const StatePair       sp            = node.getStatePair();
  const UINT            newState      = sp.m_newState;
  const UINT            fromState     = sp.m_fromState;
  const String          macroValue    = encodeMacroValue(AbstractParserTables::CompCodeImmediate, newState, fromState);
  const String          comment       = node.isDontCareNode() ? format(_T("Goto %3u"), newState) : format(_T("Goto %3u on %3u"), newState, fromState);
  return Macro(m_usedByParam, NTindex, macroValue, comment);
}

Macro CompressedTransSuccMatrix::doBitSetNode(const NTindexNode &node) {
  const UINT             NTindex      = node.getNTindex();
  const StatePairBitSet &sps          = node.getStatePairBitSet();

  const StateSet       &stateSet      = sps.getFromStateSet();
  IndexMapValue        *vp            = m_stateBitSetMap.get(stateSet);
  UINT                  byteIndex, stateSetCount;
  if(vp != nullptr) {
    byteIndex     = vp->m_arrayIndex;
    stateSetCount = vp->m_commentIndex;
    vp->addUsedByValue(NTindex);
  } else {
    stateSetCount = m_stateBitSetMap.getCount();
    byteIndex     = stateSetCount * m_stateSetSizeInBytes;
    m_stateBitSetMap.put(stateSet, IndexMapValue(m_usedByParam, NTindex, byteIndex));
  }
  const UINT            newState      = sps.getNewState();
  const String          macroValue    = encodeMacroValue(AbstractParserTables::CompCodeBitSet, newState, byteIndex);
  const String          comment       = format(_T("Goto %u on states in stateBitSet[%u]"), newState, stateSetCount);
  return Macro(m_usedByParam, NTindex, macroValue, comment);
}

// ------------------------------------ Print ------------------------------------------------

ByteCount CompressedTransSuccMatrix::print(MarginFile &output) const {
  ByteCount byteCount;
  byteCount += printMacroesAndSuccessorCodeArray(output);
  byteCount += printStatePairArrayTables(output);
  byteCount += printStateBitSetTable( output);
  return byteCount;
}

ByteCount CompressedTransSuccMatrix::printMacroesAndSuccessorCodeArray(MarginFile &output) const {
  const UINT   macroCount = getMacroCount();
  Array<Macro> macroes(getMacroArray());
  if(macroCount > 0) {
    const UINT commentLen = getMaxCommentLength() + 1;
    macroes.sort(macroCmpByName);
    for(auto it = macroes.getIterator(); it.hasNext();) {
      it.next().print(output, commentLen);
    }
    output.printf(_T("\n"));
  }

  outputBeginArrayDefinition(output, _T("successorCodeArray"), TYPE_UINT, macroCount);
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

ByteCount CompressedTransSuccMatrix::printStatePairArrayTables(MarginFile &output) const {
  ByteCount byteCount;
  if(m_fromStateArraySize == 0) {
    output.printf(_T("#define stateArrayTable    nullptr\n"  ));
    output.printf(_T("#define newStateArrayTable nullptr\n\n"));
    return byteCount;
  }
  { const StateSetIndexArray              stateListArray     = m_fromStateArrayMap.getEntryArray();
    UINT                                  tableSize          = 0;
    TCHAR                                 delim              = ' ';
    outputBeginArrayDefinition(output, _T("stateArrayTable"   ), m_stateType, stateListArray.getElementCount(true));
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
  { const StateArrayIndexArray            newStateListArray = m_newStateArrayMap.getEntryArray();
    UINT                                  tableSize         = 0;
    TCHAR                                 delim             = ' ';
    outputBeginArrayDefinition(output, _T("newStateArrayTable") , m_stateType, newStateListArray.getElementCount(false));
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

ByteCount CompressedTransSuccMatrix::printStateBitSetTable(MarginFile &output) const {
  ByteCount byteCount;
  if(m_stateBitSetMap.size() == 0) {
    output.printf(_T("#define stateBitSetTable nullptr\n\n"));
  } else {
    const StateSetIndexArray              stateSetArray     = m_stateBitSetMap.getEntryArray();
    TCHAR                                 delim             = ' ';
    const UINT                            arraySize         = (UINT)m_stateBitSetMap.size() * m_stateSetSizeInBytes;
    outputBeginArrayDefinition(output, _T("stateBitSetTable"), TYPE_UCHAR, arraySize);
    for(auto it = stateSetArray.getIterator(); it.hasNext();) {
      const IndexArrayEntry<StateSet>    &e                 = it.next();
      String                              comment           = format(_T("%3u %3u states %s"), e.m_commentIndex, (UINT)e.m_key.size(), e.getUsedByComment().cstr());
      const ByteArray                     ba                = bitSetToByteArray(e.m_key);
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

}; // namespace TransSuccMatrixCompression

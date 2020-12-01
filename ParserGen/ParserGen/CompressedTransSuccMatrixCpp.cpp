#include "stdafx.h"
#include <CompactHashMap.h>
#include "GrammarCode.h"
#include "CompressEncoding.h"
#include "StatePairMatrix.h"
#include "CompressedTransSuccMatrixCpp.h"

namespace TransposedSuccessorMatrixCompression {

CompressedTransSuccMatrix::CompressedTransSuccMatrix(const Grammar &grammar)
  : MacroMap(            grammar                               )
  , m_grammar(           grammar                               )
  , m_grammarResult(     grammar.getResult()                   )
  , m_usedByParam(       grammar.getBitSetParam(NTINDEX_BITSET))
  , m_bitSetInterval(    grammar.getSuccStateBitSetInterval()  )
  , m_maxNTermNameLength(grammar.getMaxNTermNameLength()       )
  , m_ntIndexNodeArray(TransposedSuccessorMatrix(grammar)      )
  , m_byteCountMap(grammar                                     )
{
  generateCompressedForm();
}

BitSet CompressedTransSuccMatrix::getBitSetsTableUnion() const {
  StateSet result(getStateCount());
  for(auto it = m_stateBitSetMap.getIterator(); it.hasNext();) {
    result += it.next().getKey();
  }
  return result;
}

void CompressedTransSuccMatrix::findStateBitSetPermutation() const {
  BitSet tmp = getBitSetsTableUnion();
  tmp.add(0); // make sure, that state 0 remains state 0
  m_stateBitSetPermutation = OptimizedBitSetPermutation(tmp);
}

const OptimizedBitSetPermutation &CompressedTransSuccMatrix::getStateBitSetPermutation() const {
  if(m_stateBitSetPermutation.isEmpty()) {
    findStateBitSetPermutation();
  }
  return m_stateBitSetPermutation;
}

void CompressedTransSuccMatrix::generateCompressedForm() {
  m_fromStateArraySize = 0;
  m_newStateArraySize  = 0;
  m_splitNodeCount     = 0;
  CompactUIntArray undefinedEntries;
  const UINT       termCount = getTermCount();
  for(UINT ntIndex = 0; ntIndex < getNTermCount(); ntIndex++) {
    const SymbolNode *node = m_ntIndexNodeArray[ntIndex];
    if(node == nullptr) {
      undefinedEntries.add(ntIndex);
    } else {
      Macro macro = doNTIndexNode(*node);
      const String comment = macro.getComment();
      addMacro(macro.setComment(format(_T("%-*s %s"), m_maxNTermNameLength, getSymbolName(ntIndex + termCount).cstr(), comment.cstr()))
                    .setIndex(ntIndex)
                    .setName(format(_T("_sc%04u"), ntIndex))
              );
    }
  }
  for(UINT ntIndex : undefinedEntries) {
    addMacro(Macro(m_usedByParam, ntIndex, _T("0x00000000"), _T("")).setIndex(ntIndex).setName(format(_T("_sc%04u"), ntIndex)));
  }
  m_byteCountMap.m_splitNodeCount = m_splitNodeCount;
}

Macro CompressedTransSuccMatrix::doNTIndexNode(const SymbolNode &node) {
  switch(node.getCompressionMethod()) {
  case AbstractParserTables::CompCodeBinSearch: return doBinSearchNode( node);
  case AbstractParserTables::CompCodeSplitNode: return doSplitNode(     node);
  case AbstractParserTables::CompCodeImmediate: return doImmediateNode( node);
  case AbstractParserTables::CompCodeBitSet   : return doBitSetNode(    node);
  default                                     :
    throwException(_T("%s:Unknown compressionMethod for state %u"), __TFUNCTION__, node.getNTIndex());
    break;
  }
  return Macro(m_usedByParam, 0, EMPTYSTRING, EMPTYSTRING); // should not come here
}

Macro CompressedTransSuccMatrix::doBinSearchNode(const SymbolNode &node) {
  const UINT            ntIndex        = node.getNTIndex();
  const StatePairArray &statePairArray = node.getStatePairArray();
  const NTIndexSet      fromStateSet   = statePairArray.getFromStateSet(getStateCount());
  IndexMapValue        *imvp           = m_fromStateArrayMap.get(fromStateSet);
  UINT                  fromStateArrayIndex, fromStateArrayCount;

  if(imvp != nullptr) {
    fromStateArrayIndex = imvp->m_arrayIndex;
    fromStateArrayCount = imvp->m_commentIndex;
    imvp->addUsedByValue(ntIndex);
  } else {
    fromStateArrayIndex = m_fromStateArraySize;
    fromStateArrayCount = m_fromStateArrayMap.getCount();
    m_fromStateArrayMap.put(fromStateSet, IndexMapValue(m_usedByParam, ntIndex, fromStateArrayIndex));
    m_fromStateArraySize += (UINT)fromStateSet.size() + 1;
  }

  const StateArray      newStateList   = statePairArray.getNewStateArray();
  imvp = m_newStateArrayMap.get(newStateList);
  UINT                  newStateListIndex, newStateArrayCount;
  if(imvp != nullptr) {
    newStateListIndex  = imvp->m_arrayIndex;
    newStateArrayCount = imvp->m_commentIndex;
    imvp->addUsedByValue(ntIndex);
  } else {
    newStateListIndex  = m_newStateArraySize;
    newStateArrayCount = m_newStateArrayMap.getCount();
    m_newStateArrayMap.put(newStateList, IndexMapValue(m_usedByParam, ntIndex, newStateListIndex));
    m_newStateArraySize += (UINT)newStateList.size();
  }
  const String           macroValue     = encodeMacroValue(CompCodeBinSearch, newStateListIndex, fromStateArrayIndex);
  const String           comment        = format(_T("stateArray %4u, newStateArray %4u"), fromStateArrayCount , newStateArrayCount );
  return Macro(m_usedByParam, ntIndex, macroValue, comment);
}

Macro CompressedTransSuccMatrix::doSplitNode(const SymbolNode &node) {
  const UINT             ntIndex        = node.getNTIndex();
  const SymbolNode      &left           = node.getChild(0);
  const SymbolNode      &right          = node.getChild(1);

  UINT                   indexL, indexR;
  Macro                  macroL         = doNTIndexNode(left);
  const Macro           *mpL            = findMacroByValue(macroL.getValue());
  if(mpL != nullptr) {
    mpL->addUsedByValue(ntIndex);
    indexL = mpL->getIndex();
  } else {
    const String         name            = format(_T("_ss%04u"), m_splitNodeCount);
    indexL = getNTermCount() + m_splitNodeCount++;
    addMacro(macroL.setIndex(indexL).setName(name));
    mpL = &macroL;
  }

  Macro                  macroR         = doNTIndexNode(right);
  const Macro           *mpR            = findMacroByValue(macroR.getValue());
  if(mpR != nullptr) {
    mpR->addUsedByValue(ntIndex);
    indexR = mpR->getIndex();
  } else {
    const String         name           = format(_T("_ss%04u"), m_splitNodeCount);
    indexR = getNTermCount() + m_splitNodeCount++;
    addMacro(macroR.setIndex(indexR).setName(name));
    mpR = &macroR;
  }

  const String           macroValue     = encodeMacroValue(CompCodeSplitNode, indexL, indexR);
  const String           comment        = format(_T("Split(%s,%s)"), mpL->getName().cstr(), mpR->getName().cstr());
  return Macro(m_usedByParam, ntIndex, macroValue, comment);
}

Macro CompressedTransSuccMatrix::doImmediateNode(const SymbolNode &node) {
  const UINT             ntIndex        = node.getNTIndex();
  const StatePair        sp             = node.getStatePair();
  const UINT             newState       = sp.m_newState;
  const UINT             fromState      = sp.m_fromState;
  const UINT             fromStateCount = node.getFromStateCount();
  const String           macroValue     = encodeMacroValue(CompCodeImmediate, newState, fromState);
  const String           comment        = node.isDontCareNode()
                                        ? ( (fromStateCount == 1)
                                           ? format(_T("Goto %4u No check (  1 state )"), newState)
                                           : format(_T("Goto %4u No check (%3u states)"), newState, fromStateCount)
                                          )
                                        : format(_T("Goto %4u on %3u"), newState, fromState);
  return Macro(m_usedByParam, ntIndex, macroValue, comment);
}

Macro CompressedTransSuccMatrix::doBitSetNode(const SymbolNode &node) {
  const UINT             ntIndex        = node.getNTIndex();
  const StatePairBitSet &sps            = node.getStatePairBitSet();

  const StateSet        &stateSet       = sps.getFromStateSet();
  IndexMapValue         *vp             = m_stateBitSetMap.get(stateSet);
  UINT                   byteIndex, stateSetCount;
  if(vp != nullptr) {
    byteIndex     = vp->m_arrayIndex;
    stateSetCount = vp->m_commentIndex;
    vp->addUsedByValue(ntIndex);
  } else {
    stateSetCount = m_stateBitSetMap.getCount();
    byteIndex     = stateSetCount * m_bitSetInterval.getSizeofBitSet();
    m_stateBitSetMap.put(stateSet, IndexMapValue(m_usedByParam, ntIndex, byteIndex));
  }
  const UINT             newState       = sps.getNewState();
  const String           macroValue     = encodeMacroValue(CompCodeBitSet, newState, byteIndex);
  const String           comment        = format(_T("Goto %4u on states in succStateBitSet[%u]"), newState, stateSetCount);
  return Macro(m_usedByParam, ntIndex, macroValue, comment);
}

// ------------------------------------ Print ------------------------------------------------

ByteCount CompressedTransSuccMatrix::print(MarginFile &output) const {
  m_byteCountMap.clear();
  printMacroesAndSuccessorCodeArray(output);
  printStatePairArrayTables(output);
  printStateBitSetTable( output);
  return m_byteCountMap.getSum();
}

void CompressedTransSuccMatrix::printMacroesAndSuccessorCodeArray(MarginFile &output) const {
  const UINT   macroCount = getMacroCount();
  const UINT   ntermCount = getNTermCount();
  Array<Macro> macroes(getMacroArray());
  if(macroCount > 0) {
    const UINT commentLen = getMaxCommentLength() + 1;
    macroes.sort(macroCmpByName);
    for(auto it = macroes.getIterator(); it.hasNext();) {
      it.next().print(output, commentLen);
    }
    output.printf(_T("\n"));
  }

  outputBeginArrayDefinition(output, _T("succCodeArray"), TYPE_UINT, macroCount);
  macroes.sort(macroCmpByIndex);
  TCHAR delim = ' ';
  UINT  count = 0;
  auto  it = macroes.getIterator();
  for(; it.hasNext() && (count < ntermCount); count++, delim = ',') {
    output.printf(_T("%c%s"), delim, it.next().getName().cstr());
    if((count % 10 == 9) && (count != ntermCount - 1)) {
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
  const ByteCount bc = outputEndArrayDefinition(output, TYPE_UINT, macroCount, true);
  m_byteCountMap.add(BC_SUCCESSORCODEARRAY, bc);
}

void CompressedTransSuccMatrix::printStatePairArrayTables(MarginFile &output) const {
  ByteCount byteCount;
  if(m_fromStateArraySize == 0) {
    output.printf(_T("#define succFromStateArrayTable nullptr\n"  ));
    output.printf(_T("#define succToStateArrayTable   nullptr\n\n"));
    m_byteCountMap.add(BC_STATEARRAYTABLE   , ByteCount());
    m_byteCountMap.add(BC_NEWSTATEARRAYTABLE, ByteCount());
    return;
  }

  const IntegerType                       stateType          = AllTemplateTypes(m_grammar).getStateType();

  { const StateSetIndexArray              stateArrayTable    = m_fromStateArrayMap.getEntryArray();
    UINT                                  tableSize          = 0;
    TCHAR                                 delim              = ' ';
    outputBeginArrayDefinition(output, _T("succFromStateArrayTable"   ), stateType, stateArrayTable.getElementCount(true));
    for(auto it = stateArrayTable.getIterator();   it.hasNext();) {
      const IndexArrayEntry<StateSet>    &e                  = it.next();
      String                              comment            = format(_T("%3u %s"), e.m_commentIndex, e.getUsedByComment().cstr());
      const UINT                          n                  = (UINT)e.m_key.size();
      UINT                                counter            = 0;
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
    const ByteCount bc = outputEndArrayDefinition(output, stateType, tableSize);
    m_byteCountMap.add(BC_STATEARRAYTABLE, bc);
  }

  { const StateArrayIndexArray            newStateArrayTable = m_newStateArrayMap.getEntryArray();
    UINT                                  tableSize          = 0;
    TCHAR                                 delim              = ' ';
    outputBeginArrayDefinition(output, _T("succToStateArrayTable") , stateType, newStateArrayTable.getElementCount(false));
    for(auto it = newStateArrayTable.getIterator(); it.hasNext();) {
      const IndexArrayEntry<StateArray>  &e                  = it.next();
      String                              comment            = format(_T("%3u %s"), e.m_commentIndex, e.getUsedByComment().cstr());
      const UINT                          n                  = (UINT)e.m_key.size();
      UINT                                counter            = 0;
      for(auto it1 = e.m_key.getIterator(); it1.hasNext(); counter++, delim = ',') {
        output.printf(_T("%c%4u"), delim, it1.next());
        if((counter % 20 == 19) && (counter != n - 1)) {
          newLine(output, comment, 108);
        }
      }
      newLine(output, comment, 108);
      tableSize += n;
    }
    const ByteCount bc = outputEndArrayDefinition(output, stateType, tableSize);
    m_byteCountMap.add(BC_NEWSTATEARRAYTABLE, bc);
  }
}

void CompressedTransSuccMatrix::printStateBitSetTable(MarginFile &output) const {
  if(m_stateBitSetMap.size() == 0) {
    output.printf(_T("#define succStateBitSetTable nullptr\n\n"));
    m_byteCountMap.add(BC_STATEBITSETTABLE, ByteCount());
  } else {
    const StateSetIndexArray bitSetArray = m_stateBitSetMap.getEntryArray();
    const UINT               bitSetCount = (UINT)m_stateBitSetMap.size();
    const BitSetInterval    &interval    = m_grammar.getSuccStateBitSetInterval();
    TCHAR                    delim       = ' ';
    const UINT               arraySize   = outputBeginBitSetTableDefinition(output, _T("succStateBitSetTable"), interval, bitSetCount);
    for(auto it = bitSetArray.getIterator(); it.hasNext();) {
      const IndexArrayEntry<StateSet>    &e                 = it.next();
      String                              comment           = format(_T("%3u %3u states %s"), e.m_commentIndex, (UINT)e.m_key.size(), e.getUsedByComment().cstr());
      const ByteArray                     ba                = bitSetToByteArray(e.m_key, interval);
      for(BYTE b : ba) {
        output.printf(_T("%c0x%02x"), delim, b);
        delim = ',';
      }
      newLine(output, comment);
    }
    const ByteCount bc = outputEndBitSetTableDefinition(output, arraySize);
    m_byteCountMap.add(BC_STATEBITSETTABLE, bc);
  }
}

}; // namespace TransposedSuccessorMatrixCompression

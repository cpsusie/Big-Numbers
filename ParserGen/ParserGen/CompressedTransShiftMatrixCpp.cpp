#include "stdafx.h"
#include "GrammarCode.h"
#include "TermActionPairArray.h"
#include "StatePairBitSet.h"
#include "TermSetReduction.h"
#include "CompressEncoding.h"
#include "StatePairMatrix.h"
#include "CompressedTransShiftMatrixCpp.h"

CompressedTransShiftMatrix::CompressedTransShiftMatrix(const Grammar &grammar)
  : m_grammar(                 grammar                                            )
  , m_grammarResult(           grammar.getResult()                                )
  , m_maxTermNameLength(       grammar.getMaxTermNameLength()                     )
  , m_transSuccMatrix(         grammar                                            )
  , m_shiftMacroMap(           grammar                                            )
  , m_shiftUsedByParam(        grammar.getBitSetParam(ETYPE_TERM)                 )
  , m_shiftStateBitSetInterval(grammar.getShiftStateBitSetInterval()              )
  , m_fromStateArraySize(      0                                                  )
  , m_newStateArraySize(       0                                                  )
  , m_shiftSplitNodeCount(     0                                                  )
  , m_shiftNodeArray(          TransposedShiftMatrix(grammar)                     )
  , m_reduceMacroMap(          grammar                                            )
  , m_reduceUsedByParam(       grammar.getBitSetParam(ETYPE_STATE)                )
  , m_sizeofTermBitSet(        getSizeofBitSet(grammar.getTermBitSetCapacity())   )
  , m_termArraySize(           0                                                  )
  , m_reduceArraySize(         0                                                  )
  , m_reduceSplitNodeCount(    0                                                  )
  , m_reduceNodeArray(         grammar, Options::getOptParam(OPTPARAM_REDUCE)     )
  , m_byteCountMap(            grammar                                            )
{
//  debugLog(_T("%s:m_shiftStateBitSetInterval:%s\n"), __TFUNCTION__, m_shiftStateBitSetInterval.toString().cstr());
//  debugLog(_T("%s:m_sizeofTermBitSet:%u\n"), __TFUNCTION__, m_sizeofTermBitSet);
  generateCompressedForm();
}

BitSet CompressedTransShiftMatrix::getStateBitSetTableUnion() const {
  StateSet result(getStateCount());
  for(auto it = m_stateBitSetMap.getIterator(); it.hasNext();) {
    result += it.next().getKey();
  }
  return result;
}

BitSet CompressedTransShiftMatrix::getTermBitSetTableUnion() const {
  TermSet result(getTermCount());
  for(auto it = m_termBitSetMap.getIterator(); it.hasNext();) {
    result += it.next().getKey();
  }
  result.add(0); // make sure, that term 0 remains term 0
  return result;
}

const OptimizedBitSetPermutation &CompressedTransShiftMatrix::getTermBitSetPermutation() const {
  if(m_termBitSetPermutation.isEmpty()) {
    findTermBitSetPermutation();
  }
  return m_termBitSetPermutation;
}

void CompressedTransShiftMatrix::findTermBitSetPermutation() const {
  m_termBitSetPermutation = OptimizedBitSetPermutation(getTermBitSetTableUnion());
}

const OptimizedBitSetPermutation2 &CompressedTransShiftMatrix::getStateBitSetPermutation() const {
  if(m_stateBitSetPermutation.isEmpty()) {
    findStateBitSetPermutation();
  }
  return m_stateBitSetPermutation;
}

void CompressedTransShiftMatrix::findStateBitSetPermutation() const {
  m_stateBitSetPermutation = OptimizedBitSetPermutation2(getStateBitSetTableUnion(), m_transSuccMatrix.getBitSetsTableUnion());
}

ByteCount CompressedTransShiftMatrix::getSavedBytesByOptimizedStateBitSets() const {
  return getStateBitSetPermutation().getSavedBytesByOptimizedBitSets(getStateBitSetCount(), m_transSuccMatrix.getStateBitSetCount());
}

void CompressedTransShiftMatrix::generateCompressedForm() {
  generateCompressedFormShift();
  generateCompressedFormReduce();
}

// ------------------------------------ Shift -----------------------------------------------

void CompressedTransShiftMatrix::generateCompressedFormShift() {
  m_fromStateArraySize   = 0;
  m_newStateArraySize    = 0;
  m_shiftSplitNodeCount  = 0;
  CompactUIntArray undefinedTermEntries;
  const UINT       termCount = getTermCount();
  for(UINT term = 0; term < termCount; term++) {
    const SymbolNode *node = m_shiftNodeArray[term];
    if(node == nullptr) {
      undefinedTermEntries.add(term);
    } else {
      Macro macro = doShiftNode(*node);
      const String comment = macro.getComment();
      m_shiftMacroMap.addMacro(macro.setComment(format(_T("%-*s %s"), m_maxTermNameLength, m_grammar.getSymbolName(term).cstr(), comment.cstr()))
                     .setIndex(term)
                     .setName(format(_T("_tc%04u"), term))
              );
    }
  }
  for(UINT term : undefinedTermEntries) {
    m_shiftMacroMap.addMacro(Macro(m_shiftUsedByParam, term, _T("0x00000000"), _T("")).setIndex(term).setName(format(_T("_tc%04u"), term)));
  }
  m_byteCountMap.m_splitNodeCount = m_shiftSplitNodeCount;
}

Macro CompressedTransShiftMatrix::doShiftNode(const SymbolNode &node) {
  switch(node.getCompressionMethod()) {
  case AbstractParserTables::CompCodeBinSearch: return doShiftNodeBinSearch( node);
  case AbstractParserTables::CompCodeSplitNode: return doShiftNodeSplit(     node);
  case AbstractParserTables::CompCodeImmediate: return doShiftNodeImmediate( node);
  case AbstractParserTables::CompCodeBitSet   : return doShiftNodeBitSet(    node);
  default                                     :
    throwException(_T("%s:Unknown compressionMethod for state %u"), __TFUNCTION__, node.getTerm());
    break;
  }
  return Macro(m_shiftUsedByParam, 0, EMPTYSTRING, EMPTYSTRING); // should not come here
}

Macro CompressedTransShiftMatrix::doShiftNodeBinSearch(const SymbolNode &node) {
  const UINT            term           = node.getTerm();
  const StatePairArray &statePairArray = node.getStatePairArray();
  const NTIndexSet      fromStateSet   = statePairArray.getFromStateSet(getStateCount());
  IndexMapValue        *imvp           = m_fromStateArrayMap.get(fromStateSet);
  UINT                  fromStateArrayIndex, fromStateArrayCount;

  if(imvp != nullptr) {
    fromStateArrayIndex = imvp->m_arrayIndex;
    fromStateArrayCount = imvp->m_commentIndex;
    imvp->addUsedByValue(term);
  } else {
    fromStateArrayIndex = m_fromStateArraySize;
    fromStateArrayCount = m_fromStateArrayMap.getCount();
    m_fromStateArrayMap.put(fromStateSet, IndexMapValue(m_shiftUsedByParam, term, fromStateArrayIndex));
    m_fromStateArraySize += (UINT)fromStateSet.size() + 1;
  }

  const StateArray       newStateList   = statePairArray.getNewStateArray();
  imvp = m_newStateArrayMap.get(newStateList);
  UINT                  newStateListIndex, newStateArrayCount;
  if(imvp != nullptr) {
    newStateListIndex  = imvp->m_arrayIndex;
    newStateArrayCount = imvp->m_commentIndex;
    imvp->addUsedByValue(term);
  } else {
    newStateListIndex  = m_newStateArraySize;
    newStateArrayCount = m_newStateArrayMap.getCount();
    m_newStateArrayMap.put(newStateList, IndexMapValue(m_shiftUsedByParam, term, newStateListIndex));
    m_newStateArraySize += (UINT)newStateList.size();
  }
  const String           macroValue     = encodeMacroValue(CompCodeBinSearch, newStateListIndex, fromStateArrayIndex);
  const String           comment        = format(_T("shiftFromStateArray %4u, shiftToStateArrayTable %4u"), fromStateArrayCount , newStateArrayCount );
  return Macro(m_shiftUsedByParam, term, macroValue, comment);
}

Macro CompressedTransShiftMatrix::doShiftNodeSplit(const SymbolNode &node) {
  const UINT             term           = node.getTerm();
  const SymbolNode      &left           = node.getChild(0);
  const SymbolNode      &right          = node.getChild(1);

  UINT                   indexL, indexR;
  Macro                  macroL         = doShiftNode(left);
  const Macro           *mpL            = m_shiftMacroMap.findMacroByValue(macroL.getValue());
  if(mpL != nullptr) {
    mpL->addUsedByValue(term);
    indexL = mpL->getIndex();
  } else {
    const String         name           = format(_T("_ts%04u"), m_shiftSplitNodeCount);
    indexL = m_shiftMacroMap.getTermCount() + m_shiftSplitNodeCount++;
    m_shiftMacroMap.addMacro(macroL.setIndex(indexL).setName(name));
    mpL = &macroL;
  }

  Macro                  macroR         = doShiftNode(right);
  const Macro           *mpR            = m_shiftMacroMap.findMacroByValue(macroR.getValue());
  if(mpR != nullptr) {
    mpR->addUsedByValue(term);
    indexR = mpR->getIndex();
  } else {
    const String         name           = format(_T("_ts%04u"), m_shiftSplitNodeCount);
    indexR = m_shiftMacroMap.getTermCount() + m_shiftSplitNodeCount++;
    m_shiftMacroMap.addMacro(macroR.setIndex(indexR).setName(name));
    mpR = &macroR;
  }

  const String           macroValue     = encodeMacroValue(CompCodeSplitNode, indexL, indexR);
  const String           comment        = format(_T("Split(%s,%s)"), mpL->getName().cstr(), mpR->getName().cstr());
  return Macro(m_shiftUsedByParam, term, macroValue, comment);
}

Macro CompressedTransShiftMatrix::doShiftNodeImmediate(const SymbolNode &node) {
  const UINT             term           = node.getTerm();
  const StatePair        sp             = node.getStatePair();
  const UINT             newState       = sp.m_newState;
  const UINT             fromState      = sp.m_fromState;
  const UINT             fromStateCount = node.getFromStateCount();
  const String           macroValue     = encodeMacroValue(CompCodeImmediate, newState, fromState);
  const String           comment        = format(_T("Shift to %4u from %4u"), newState, fromState);
  return Macro(m_shiftUsedByParam, term, macroValue, comment);
}

Macro CompressedTransShiftMatrix::doShiftNodeBitSet(const SymbolNode &node) {
  const UINT             term           = node.getTerm();
  const StatePairBitSet &sps            = node.getStatePairBitSet();
  const StateSet        &stateSet       = sps.getFromStateSet();
  IndexMapValue         *vp             = m_stateBitSetMap.get(stateSet);
  UINT                   byteIndex, stateSetCount;
  if(vp != nullptr) {
    byteIndex     = vp->m_arrayIndex;
    stateSetCount = vp->m_commentIndex;
    vp->addUsedByValue(term);
  } else {
    stateSetCount = m_stateBitSetMap.getCount();
    byteIndex     = stateSetCount * m_shiftStateBitSetInterval.getSizeofBitSet();
    m_stateBitSetMap.put(stateSet, IndexMapValue(m_shiftUsedByParam, term, byteIndex));
  }
  const UINT             newState       = sps.getNewState();
  const String           macroValue     = encodeMacroValue(CompCodeBitSet, newState, byteIndex);
  const String           comment        = format(_T("Shift to %4u on states in shiftStateBitSet[%u]"), newState, stateSetCount);
  return Macro(m_shiftUsedByParam, term, macroValue, comment);
}

// ------------------------------------ Reduce -----------------------------------------------

void CompressedTransShiftMatrix::generateCompressedFormReduce() {
  m_termArraySize        = 0;
  m_reduceArraySize      = 0;
  m_reduceSplitNodeCount = 0;
  CompactUIntArray undefinedTermEntries;
  for(UINT state = 0; state < getStateCount(); state++) {
    const ReduceNode *node  = m_reduceNodeArray[state];
    if(node == nullptr) {
      undefinedTermEntries.add(state);
    } else {
      Macro             macro = doReduceNode(*node);
      m_reduceMacroMap.addMacro(macro.setIndex(state).setName(format(_T("_rc%04u"), state)));
    }
  }
  const String errorValue = encodeMacroValue(CompCodeImmediate, 1, AbstractParserTables::_NoFromStateCheck);
  for(UINT state : undefinedTermEntries) {
    m_reduceMacroMap.addMacro(Macro(m_reduceUsedByParam, state, errorValue, _T("")).setIndex(state).setName(format(_T("_rc%04u"), state)));
  }
  m_byteCountMap.m_splitNodeCount += m_reduceSplitNodeCount;
}

Macro CompressedTransShiftMatrix::doReduceNode(const ReduceNode &node) {
  switch(node.getCompressionMethod()) {
  case AbstractParserTables::CompCodeBinSearch: return doReduceNodeBinSearch(node);
  case AbstractParserTables::CompCodeSplitNode: return doReduceNodeSplit(    node);
  case AbstractParserTables::CompCodeImmediate: return doReduceNodeImmediate(node);
  case AbstractParserTables::CompCodeBitSet   : return doReduceNodeBitSet(   node);
  default                                     :
    throwException(_T("%s:Unknown compressionMethod for state %u"), __TFUNCTION__, node.getState());
    break;
  }
  return Macro(m_reduceUsedByParam, 0, EMPTYSTRING, EMPTYSTRING); // should not come here
}

Macro CompressedTransShiftMatrix::doReduceNodeBinSearch(const ReduceNode &node) {
  const UINT                 state           = node.getState();
  const TermActionPairArray &termActionArray = node.getTermActionPairArray();
  const TermSet              termSet         = termActionArray.getLegalTermSet(getTermCount());
  IndexMapValue             *imvp            = m_termArrayMap.get(termSet);
  UINT                       termArrayIndex, termArrayCount;

  if(imvp != nullptr) {
    termArrayIndex = imvp->m_arrayIndex;
    termArrayCount = imvp->m_commentIndex;
    imvp->addUsedByValue(state);
  } else {
    termArrayIndex = m_termArraySize;
    termArrayCount = m_termArrayMap.getCount();
    m_termArrayMap.put(termSet, IndexMapValue(m_reduceUsedByParam, state, termArrayIndex));
    m_termArraySize += (UINT)termSet.size() + 1;
  }

  const ActionArray reduceProdList = termActionArray.getActionArray();
  imvp = m_reduceArrayMap.get(reduceProdList);
  UINT              reduceArrayIndex, reduceArrayCount;
  if(imvp != nullptr) {
    reduceArrayIndex = imvp->m_arrayIndex;
    reduceArrayCount = imvp->m_commentIndex;
    imvp->addUsedByValue(state);
  } else {
    reduceArrayIndex = m_reduceArraySize;
    reduceArrayCount = m_reduceArrayMap.getCount();
    m_reduceArrayMap.put(reduceProdList, IndexMapValue(m_reduceUsedByParam, state, reduceArrayIndex));
    m_reduceArraySize += (UINT)reduceProdList.size();
  }
  const String            macroValue = encodeMacroValue(CompCodeBinSearch, reduceArrayIndex, termArrayIndex);
  const String            comment    = format(_T("termArray %4u, reduceArray %4u"), termArrayCount, reduceArrayCount);
  return Macro(m_reduceUsedByParam, state, macroValue, comment);
}

Macro CompressedTransShiftMatrix::doReduceNodeSplit(const ReduceNode &node) {
  const UINT              state      = node.getState();
  const ReduceNode       &left       = node.getChild(0);
  const ReduceNode       &right      = node.getChild(1);

  UINT                    indexL, indexR;
  Macro                   macroL     = doReduceNode(left);
  const Macro            *mpL        = m_reduceMacroMap.findMacroByValue(macroL.getValue());
  if(mpL != nullptr) {
    mpL->addUsedByValue(state);
    indexL = mpL->getIndex();
  } else {
    const String          name       = format(_T("_rs%04u"), m_reduceSplitNodeCount);
    indexL = getStateCount() + m_reduceSplitNodeCount++;
    m_reduceMacroMap.addMacro(macroL.setIndex(indexL).setName(name));
    mpL = &macroL;
  }

  Macro                   macroR     = doReduceNode(right);
  const Macro            *mpR        = m_reduceMacroMap.findMacroByValue(macroR.getValue());
  if(mpR != nullptr) {
    mpR->addUsedByValue(state);
    indexR = mpR->getIndex();
  } else {
    const String          name       = format(_T("_rs%04u"), m_reduceSplitNodeCount);
    indexR = getStateCount() + m_reduceSplitNodeCount++;
    m_reduceMacroMap.addMacro(macroR.setIndex(indexR).setName(name));
    mpR = &macroR;
  }

  const String            macroValue = encodeMacroValue(CompCodeSplitNode, indexL, indexR);
  const String            comment    = format(_T("Split(%s,%s)"), mpL->getName().cstr(), mpR->getName().cstr());
  return Macro(m_reduceUsedByParam, state, macroValue, comment);
}

Macro CompressedTransShiftMatrix::doReduceNodeImmediate(const ReduceNode &node) {
  const UINT              state      = node.getState();
  const TermActionPair    tap        = node.getTermActionPair();
  assert(tap.isReduceAction());
  const UINT              term       = tap.getTerm();
  const UINT              prod       = tap.getReduceProduction();
  const String            macroValue = encodeMacroValue(CompCodeImmediate, prod, term);
  const String            comment    = tap.isAcceptAction()
                                     ? format(_T("Reduce by %4u (Accept) on %s"), prod, m_grammar.getSymbolName(term).cstr())
                                     : format(_T("Reduce by %4u on %s")         , prod, m_grammar.getSymbolName(term).cstr())
                                     ;
  return Macro(m_reduceUsedByParam, state, macroValue, comment);
}

Macro CompressedTransShiftMatrix::doReduceNodeBitSet(const ReduceNode &node) {
  const UINT              state      = node.getState();
  const TermSetReduction &tsr        = node.getTermSetReduction();
  const TermSet          &termSet    = tsr.getTermSet();
  IndexMapValue          *vp         = m_termBitSetMap.get(termSet);
  UINT                    byteIndex, termSetCount;
  if(vp != nullptr) {
    termSetCount = vp->m_commentIndex;
    byteIndex    = vp->m_arrayIndex;
    vp->addUsedByValue(state);
  } else {
    termSetCount = m_termBitSetMap.getCount();

//  debugLog(_T("%s:m_sizeofTermBitSet:%u\n"), __TFUNCTION__, m_sizeofTermBitSet);

    byteIndex    = termSetCount * m_sizeofTermBitSet;
    m_termBitSetMap.put(termSet, IndexMapValue(m_reduceUsedByParam, state, byteIndex));
  }
  const UINT              prod       = tsr.getProduction();
  const String            macroValue = encodeMacroValue(CompCodeBitSet, prod, byteIndex);
  const String            comment    = format(_T("Reduce by %4u on tokens in termBitSet[%u]"), prod, termSetCount);
  return Macro(m_reduceUsedByParam, state, macroValue, comment);
}

// ------------------------------------ Print ------------------------------------------------

TableTypeByteCountMap CompressedTransShiftMatrix::findTablesByteCount(const Grammar &grammar) {
  Grammar g = grammar;
  CompressedTransShiftMatrix sm(g);
  const ByteCount savedStateBytes = sm.getSavedBytesByOptimizedStateBitSets();
  const ByteCount savedTermBytes  = sm.getSavedBytesByOptimizedTermBitSets();
  if(savedStateBytes.getByteCount(PLATFORM_X64) > 2) {
    g.reorderStates(sm.getStateBitSetPermutation());
  }
  if(savedTermBytes.getByteCount(PLATFORM_X64) > 2) {
    g.reorderTerminals(sm.getTermBitSetPermutation());
  }
  CompressedTransShiftMatrix sm1(g);
  std::wostringstream s;
  sm1.print(MarginFile(s));
  return sm1.m_byteCountMap;
}

ByteCount CompressedTransShiftMatrix::print(MarginFile &output) const {
  m_byteCountMap.clear();
  printShift( output);
  printReduce(output);
  printSucc(  output);
  return m_byteCountMap.getSum();
}

void CompressedTransShiftMatrix::printShift(MarginFile &output) const {
  printMacroesAndShiftCodeArray(output);
  printStatePairArrayTables(    output);
  printStateBitSetTable(        output);
}

void CompressedTransShiftMatrix::printMacroesAndShiftCodeArray(MarginFile &output) const {
  const UINT   macroCount = m_shiftMacroMap.getMacroCount();
  const UINT   termCount  = m_shiftMacroMap.getTermCount();
  Array<Macro> macroes(m_shiftMacroMap.getMacroArray());
  if(macroCount > 0) {
    const UINT commentLen = m_shiftMacroMap.getMaxCommentLength() + 1;
    macroes.sort(macroCmpByName);
    for(auto it = macroes.getIterator(); it.hasNext();) {
      it.next().print(output, commentLen, &m_grammar);
    }
    output.printf(_T("\n"));
  }

  outputBeginArrayDefinition(output, _T("shiftCodeArray"), TYPE_UINT, macroCount);
  macroes.sort(macroCmpByIndex);
  TCHAR delim = ' ';
  UINT  count = 0;
  auto  it = macroes.getIterator();
  for(; it.hasNext() && (count < termCount); count++, delim = ',') {
    output.printf(_T("%c%s"), delim, it.next().getName().cstr());
    if((count % 10 == 9) && (count != termCount - 1)) {
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
  m_byteCountMap.add(BC_SHIFTCODEARRAY, bc);
}

void CompressedTransShiftMatrix::printStatePairArrayTables(MarginFile &output) const {
  ByteCount byteCount;
  if(m_fromStateArraySize == 0) {
    output.printf(_T("#define shiftFromStateArrayTable nullptr\n"  ));
    output.printf(_T("#define shiftToStateArrayTable   nullptr\n\n"));
    m_byteCountMap.add(BC_STATEARRAYTABLE   , ByteCount());
    m_byteCountMap.add(BC_NEWSTATEARRAYTABLE, ByteCount());
    return;
  }

  const IntegerType                       stateType          = AllTemplateTypes(m_grammar).getStateType();

  { const StateSetIndexArray              stateArrayTable    = m_fromStateArrayMap.getEntryArray();
    UINT                                  tableSize          = 0;
    TCHAR                                 delim              = ' ';
    outputBeginArrayDefinition(output, _T("shiftFromStateArrayTable"   ), stateType, stateArrayTable.getElementCount(true));
    for(auto it = stateArrayTable.getIterator();   it.hasNext();) {
      const IndexArrayEntry<StateSet>    &e                  = it.next();
      String                              comment            = format(_T("%3u %s"), e.m_commentIndex, e.getUsedByComment(&m_grammar).cstr());
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
    outputBeginArrayDefinition(output, _T("shiftToStateArrayTable") , stateType, newStateArrayTable.getElementCount(false));
    for(auto it = newStateArrayTable.getIterator(); it.hasNext();) {
      const IndexArrayEntry<StateArray>  &e                  = it.next();
      String                              comment            = format(_T("%3u %s"), e.m_commentIndex, e.getUsedByComment(&m_grammar).cstr());
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

void CompressedTransShiftMatrix::printStateBitSetTable(MarginFile &output) const {
  if(m_stateBitSetMap.size() == 0) {
    output.printf(_T("#define shiftStateBitSetTable nullptr\n\n"));
    m_byteCountMap.add(BC_STATEBITSETTABLE, ByteCount());
  } else {
    const StateSetIndexArray bitSetArray = m_stateBitSetMap.getEntryArray();
    const UINT               bitSetCount = (UINT)m_stateBitSetMap.size();
    const BitSetInterval    &interval    = m_grammar.getShiftStateBitSetInterval();
    TCHAR                    delim       = ' ';
    const UINT               arraySize   = outputBeginBitSetTableDefinition(output, _T("shiftStateBitSetTable"), interval, bitSetCount);
    for(auto it = bitSetArray.getIterator(); it.hasNext();) {
      const IndexArrayEntry<StateSet>    &e                 = it.next();
      String                              comment           = format(_T("%3u %3u states %s"), e.m_commentIndex, (UINT)e.m_key.size(), e.getUsedByComment(&m_grammar).cstr());
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

void CompressedTransShiftMatrix::printReduce(MarginFile &output) const {
  printMacroesAndReduceCodeArray(output);
  printTermAndReduceArrayTable(  output);
  printTermBitSetTable(          output);
}

void CompressedTransShiftMatrix::printMacroesAndReduceCodeArray(MarginFile &output) const {
  const UINT   macroCount = m_reduceMacroMap.getMacroCount();
  const UINT   stateCount = getStateCount();
  Array<Macro> macroes(m_reduceMacroMap.getMacroArray());
  if(macroCount > 0) {
    const UINT commentLen = m_reduceMacroMap.getMaxCommentLength() + 1;
    macroes.sort(macroCmpByName);
    for(auto it = macroes.getIterator(); it.hasNext();) {
      it.next().print(output, commentLen);
    }
    output.printf(_T("\n"));
  }

  outputBeginArrayDefinition(output, _T("reduceCodeArray"), TYPE_UINT, macroCount);
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
  const ByteCount bc = outputEndArrayDefinition(output, TYPE_UINT, macroCount, true);
  m_byteCountMap.add(BC_REDUCECODEARRAY, bc);
}

void CompressedTransShiftMatrix::printTermAndReduceArrayTable(MarginFile &output) const {
  if(m_termArraySize == 0) {
    output.printf(_T("#define termArrayTable   nullptr\n"  ));
    output.printf(_T("#define reduceArrayTable nullptr\n\n"));
    m_byteCountMap.add(BC_TERMARRAYTABLE  , ByteCount());
    m_byteCountMap.add(BC_REDUCEARRAYTABLE, ByteCount());
    return;
  }

  const AllTemplateTypes types(m_grammar);

  { const TermSetIndexArray               termArrayTable    = m_termArrayMap.getEntryArray();
    UINT                                  tableSize         = 0;
    TCHAR                                 delim             = ' ';
    outputBeginArrayDefinition(output, _T("termArrayTable"   ), types.getTermType(), termArrayTable.getElementCount(true));
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
    const ByteCount bc = outputEndArrayDefinition(output, types.getTermType(), tableSize);
    m_byteCountMap.add(BC_TERMARRAYTABLE, bc);
  }
  { const ActionArrayIndexArray           reduceArrayTable  = m_reduceArrayMap.getEntryArray();
    UINT                                  tableSize         = 0;
    TCHAR                                 delim             = ' ';
    outputBeginArrayDefinition(output, _T("reduceArrayTable") , types.getActionType(), reduceArrayTable.getElementCount(false));
    for(auto it = reduceArrayTable.getIterator(); it.hasNext();) {
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
    const ByteCount bc = outputEndArrayDefinition(output, types.getActionType(), tableSize);
    m_byteCountMap.add(BC_REDUCEARRAYTABLE, bc);
  }
}

void CompressedTransShiftMatrix::printTermBitSetTable(MarginFile &output) const {
  if(m_termBitSetMap.size() == 0) {
    output.printf(_T("#define termBitSetTable nullptr\n\n"));
    m_byteCountMap.add(BC_TERMBITSETTABLE, ByteCount());
  } else {
    const TermSetIndexArray               bitSetArray = m_termBitSetMap.getEntryArray();
    const UINT                            bitSetCount = (UINT)m_termBitSetMap.size();
    const UINT                            capacity    = m_grammar.getTermBitSetCapacity();
    const BitSetInterval                  interval(0, capacity);
    TCHAR                                 delim       = ' ';
    const UINT                            arraySize   = outputBeginBitSetTableDefinition(output, _T("termBitSetTable"), interval, bitSetCount);
    for(auto it = bitSetArray.getIterator(); it.hasNext();) {
      const IndexArrayEntry<TermSet>     &e                 = it.next();
      String                              comment           = format(_T("%3u %3u tokens %s"), e.m_commentIndex, (UINT)e.m_key.size(), e.getUsedByComment().cstr());
      const ByteArray                     ba                = bitSetToByteArray(e.m_key, capacity);
      for(BYTE b : ba) {
        output.printf(_T("%c0x%02x"), delim, b);
        delim = ',';
      }
      newLine(output, comment);
    }
    ByteCount bc = outputEndBitSetTableDefinition(output, arraySize);
    m_byteCountMap.add(BC_TERMBITSETTABLE, bc);
  }
}

void CompressedTransShiftMatrix::printSucc(MarginFile &output) const {
  m_transSuccMatrix.print(output);
  m_byteCountMap += m_transSuccMatrix.getByteCountMap();
}

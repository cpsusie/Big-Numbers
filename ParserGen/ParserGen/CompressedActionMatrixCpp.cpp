#include "stdafx.h"
#include <CompactHashMap.h>
#include "GrammarCode.h"
#include "CompressedActionMatrixCpp.h"

namespace ActionMatrix {

int rawActionArrayCmp(const RawActionArray &a1, const RawActionArray &a2) {
  const size_t n = a1.size();
  int          c = sizetHashCmp(n, a2.size());
  if(c) return c;
  return n ? memcmp(a1.begin(), a2.begin(), n * sizeof(short)) : 0;
}

CompressedActionMatrix::CompressedActionMatrix(const GrammarTables &tables)
  : m_tables(           tables                   )
  , m_stateCount(       tables.getStateCount()   )
  , m_terminalCount(    tables.getTerminalCount())
  , m_laSetSizeInBytes((tables.getTerminalCount() - 1) / 8 + 1)
  , m_terminalType(     tables.getTerminalType() )
  , m_actionType(       tables.getActionType()   )
{
  m_stateInfoArray.setCapacity(getStateCount());

  const Array<ActionArray> &stateActions = m_tables.getStateActions();
  for(UINT state = 0; state < getStateCount(); state++) {
    m_stateInfoArray.add(StateActionInfo(getTerminalCount(), state, stateActions[state]));
  }
  generateCompressedForm();
}

void CompressedActionMatrix::generateCompressedForm() {
  m_currentTermListSize   = 0;
  m_currentActionListSize = 0;
  m_currentLASetArraySize = 0;
  m_currentSplitNodeCount = 0;
  ByteCount totalByteCount;

  for(UINT state = 0; state < getStateCount(); state++) {
    const StateActionInfo &stateInfo = m_stateInfoArray[state];
    Macro                  macro     = doStateActionInfo(stateInfo);
    addMacro(macro.setIndex(state).setName(format(_T("_ac%04u"), state)));
  }
}

Macro CompressedActionMatrix::doStateActionInfo(const StateActionInfo &stateInfo) {
  switch(stateInfo.getCompressionMethod()) {
  case UNCOMPRESSED                : return doUncompressedState(    stateInfo);
  case SPLITCOMPRESSION            : return doSplitCompression(     stateInfo);
  case ONEITEMCOMPRESSION          : return doOneItemState(         stateInfo);
  case REDUCEBYSAMEPRODCOMPRESSION : return doReduceBySameProdState(stateInfo);
  default                         :
    throwException(_T("%s:Unknown compressionMethod for state %u"), __TFUNCTION__, stateInfo.getState());
    break;
  }
  return Macro(getStateCount(), 0, EMPTYSTRING, EMPTYSTRING); // should not come here
}

const Macro *CompressedActionMatrix::findMacroByValue(const String &macroValue) const {
  const UINT *indexp = m_macroMap.get(macroValue);
  return indexp ? &m_macroArray[*indexp] : NULL;
}

void CompressedActionMatrix::addMacro(const Macro &m) {
  assert(m.getIndex() >= 0);
  const UINT index = (UINT)m_macroArray.size();
  m_macroArray.add(m);
  m_macroMap.put(m.getValue(), index);
}

Macro CompressedActionMatrix::doUncompressedState(const StateActionInfo &stateInfo) {
  const UINT        state   = stateInfo.getState();
  const ActionArray aa      = stateInfo.getActionArray();
  const SymbolSet   laSet   = aa.getLookaheadSet(getTerminalCount());
  IndexMapValue    *imvp    = m_termListMap.get(laSet);
  UINT              termListIndex, laCount;

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

  const RawActionArray raa = aa.getRawActionArray();
  imvp = m_raaMap.get(raa);
  UINT                 actionListIndex, raCount;
  if(imvp != nullptr) {
    actionListIndex = imvp->m_arrayIndex;
    raCount         = imvp->m_commentIndex;
    imvp->addState(state);
  } else {
    actionListIndex = m_currentActionListSize;
    raCount         = m_raaMap.getCount();
    IndexMapValue nv(getStateCount(), state, actionListIndex);
    m_raaMap.put(raa, nv);
    m_currentActionListSize += (UINT)raa.size();
  }
  const String comment      = format(_T("termList %3u, actionList %3u"), laCount, raCount);
  const String macroValue   = encodeMacroValue(UNCOMPRESSED, actionListIndex, termListIndex);
  return Macro(getStateCount(), state, macroValue, comment);
}

Macro CompressedActionMatrix::doSplitCompression(const StateActionInfo &stateInfo) {
  const UINT                   state         = stateInfo.getState();
  const ActionArray           &shiftActions  = stateInfo.getShiftActionArray();
  const SameReduceActionArray &reduceActions = stateInfo.getReduceActionArray();
  const BYTE                   code          = ((BYTE)reduceActions.size() * 3) + (BYTE)shiftActions.size();
  ActionArray a1(1), a2(1);
  switch(code) {
  case 2 : // 2 shift actions
    a1.add(shiftActions[0]);
    a2.add(shiftActions[1]);
    break;
  case 4 : // 1 shift action, 1 reduce action
    a1.add(shiftActions[0]);
    a2.addAll((ActionArray)reduceActions[0]);
    break;
  case 6 : // 0 shift actions, 2 reduce actions
    a1.addAll((ActionArray)reduceActions[0]);
    a2.addAll((ActionArray)reduceActions[1]);
    break;

  case 0 : // 0 actions                        -> error
  case 1 : // 1 shift action                   -> error
  case 3 : // 1 reduce action                  -> error
  case 5 : // 2 shift actions, 1 reduce action -> error
  default:
    throwInvalidArgumentException(__TFUNCTION__,_T("stateInfo(shifts:%u, reductions:%u")
                                               ,(UINT)shiftActions.size()
                                               ,(UINT)reduceActions.size()
                                 );
  }

  int arrayIndex1, arrayIndex2;
  Macro        macro1 = doStateActionInfo(StateActionInfo(getTerminalCount(), state, a1));
  const Macro *mp1    = findMacroByValue(macro1.getValue());
  if(mp1 != nullptr) {
    mp1->addState(state);
    arrayIndex1 = mp1->getIndex();
  } else {
    const String name = format(_T("_sn%04u"), m_currentSplitNodeCount);
    arrayIndex1 = getStateCount() + m_currentSplitNodeCount++;
    addMacro(macro1.setIndex(arrayIndex1).setName(name));
    mp1 = &macro1;
  }

  Macro        macro2 = doStateActionInfo(StateActionInfo(getTerminalCount(), state, a2));
  const Macro *mp2    = findMacroByValue(macro2.getValue());
  if(mp2 != nullptr) {
    mp2->addState(state);
    arrayIndex2 = mp2->getIndex();
  } else {
    const String  name = format(_T("_sn%04u"), m_currentSplitNodeCount);
    arrayIndex2 = getStateCount() + m_currentSplitNodeCount++;
    addMacro(macro2.setIndex(arrayIndex2).setName(name));
    mp2 = &macro2;
  }

  const String comment    = format(_T("Split(%s,%s)"), mp1->getName().cstr(), mp2->getName().cstr());
  const String macroValue = encodeMacroValue(SPLITCOMPRESSION, arrayIndex1, arrayIndex2);
  return Macro(getStateCount(), state, macroValue, comment);
}

Macro CompressedActionMatrix::doOneItemState(const StateActionInfo &stateInfo) {
  const UINT         state = stateInfo.getState();
  const ActionArray  paa   = stateInfo.getActionArray();

  assert(paa.size() == 1);

  const ParserAction &pa = paa[0];
  const int           action  = pa.m_action;                    // positive or negative
  const UINT          token   = pa.m_token;
  const String        comment = (action <= 0)
                              ? format(_T("Reduce by %u on %s"), -action, getSymbolName(pa.m_token))
                              : format(_T("Shift  to %u on %s"),  action, getSymbolName(pa.m_token));
  const String        macroValue   = encodeMacroValue(ONEITEMCOMPRESSION, action, token);
  return Macro(getStateCount(), state, macroValue, comment);
}

Macro CompressedActionMatrix::doReduceBySameProdState(const StateActionInfo &stateInfo) {
  const UINT                   state = stateInfo.getState();
  const SameReduceActionArray &raa   = stateInfo.getReduceActionArray();

  assert((raa.size() == 1) && stateInfo.getShiftActionArray().isEmpty());

  const SymbolSet &laSet = raa[0].getTerminalSet();
  IndexMapValue   *vp    = m_laSetMap.get(laSet);
  UINT             byteIndex, termSetCount;
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
  const int           prod         = raa[0].getProduction();
  const int           action       = -prod;
  const String        comment      = format(_T("Reduce by %u on tokens in termSet[%u]"), prod, termSetCount);
  const String        macroValue   = encodeMacroValue(REDUCEBYSAMEPRODCOMPRESSION, action, byteIndex);
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
  const UINT macroCount = (UINT)m_macroArray.size();
  Array<Macro> macroes(m_macroArray);
  if(macroCount > 0) {
    macroes.sort(macroCmpByName);
    for(auto it = macroes.getIterator(); it.hasNext();) {
      const Macro &m = it.next();
      output.printf(_T("#define %s %-10s /* %-40s*/\n"), m.getName().cstr(), m.getValue().cstr(), m.getComment().cstr());
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
    for(ConstIterator<IndexArrayEntry<SymbolSet>> it = termListArray.getIterator(); it.hasNext();) {
      const IndexArrayEntry<SymbolSet> &e       = it.next();
      String                            comment = format(_T(" %3u %s"), e.m_commentIndex, e.getComment().cstr());
      const UINT                        n       = (UINT)e.m_key.size();
      UINT                              counter = 0;
      output.setLeftMargin(2);
      output.printf(_T("%c%3u"), delim, n); delim = ',';
      output.setLeftMargin(6);
      for(ConstIterator<size_t> it1 = e.m_key.getIterator(); it1.hasNext(); counter++) {
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
    for(ConstIterator<IndexArrayEntry<RawActionArray>> it = raaArray.getIterator(); it.hasNext();) {
      const IndexArrayEntry<RawActionArray> &e       = it.next();
      String                                 comment = format(_T("%3u %s"), e.m_commentIndex, e.getComment().cstr());
      const UINT                             n       = (UINT)e.m_key.size();
      UINT                                   counter = 0;
      for(ConstIterator<short> it1 = e.m_key.getIterator(); it1.hasNext(); counter++, delim = ',') {
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
    for(ConstIterator<IndexArrayEntry<SymbolSet> > it = laSetArray.getIterator(); it.hasNext();) {
      const IndexArrayEntry<SymbolSet> &e = it.next();
      const ByteArray                   ba = symbolSetToByteArray(e.m_key);
      const UINT                        n = (UINT)ba.size();
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

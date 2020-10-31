#include "stdafx.h"
#include <CompactHashMap.h>
#include "GrammarCode.h"
#include "CompressedActionMatrixCpp.h"

void checkMax15Bits(const TCHAR *method, int line, int v, const TCHAR *varName) {
  if((v & 0xffff8000) != 0) {
    throwException(_T("%s:(%d):value %s=(%08x) cannot be contained in 15 bits"), method, line, varName, v);
  }
}

#if defined(_DEBUG)
void checkCodeBits(const TCHAR *method, UINT v, const TCHAR *varName) {
  if((v & encodeCompressMethod(MAXCOMPRESSIONVALUE)) != 0) {
    MessageBox(NULL, format(_T("%s:Encoded variable %s has value %08x, with non-zero value in encoding bits")
              ,method, varName, v).cstr()
              ,_T("Error")
              ,MB_ICONSTOP
              );
    exit(-1);
  }
}
#endif // _DEBUG

namespace ActionMatrix {

int rawActionArrayCmp(const RawActionArray &a1, const RawActionArray &a2) {
  const size_t n = a1.size();
  int          c = sizetHashCmp(n, a2.size());
  if(c) return c;
  for(const short *v1 = &a1.first(), *v2 = &a2.first(), *end = v1+n; v1 < end;) {
    c = shortHashCmp(*(v1++), *(v2++));
    if(c) return c;
  }
  return 0;
}

StateActionInfo::StateActionInfo(UINT terminalCount, UINT state, const ActionArray &actionArray)
: m_actionArray(actionArray)
, m_state(state)
{
  const size_t count = m_actionArray.size();
  CompactUIntHashMap<UINT, 1999>  sameReductionMap; // map from reduce-production -> index into m_shiftActionArray
  for(auto it = m_actionArray.getIterator(); it.hasNext();) {
    const ParserAction &pa = it.next();
    if(pa.m_action > 0) {
      m_shiftActionArray.add(pa);
    } else {
      const UINT  prod = -pa.m_action;
      const UINT *indexp = sameReductionMap.get(prod);
      if(indexp) {
        m_sameReductionArray[*indexp].addTerminal(pa.m_token);
      } else {
        const UINT index = (UINT)m_sameReductionArray.size();
        m_sameReductionArray.add(SameReduceActionInfo(terminalCount, prod, pa.m_token));
        sameReductionMap.put(prod, index);
      }
    }
  }
}

CompressionMethod StateActionInfo::getCompressionMethod() const {
  const UINT ac = getDifferentActionCount();
  switch(ac) {
  case 1 :
    if(m_shiftActionArray.size() == 1) {
      return ONEITEMCOMPRESSION;
    } else {
      return (m_sameReductionArray[0].getTerminalSet().size() == 1)
           ? ONEITEMCOMPRESSION
           : REDUCEBYSAMEPRODCOMPRESSION;
    }
//  case 2 :
//    return SPLITCOMPRESSION;
  default:
    return UNCOMPRESSED;
  }
}

CompressedActionMatrix::CompressedActionMatrix(const GrammarTables &tables)
  : m_tables(           tables                   )
  , m_stateCount(       tables.getStateCount()   )
  , m_terminalCount(    tables.getTerminalCount())
  , m_laSetSizeInBytes((tables.getTerminalCount() - 1) / 8 + 1)
{
  m_stateInfoArray.setCapacity(m_stateCount);

  const Array<ActionArray> &stateActions = m_tables.getStateActions();
  for(UINT state = 0; state < m_stateCount; state++) {
    m_stateInfoArray.add(StateActionInfo(m_terminalCount, state, stateActions[state]));
  }
  generateCompressedForm();
}

ByteCount CompressedActionMatrix::print(MarginFile &output) const {
  ByteCount byteCount;
  byteCount += printDefinesAndActionCode(output);
  byteCount += printTermAndActionList(   output);
  byteCount += printTermSetTable(        output);
  return byteCount;
}

void CompressedActionMatrix::addACdefine(UINT state, const String &value, const String &comment) {
  m_defines.add(format(_T("_ac%04u %-10s /* %-40s*/"), state, value.cstr(), comment.cstr()));
}

void CompressedActionMatrix::doUncompressedState(const StateActionInfo &stateInfo) {
  const UINT        state  = stateInfo.getState();
  const SymbolSet   laSet  = m_tables.getLookaheadSet(state);
  IndexMapValue    *imvp   = m_termListMap.get(laSet);
  UINT              termListIndex, laCount;

  if(imvp != nullptr) {
    termListIndex = imvp->m_arrayIndex;
    laCount       = imvp->m_commentIndex;
    imvp->addState(state);
  } else {
    termListIndex = m_currentTermListSize;
    laCount       = m_termListMap.getCount();
    IndexMapValue nv(m_stateCount, state, termListIndex);
    m_termListMap.put(laSet, nv);
    m_currentTermListSize += (UINT)laSet.size() + 1;
  }

  const RawActionArray raa = m_tables.getRawActionArray(state);
  imvp = m_raaMap.get(raa);
  UINT                 actionListIndex, raCount;
  if(imvp != nullptr) {
    actionListIndex = imvp->m_arrayIndex;
    raCount         = imvp->m_commentIndex;
    imvp->addState(state);
  } else {
    actionListIndex = m_currentActionListSize;
    raCount         = m_raaMap.getCount();
    IndexMapValue nv(m_stateCount, state, actionListIndex);
    m_raaMap.put(raa, nv);
    m_currentActionListSize += (UINT)raa.size();
  }
  const UINT   encodedValue = ((UINT)actionListIndex << 17) | (UINT)termListIndex;
  const String macroValue   = format(_T("0x%08x"), encodeValue(encodedValue,UNCOMPRESSED));
  const String comment      = format(_T("termList %3u, actionList %3u"), laCount, raCount);
  addACdefine(state, macroValue, comment);
}

void CompressedActionMatrix::doSplitCompression(const StateActionInfo &stateInfo) {
  const ActionArray &sa = stateInfo.getShiftActionArray();
//  const stateInfo.getReduceActionArray()
}

void CompressedActionMatrix::doOneItemState(const StateActionInfo &stateInfo) {
  const UINT         state = stateInfo.getState();
  const ActionArray &paa   = stateInfo.getActions();

  assert(paa.size() == 1);

  const ParserAction &pa = paa[0];
  const int           action  = pa.m_action;                    // positive or negative
  const UINT          token   = pa.m_token;
  const String        comment = (action <= 0)
                              ? format(_T("Reduce by %u on %s"), -action, m_tables.getSymbolName(pa.m_token))
                              : format(_T("Shift  to %u on %s"),  action, m_tables.getSymbolName(pa.m_token));
  CHECKMAX15BITS(token);
  CHECKMAX15BITS(abs(action));
  const UINT          encodedValue = ((action << 17) | token);
  const String        macroValue   = format(_T("0x%08x"), encodeValue(encodedValue,ONEITEMCOMPRESSION));
  addACdefine(state, macroValue, comment);
}

void CompressedActionMatrix::doReduceBySameProdState(const StateActionInfo &stateInfo) {
  const UINT                         state = stateInfo.getState();
  const Array<SameReduceActionInfo> &raa   = stateInfo.getReduceActionArray();

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
    IndexMapValue nv(m_stateCount, state, byteIndex);
    m_laSetMap.put(laSet, nv);
    m_currentLASetArraySize += m_laSetSizeInBytes;
  }
  const int           prod         = raa[0].getProduction();
  const int           action       = -prod;
  const String        comment      = format(_T("Reduce by %u on tokens in termSet[%u]"), prod, termSetCount);
  CHECKMAX15BITS(byteIndex);
  CHECKMAX15BITS(abs(action));
  const UINT          encodedValue = ((action << 17) | byteIndex);
  const String        macroValue   = format(_T("0x%08x"), encodeValue(encodedValue,REDUCEBYSAMEPRODCOMPRESSION));
  addACdefine(state, macroValue, comment);
}

void CompressedActionMatrix::generateCompressedForm() {
  m_currentTermListSize   = 0;
  m_currentActionListSize = 0;
  m_currentLASetArraySize = 0;
  ByteCount totalByteCount;

  for(UINT state = 0; state < m_stateCount; state++) {
    const StateActionInfo &stateInfo = m_stateInfoArray[state];
    switch(stateInfo.getCompressionMethod()) {
    case UNCOMPRESSED               :
      doUncompressedState(stateInfo);
      break;
    case SPLITCOMPRESSION           :
      doSplitCompression(stateInfo);
      break;
    case ONEITEMCOMPRESSION:
      doOneItemState(stateInfo);
      break;
    case REDUCEBYSAMEPRODCOMPRESSION:
      doReduceBySameProdState(stateInfo);
      break;
    default                         :
      throwException(_T("%s:unknown compressionMethod for state %u"), __TFUNCTION__, state);
      break;
    }
  }
}

ByteCount CompressedActionMatrix::printTermAndActionList(MarginFile &output) const {
  ByteCount byteCount;
  if(m_currentTermListSize == 0) {
    output.printf(_T("#define termListTable   nullptr\n"  ));
    output.printf(_T("#define actionListTable nullptr\n\n"));
    return byteCount;
  }

  { const SymbolSetIndexArray termListArray = m_termListMap.getEntryArray();
    const IntegerType         terminalType  = m_tables.getTerminalType();
    UINT                      tableSize     = 0;
    TCHAR                     delim         = ' ';

    output.printf(_T("static const %s termListTable[%u] = {\n"), getTypeName(terminalType), termListArray.getElementCount(true));
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
    const ByteCount tableByteCount = ByteCount::wordAlignedSize(tableSize * getTypeSize(terminalType));
    output.setLeftMargin(0);
    output.printf(_T("}; // Size of table:%s.\n\n"), tableByteCount.toString().cstr());
    byteCount += tableByteCount;
  }

  { const RawActionArrayIndexArray raaArray   = m_raaMap.getEntryArray();
    const IntegerType              actionType = m_tables.getActionType();
    UINT                           tableSize  = 0;
    TCHAR                          delim      = ' ';

    output.printf(_T("static const %s actionListTable[%u] = {\n"), getTypeName(actionType), raaArray.getElementCount(false));
    output.setLeftMargin(2);
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
    const ByteCount tableByteCount = ByteCount::wordAlignedSize(tableSize * getTypeSize(actionType));
    output.setLeftMargin(0);
    output.printf(_T("}; // Size of table:%s.\n\n"), tableByteCount.toString().cstr());
    byteCount += tableByteCount;
  }

  return byteCount;
}

ByteCount CompressedActionMatrix::printTermSetTable(MarginFile &output) const {
  ByteCount byteCount;
  if(m_laSetMap.size() == 0) {
    output.printf(_T("#define termSetTable nullptr\n\n"));
  } else {
    const SymbolSetIndexArray laSetArray = m_laSetMap.getEntryArray();
    output.printf(_T("static const unsigned char termSetTable[%u] = {\n"), m_currentLASetArraySize);
    output.setLeftMargin(2);
    TCHAR delim = ' ';
    for(ConstIterator<IndexArrayEntry<SymbolSet> > it = laSetArray.getIterator(); it.hasNext();) {
      const IndexArrayEntry<SymbolSet> &e = it.next();
      const ByteArray                   ba = bitSetToByteArray(e.m_key);
      const UINT                        n = (UINT)ba.size();
      String                            comment = format(_T("%3u %3u tokens %s"), e.m_commentIndex, (UINT)e.m_key.size(), e.getComment().cstr());
      for(const BYTE *cp = ba.getData(), *last = cp + n; cp < last; delim = ',') {
        output.printf(_T("%c0x%02x"), delim, *(cp++));
      }
      newLine(output, comment);
    }
    output.setLeftMargin(0);
    byteCount = ByteCount::wordAlignedSize(m_currentLASetArraySize * sizeof(char));
    output.printf(_T("}; // Size of table:%s.\n\n"), byteCount.toString().cstr());
  }
  return byteCount;
}

ByteCount CompressedActionMatrix::printDefinesAndActionCode(MarginFile &output) const {
  const UINT defineCount = (UINT)m_defines.size();
  if(defineCount > 0) {
    m_defines.sort(stringCmp);
    for(ConstIterator<String> it = m_defines.getIterator(); it.hasNext();) {
      output.printf(_T("#define %s\n"), it.next().cstr());
    }
    output.printf(_T("\n"));
  }

  output.setLeftMargin(0);
  output.printf(_T("static const unsigned int actionCode[%u] = {\n"), defineCount);
  output.setLeftMargin(2);
  TCHAR delim = ' ';
  for(UINT d = 0; d < defineCount; d++, delim = ',') {
    output.printf(_T("%c_ac%04d"), delim, d);
    if((d % 10 == 9) && (d != defineCount - 1)) {
      output.printf(_T("\n"));
    }
  }
  output.setLeftMargin(0);
  const ByteCount byteCount = ByteCount::wordAlignedSize(defineCount * sizeof(UINT));
  output.printf(_T("\n}; // Size of table:%s.\n\n"), byteCount.toString().cstr());
  return byteCount;
}

}; // namespace ActionMatrix

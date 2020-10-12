#include "stdafx.h"
#include "GrammarCode.h"

static const TCHAR *TABLETYPE = _T("static const TableType");

static const TCHAR *comment1 =
_T("/******************************************************************************\\\n"
   "* The action matrix holds the parse action(state,terminal)                     *\n"
   "* Used in LRParser::parserStep() tp determine what to do in the current state  *\n"
   "* and a given terminal on input. The interpretation of action is:              *\n"
   "*                                                                              *\n"
   "*   action <  0   - Reduce by production p, p == -action.                      *\n"
   "*   action == 0   - Accept. Reduce by production 0.                            *\n"
   "*   action >  0   - Go to state s (=action),                                   *\n"
   "*                   and push [s,input,pos] to parser stack.                    *\n"
   "*                   Then read next symbol from input.                          *\n"
   "*   action == _ParserError - not found (=unexpected input).                    *\n"
   "*                                                                              *\n"
   "* 3 different formats are used:1 Uncompressed and 2 Compressed.                *\n"
   "* Uncompressed state:                                                          *\n"
   "*   The array uncompressedActions[] holds a list of numbers for each state     *\n"
   "*   starting with number of items, M, belonging to the state, followed by M    *\n"
   "*   pairs, each consisting of (token,action)                                   *\n"
   "*     Item list for state NNNN with 2 items             2, 1, 2, 2,-3          *\n"
   "*     Number of pairs in list---------------------------+  |  |  |  |          *\n"
   "*     Legal input symbol-----------------------------------+  |  |  |          *\n"
   "*     Action-(> 0 => shift input and goto state 1)------------+  |  |          *\n"
   "*     Legal input symbol-----------------------------------------+  |          *\n"
   "*     Action-(< 0 => reduce by production 3)------------------------+          *\n"
   "*   A line containing \"#define _acNNNN Index\" is added, where Index is a       *\n"
   "*   reference to the first number in list of numbers belonging to the state    *\n"
   "*                                                                              *\n"
   "* Compressed state:                                                            *\n"
   "*   Single-item-state (SIS) ot Multi-item-state (MIS)                          *\n"
   "*   SIS:If there is only 1 possible action in the state:                       *\n"
   "*     A line containing \"#define _acNNNN code\" is added, where code is         *\n"
   "*     encoded as:          ((Action&0x7fff) << 16) | (Token&0x7fff) )          *\n"
   "*     Bit[16-30] = Action-------+                       |                      *\n"
   "*     Bit[0 -14] = Legal token--------------------------+                      *\n"
   "*     Bit 31     = 1 Compressed state indicator                                *\n"
   "*     Bit 15     = 0 SIS compression                                           *\n"
   "*                                                                              *\n"
   "*   MIS:If all actions in the state are reduce by the same production:         *\n"
   "*     A line containing \"#define _acNNNN code\" is added, where code is         *\n"
   "*     encoded as:          ((Action&0x7fff) << 16) | (Index&0x7fff))           *\n"
   "*     Bit[16-30] = Action-------+                       |                      *\n"
   "*     Bit[0 -14] = Index into compressedLasets----------+                      *\n"
   "*     Bit 31     = 1 Compressed state indicator                                *\n"
   "*     Bit 15     = 1 MIS compression                                           *\n"
   "*                                                                              *\n"
   "* CompressedLasets is a list of bitsets, containing 1-bits for all legal       *\n"
   "* inputsymbols in the given state (MIS). Many MIS may refer to the same bitset *\n"
   "* Number of bytes in each bitset = (terminalcount-1)/8+1                       *\n"
   "* Index in MIS-code refer to the first byte in the bitset belonging the state  *\n"
   "*                                                                              *\n"
   "* If any uncompressed state N has the same actionarray as a previous           *\n"
   "* generated uncompressed state M, a #define _acN _acM                          *\n"
   "*                                                                              *\n"
   "\\******************************************************************************/\n")
  ;
static const TCHAR *comment2 =
_T("/******************************************************************************\\\n"
   "* The successor matrix is used when the parser has reduced by prod A -> alfa   *\n"
   "* The number of elements popped from the stack is the length of the alfa, L.   *\n"
   "* and the state is taken from stacktop. The nonterminal A is leftside of the   *\n"
   "* reduce production                                                            *\n"
   "* Used by LRParser to find newstate = successor(state,A).                      *\n"
   "* For each relevant state the array stateSuccessors contains a list of numbers *\n"
   "*                                                                              *\n"
   "*   Item list for state NNNN with 3 items   3, 5,3 , 6,2, 8,5                  *\n"
   "*   Number of pairs in list ----------------+  | |                             *\n"
   "*   NonTerminal A------------------------------+ |                             *\n"
   "*   Goto this state------------------------------+                             *\n"
   "*                                                                              *\n"
   "* The array successorsIndex contains an index for each of these states         *\n"
   "* referering to the first number belonging to the state (as actionCode)        *\n"
   "* or 0 if a state has no items of the form B -> beta . A gamma                 *\n"
   "\\******************************************************************************/\n")
  ;
static const TCHAR *comment3 =
_T("/******************************************************************************\\\n"
   "* The productionLength[] array is indexed by production number and holds       *\n"
   "* the number of symbols on the right side of each production.                  *\n"
   "\\******************************************************************************/\n")
  ;
static const TCHAR *comment4 =
_T("/******************************************************************************\\\n"
   "* The leftSide[] array is indexed by production number, and holds the          *\n"
   "* nonTerminal A on the left side of each production.                           *\n"
   "\\******************************************************************************/\n")
  ;

static const TCHAR *comment5 =
_T("/******************************************************************************\\\n"
   "* The rightSide[] matrix is indexed by production number and holds             *\n"
   "* the right side symbols of each production.                                   *\n"
   "* Compressed and only used for debugging.                                      *\n"
   "\\******************************************************************************/\n")
  ;
static const TCHAR *comment6 =
_T("/******************************************************************************\\\n"
   "* symbolNames contains names of terminal and nonTerminal separated by space    *\n"
   "* Used for debugging.                                                          *\n"
   "\\******************************************************************************/\n")
  ;

void GrammarTables::printCpp(MarginFile &output, bool useTableCompression) const {
  const bool useShortTable = isTableTypeShort();
  const UINT tableTypeSize = getTableTypeSize();

  m_countTableBytes.reset();

  output.printf(_T("typedef %s TableType;\n\n"), getTableTypeName());

  if(!useTableCompression) {
    m_compressibleStateSet.clear();
  }
  output.printf(comment1);
  StringArray actionDefines(getStateCount());
  m_countTableBytes += printCompressedActionsCpp(    output, actionDefines);
  m_countTableBytes += printUncompressedActionsCpp(  output, actionDefines);
  m_countTableBytes += printActionMatrixCpp(         output);
  m_countTableBytes += printSuccessorMatrixCpp(      output);
  m_countTableBytes += printProductionLengthTableCpp(output);
  m_countTableBytes += printLeftSideTableCpp(        output);
  m_countTableBytes += printRightSideTableCpp(       output);
  m_countTableBytes += printSymbolNameTableCpp(      output);

  const int sizeofTableTemlatex86  = 68;  // sizeof(ParserTablesTemplate) x86
  const int sizeofTableTemplatex64 = 120; // sizeof(ParserTablesTemplate) x64

  const ByteCount tableClassSize(sizeofTableTemlatex86, sizeofTableTemplatex64);
  m_countTableBytes += tableClassSize;
  m_countTableBytes += ByteCount::s_pointerSize;

  output.printf(_T("static const ParserTablesTemplate<TableType, %s> %s_s("), getIndexTypeName(m_succIndexType), m_tablesClassName.cstr());

  const int column = output.getCurrentLineLength()-1;
  output.printf(_T("actionCode, compressedLasets, uncompressedActions\n"
                   "%*s,successorsIndex , stateSuccessors\n"
                   "%*s,productionLength, leftSide\n"
                   "%*s,rightSideTable  , symbolNames\n"
                   "%*s,%u, %u, %u, %u, %u, %u);\n\n")
                   ,column,EMPTYSTRING,column,EMPTYSTRING,column,EMPTYSTRING,column,EMPTYSTRING
                   ,getTerminalCount(), getSymbolCount(), getProductionCount(), getStateCount()
                   ,m_countTableBytes.getByteCount(PLATFORM_X86),m_countTableBytes.getByteCount(PLATFORM_X64));

  output.printf(_T("const ParserTables *%s::%s = &%s_s;\n")
               ,m_parserClassName.cstr()
               ,m_tablesClassName.cstr()
               ,m_tablesClassName.cstr());
  output.printf(_T("// Size of %s_s: %s. Size of %s:%s\n")
               ,m_tablesClassName.cstr(), tableClassSize.toString().cstr()
               ,m_tablesClassName.cstr(), ByteCount::s_pointerSize.toString().cstr());

  output.printf(_T("\n// Total size of table data:%s.\n"), m_countTableBytes.toString().cstr());
}

ByteCount GrammarTables::printActionMatrixCpp(MarginFile &output) const {
  const int stateCount = getStateCount();
  output.setLeftMargin(0);
  output.printf(_T("static const unsigned int actionCode[%d] = {\n"), stateCount);
  output.setLeftMargin(2);
  for(int s = 0; s < stateCount; s++) {
    output.printf(s > 0 ? _T(","):_T(" "));
    output.printf(_T("_ac%04d"), s);
    if(s % 10 == 9 || s == stateCount-1) {
      output.printf(_T("\n"));
    }
  }
  output.setLeftMargin(0);
  const ByteCount byteCount = wordAlignedSize(stateCount*sizeof(UINT));
  output.printf(_T("}; // Size of table:%s.\n\n"), byteCount.toString().cstr());
  return byteCount;
}

// ---------------------------------------- Compressed actions ---------------------------------------------

class CompressedLookaheadMap : public TreeMap<BitSet, USHORT> {
public:
  CompressedLookaheadMap() : TreeMap<BitSet, USHORT>(bitSetCmp) {
  }
};

typedef Entry<BitSet, USHORT> LASetEntry;

class BitSetWithIndex : public BitSet {
public:
  USHORT m_index;
  BitSetWithIndex(const LASetEntry &e) : BitSet(e.getKey()), m_index(e.getValue()) {
  }
};

static int LASetIndexCmp(const BitSetWithIndex &s1, const BitSetWithIndex &s2) {
  return (int)s1.m_index - (int)s2.m_index;
}

ByteCount GrammarTables::printCompressedActionsCpp(MarginFile &output, StringArray &defines) const {
  CompressedLookaheadMap laSetMap;
  CompactIntArray        setIndex;

  const UINT stateCount = getStateCount();

  for(UINT state = 0; state < stateCount; state++) {
    if(isCompressibleState(state) && !isOneItemState(state)) {
      const BitSet  laSet = getLookaheadSet(state) ;
      const USHORT *indexp = laSetMap.get(laSet);
      USHORT index;
      if(indexp != NULL) {
        index = *indexp;
      } else {
        index = (USHORT)laSetMap.size();
        laSetMap.put(laSet, index);
      }
      setIndex.add(index);
    } else {
      setIndex.add(-1);
    }
  }
  const UINT laSetCount       = (UINT)laSetMap.size();
  UINT       laSetSize1State  = 0;
  ByteCount  byteCount;

  if(laSetCount == 0) {
    output.printf(_T("#define compressedLasets NULL\n\n"));
  } else {
    Array<BitSetWithIndex> laSetArray;
    for(Iterator<LASetEntry> it = laSetMap.getIterator(); it.hasNext();) {
      laSetArray.add(it.next());
    }
    laSetMap.clear();
    laSetArray.sort(LASetIndexCmp);
    ByteArray laSetTotal;
    StringArray linePrefix;
    for(UINT i = 0; i < laSetCount; i++) {
      laSetTotal.addAll(bitSetToByteArray(laSetArray[i]));
      linePrefix.add(format(_T("%2u %3zu tokens"), i, laSetArray[i].size()));
    }
    laSetSize1State = (UINT)laSetTotal.size() / laSetCount; // all laSets are the same size
    byteCount = printByteArray(output, _T("compressedLasets"), laSetTotal, laSetSize1State, &linePrefix);
    output.printf(_T("\n"));
  }

  constexpr UINT compressedBit = 0x80000000;

  for(UINT state = 0; state < stateCount; state++) {
    if(isCompressibleState(state)) {
      String comment;
      UINT   encodedValue;
      if(isOneItemState(state)) {
        const ParserAction &pa     = m_stateActions[state][0];
        const short         action = pa.m_action;                    // positive or negative
        const USHORT        token  = pa.m_token;
        assert((token       & 0x8000) == 0);
        assert((abs(action) & 0x8000) == 0);
        comment = (action <= 0)
                ? format(_T("Reduce by %u on %s"), -action, getSymbolName(pa.m_token))
                : format(_T("Shift  to %u on %s"),  action, getSymbolName(pa.m_token));
        encodedValue = ((action << 16) | token) & 0x7fff7fff;
      } else {
        const short  action     = m_stateActions[state][0].m_action; // always negative
        const USHORT index      = setIndex[state];
        const USHORT byteIndex  = index * laSetSize1State;
        assert((byteIndex   & 0x8000) == 0);
        assert((abs(action) & 0x8000) == 0);
        comment      = format(_T("Reduce by %u on tokens in set[%u]"), -action, index);
        encodedValue = ((action<<16) | 0x8000 | byteIndex) & 0x7fffffff;
      }
      assert((encodedValue & compressedBit) == 0);
      const String macroValue = format(_T("0x%08x"), compressedBit | (encodedValue & 0x7fffffff));
      defines.add(format(_T("_ac%04u %-10s /* %-40s*/"), state, macroValue.cstr(), comment.cstr()));
    }
  }
  return byteCount;
}

// ---------------------------------------- Uncompressed actions ---------------------------------------------

static ULONG actionArrayHash(const ActionArray &a) {
  return a.hashCode();
}

static int actionArrayCmp(const ActionArray &a1, const ActionArray &a2) {
  return (a1 == a2) ? 0 : 1;
}

class ActionArrayMap : public HashMap<ActionArray, UINT> {
public:
  ActionArrayMap() : HashMap<ActionArray, UINT>(actionArrayHash, actionArrayCmp, 1000) {
  }
};

static int stringCmp(const String &s1, const String &s2) {
  return _tcscmp(s1.cstr(), s2.cstr());
}

ByteCount GrammarTables::printUncompressedActionsCpp(MarginFile &output, StringArray &defines) const {
  const UINT stateCount    = getStateCount();
  const UINT tableTypeSize = getTableTypeSize();

  m_uncompressedStateBytes.reset();
  ActionArrayMap   actionMap;
  UINT             currentUncompressedIndex = 0;
  output.printf(_T("%s uncompressedActions[] = {\n"), TABLETYPE);
  for(UINT state = 0; state < stateCount; state++) {
    if(!isCompressibleState(state)) {
      const ActionArray &sa    = m_stateActions[state];
      const UINT         count = (UINT)sa.size();
      const UINT        *ps    = actionMap.get(sa);
      String macroValue, comment;
      if(ps) {
        macroValue = format(_T("_ac%04u"), *ps);
        comment    = format(_T("Saved %u bytes"), tableTypeSize * (2*count+1));
      } else {
        printUncompressedActionArrayCpp (output, state, currentUncompressedIndex);
        actionMap.put(sa, state);
        macroValue = format(_T("%u"), currentUncompressedIndex);
        comment    = format(_T("Index of uncompressed state[%d]"), state);
        currentUncompressedIndex += 2*count + 1;
      }
      defines.add(format(_T("_ac%04u %-10s /* %-40s*/"), state, macroValue.cstr(), comment.cstr()));
    }
  }
  output.setLeftMargin(0);
  const ByteCount byteCount = wordAlignedSize(currentUncompressedIndex * getTableTypeSize());

  output.printf(_T("}; // Size of table:%s.\n\n"), byteCount.toString().cstr());
  if(defines.size() > 0) {
    defines.sort(stringCmp);
    for(size_t i = 0; i < defines.size(); i++) {
      output.printf(_T("#define %s\n"), defines[i].cstr());
    }
    output.printf(_T("\n"));
  }
  return byteCount;
}

void GrammarTables::printUncompressedActionArrayCpp(MarginFile &output, UINT state, UINT startIndex) const {
  const ActionArray &actions       = m_stateActions[state];
  const UINT         actionCount   = (UINT)actions.size();

  output.setLeftMargin(2);
  String comment = format(_T("state[%d],Index=%d"), state, startIndex);
  output.printf(_T("/* %-36s */ %c%3d"), comment.cstr(), (startIndex==0)?' ':',', actionCount);
  for(UINT a = 0; a < actionCount; a++) {
    const ParserAction &pa = actions[a];
    output.printf(_T(",%4d,%4d"), pa.m_token, pa.m_action);
    if((a % 5 == 4) && (a != actionCount-1)) {
      output.printf(_T("\n"));
      output.setLeftMargin(49);
    }
  }
  output.printf(_T("\n"));
}

ByteCount GrammarTables::printSuccessorMatrixCpp(MarginFile &output) const {
  output.printf(comment2);
  const UINT       stateCount    = getStateCount();
  const UINT       tableTypeSize = getTableTypeSize();
  ActionArrayMap   successorMap;
  UINT             currentIndex = 0;
  CompactIntArray  startIndex(stateCount);

  output.printf(_T("%s stateSuccessors[] = {\n"), TABLETYPE);
  for(UINT state = 0; state < stateCount; state++) {
    const ActionArray &succlist = m_stateSucc[state];
    const UINT         count    = (UINT)succlist.size();
    if(count == 0) {
      startIndex.add(-1);
    } else {
      const UINT *ps = successorMap.get(succlist);
      if(ps) {
        startIndex.add(*ps);
      } else {
        startIndex.add(currentIndex);
        successorMap.put(succlist, currentIndex);
        printSuccessorArrayCpp(output, state, currentIndex);
        currentIndex += count * 2 + 1;
      }
    }
  }
  const ByteCount table1Size = wordAlignedSize(tableTypeSize * currentIndex);
  output.printf(_T("}; // Size of table:%s.\n\n"), table1Size.toString().cstr());

  output.setLeftMargin(0);
  m_succIndexType = findIndexType(currentIndex);
  const TCHAR *succIndexTypeName = getIndexTypeName(m_succIndexType);
  output.printf(_T("#define nil (%s)-1\n"), succIndexTypeName);
  output.printf(_T("static const %s successorsIndex[%u] = {\n"), succIndexTypeName, stateCount);
  output.setLeftMargin(2);
  for(UINT state = 0; state < stateCount; state++) {
    output.printf(state > 0 ? _T(","):_T(" "));
    const int index = startIndex[state];
    if(index < 0) {
      output.printf(_T("%5s"), _T("nil"));
    } else {
      output.printf(_T("%5d"), index);
    }
    if((state % 10 == 9) || (state == stateCount-1)) {
      output.printf(_T("\n"));
    }
  }
  output.setLeftMargin(0);
  const ByteCount table2Size = wordAlignedSize(stateCount * getIndexTypeSize(m_succIndexType));
  output.printf(_T("}; // Size of table:%s.\n\n"), table2Size.toString().cstr());
  return table1Size + table2Size;
}

void GrammarTables::printSuccessorArrayCpp(MarginFile &output, UINT state, UINT startIndex) const {
  const ActionArray &succlist = m_stateSucc[state];
  const UINT         count    = (UINT)succlist.size();

  output.setLeftMargin(2);
  String comment = format(_T("successors state[%u], Index=%u"), state, startIndex);
  output.printf(_T("/* %-36s */ %c%3d"), comment.cstr(), (startIndex==0)?' ':',', count);
  for(UINT a = 0; a < count; a++) {
    const ParserAction &pa = succlist[a];
    output.printf(_T(",%4d,%4d"), pa.m_token, pa.m_action);
    if((a % 5 == 4) && (a != count-1)) {
      output.printf(_T("\n"));
      output.setLeftMargin(49);
    }
  }
  output.setLeftMargin(0);
  output.printf(_T("\n"));
}

ByteCount GrammarTables::printProductionLengthTableCpp(MarginFile &output) const {
  output.printf(comment3);

  const UINT productionCount = getProductionCount();

  output.printf(_T("static const unsigned char productionLength[%u] = {\n"), productionCount);
  output.setLeftMargin(2);
  for(UINT p = 0; p < productionCount; p++) {
    const UINT l = m_productionLength[p];
    if(p % 10 == 0) {
      output.printf(_T("/* %3u */ "), p);
    }
    output.printf(_T(" %3u"), l);
    if(p < productionCount-1) {
      output.printf(_T(","));
    }
    if(p % 10 == 9) {
      output.printf(_T("\n"));
    }
  }
  output.setLeftMargin(0);

  const ByteCount byteCount = wordAlignedSize(productionCount*sizeof(char));

  output.printf(_T("\n}; // Size of table:%s.\n\n"), byteCount.toString().cstr());

  return byteCount;
}

ByteCount GrammarTables::printLeftSideTableCpp(MarginFile &output) const {
  output.printf(comment4);

  const UINT productionCount = getProductionCount();
  output.printf(_T("%s leftSide[%u] = {\n"), TABLETYPE, productionCount);
  output.setLeftMargin(2);

  for(UINT p = 0; p < productionCount; p++) {
    const UINT l = m_left[p];
    if(p % 10 == 0) {
      output.printf(_T("/* %3u */ "), p);
    }
    output.printf(_T(" %3u"), l);
    if(p < productionCount-1) {
      output.printf(_T(","));
    }
    if(p % 10 == 9) {
      output.printf(_T("\n"));
    }
  }
  output.setLeftMargin(0);

  const ByteCount byteCount = wordAlignedSize(productionCount * getTableTypeSize());
  output.printf(_T("\n}; // Size of table:%s.\n\n"), byteCount.toString().cstr());
  return byteCount;
}

ByteCount GrammarTables::printRightSideTableCpp(MarginFile &output) const {
  output.printf(comment5);

  const UINT productionCount = getProductionCount();
  UINT       totalItemCount  = 0;

  for(UINT p = 0; p < productionCount; p++) {
    totalItemCount += (UINT)m_rightSide[p].size();
  }
  TCHAR *delim = _T(" ");
  output.printf(_T("%s rightSideTable[%u] = {\n"), TABLETYPE, totalItemCount);
  output.setLeftMargin(2);
  for(UINT p = 0; p < productionCount; p++) {
    const CompactIntArray &r = m_rightSide[p];
    if(r.size() == 0) {
      continue;
    }
    output.printf(_T("/* %3u */ "), p);
    for(size_t i = 0; i < r.size(); i++, delim = _T(",")) {
      output.printf(_T("%s%3d"), delim, r[i]);
    }
    output.printf(_T("\n"));
  }
  output.setLeftMargin(0);

  const ByteCount byteCount = wordAlignedSize(totalItemCount * getTableTypeSize());
  output.printf(_T("}; // Size of table:%s.\n"), byteCount.toString().cstr());
  output.printf(_T("\n"));

  return byteCount;
}

ByteCount GrammarTables::printSymbolNameTableCpp(MarginFile &output) const {
  output.printf(comment6);

  const UINT terminalCount = getTerminalCount();
  const UINT symbolCount   = getSymbolCount();
  size_t     charCount     = 0;
  output.printf(_T("static const char *symbolNames = {\n"));
  output.setLeftMargin(2);
  for(UINT s = 0; s < symbolCount; s++) {
    const String &name = m_symbolName[s];
    if(s == 0) {
      output.printf(_T("\"%s\"\n"), name.cstr());
    } else {
      output.printf(_T("\" %s\"\n"), name.cstr());
    }
    charCount += name.length() + 1;
  }
  output.setLeftMargin(0);

  const ByteCount stringByteCount = wordAlignedSize(sizeof(char)*(UINT)charCount);
  output.printf(_T("}; // Total size of strings:%s\n\n")
               ,stringByteCount.toString().cstr());

  return stringByteCount;
}

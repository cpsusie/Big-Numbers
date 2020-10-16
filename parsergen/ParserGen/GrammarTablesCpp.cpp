#include "stdafx.h"
#include <Comparator.h>
#include "GrammarCode.h"

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
  m_countTableBytes.reset();

  if(!useTableCompression) {
    m_compressibleStateSet.clear();
  }
  findTemplateTypes();
  output.printf(_T("%s"), comment1);
  m_countTableBytes += printActionMatrixCpp(             output);
  m_countTableBytes += printSuccessorMatrixCpp(          output);
  m_countTableBytes += printProductionLengthTableCpp(    output);
  m_countTableBytes += printLeftSideTableCpp(            output);
  m_countTableBytes += printRightSideTableCpp(           output);
  m_countTableBytes += printSymbolNameTableCpp(          output);

  const int sizeofTableTemlatex86  = 68;  // sizeof(ParserTablesTemplate) x86
  const int sizeofTableTemplatex64 = 120; // sizeof(ParserTablesTemplate) x64

  const ByteCount tableClassSize(sizeofTableTemlatex86, sizeofTableTemplatex64);
  m_countTableBytes += tableClassSize;
  m_countTableBytes += ByteCount::s_pointerSize;

  output.printf(_T("static const ParserTablesTemplate<"));
  int column = output.getCurrentLineLength();
  output.setLeftMargin(column - 1);
  output.printf(_T("%u,%u,%u,%u\n,%s\n,%s\n,%s\n,%s\n,%s> %s_s(")
               ,getTerminalCount(),getSymbolCount(), getProductionCount(), getStateCount()
               ,getTypeName(m_terminalType )
               ,getTypeName(m_NTIndexType  )
               ,getTypeName(m_symbolType   )
               ,getTypeName(m_actionType   )
               ,getTypeName(m_stateType    )
               ,m_tablesClassName.cstr()   );

  column = output.getCurrentLineLength();
  output.setLeftMargin(column - 1);
  output.printf(_T("actionCode      , compressedLAsets  , termListTable, actionListTable\n"
                   ",successorCode   , NTindexListTable  , stateListTable\n"
                   ",productionLength, leftSideTable\n"
                   ",rightSideTable  , symbolNames\n"
                   ",%u, %u);\n\n")
                   ,m_countTableBytes.getByteCount(PLATFORM_X86),m_countTableBytes.getByteCount(PLATFORM_X64));
  output.setLeftMargin(0);

  output.printf(_T("const ParserTables *%s::%s = &%s_s;\n")
               ,m_parserClassName.cstr()
               ,m_tablesClassName.cstr()
               ,m_tablesClassName.cstr());
  output.printf(_T("// Size of %s_s: %s. Size of %s:%s\n\n")
               ,m_tablesClassName.cstr(), tableClassSize.toString().cstr()
               ,m_tablesClassName.cstr(), ByteCount::s_pointerSize.toString().cstr());

  output.printf(_T("// Total size of table data:%s.\n"), m_countTableBytes.toString().cstr());
}

void GrammarTables::findTemplateTypes() const {
  const UINT ntCount = getSymbolCount() - getTerminalCount();
  m_terminalType     = findUintType(m_terminalCount  - 1);
  m_NTIndexType      = findUintType(ntCount          - 1);
  m_symbolType       = findUintType(getSymbolCount() - 1);
  m_stateType        = findUintType(getStateCount()  - 1);
  m_actionType       = ((getStateCount() < 128) && (getProductionCount() < 128))
                     ? TYPE_CHAR
                     : TYPE_SHORT;
}

// ---------------------------------------- Compressed actions ---------------------------------------------

class ArrayIndex {
public:
  const UINT m_arrayIndex; // offset into array
  int        m_count;      // counter
  ArrayIndex(UINT arrayIndex) : m_arrayIndex(arrayIndex), m_count(-1) {
  }
};

static int arrayIndexCmp(const ArrayIndex &i1, const ArrayIndex &i2) {
  return (int)i1.m_arrayIndex - (int)i2.m_arrayIndex;
}

class IndexMapValue : public ArrayIndex {
public:
  BitSet m_stateSet;
  IndexMapValue(size_t stateCount, UINT state0, UINT arrayIndex) : ArrayIndex(arrayIndex), m_stateSet(stateCount) {
    addState(state0);
  }
  void addState(UINT state) {
    m_stateSet.add(state);
  }
  String getComment() const;
};

String IndexMapValue::getComment() const {
  const size_t n = m_stateSet.size();
  return format(_T("Used by state%c %s"), (n == 1) ? ' ': 's', m_stateSet.toString().cstr());
}

template<typename Key> class IndexArrayEntry : public IndexMapValue {
public:
  Key m_key;
  IndexArrayEntry(const Entry<Key, IndexMapValue> &e) : IndexMapValue(e.getValue()), m_key(e.getKey()) {
  }
};

template<typename Key> class IndexComparator : public Comparator<IndexArrayEntry<Key> > {
public:
  int compare(const IndexArrayEntry<Key> &kv1, const IndexArrayEntry<Key> &kv2) override {
    return arrayIndexCmp(kv1, kv2);
  }
  AbstractComparator *clone() const override {
    return new IndexComparator<Key>;
  }
};

template<typename Key> class IndexArray : public Array<IndexArrayEntry<Key> > {
public:
  explicit IndexArray(size_t capacity) : Array<IndexArrayEntry<Key> >(capacity) {
  }
  void sortByIndex() {
    sort(IndexComparator<Key>());
  }
};

template<typename Key> class IndexMap : public TreeMap<Key, IndexMapValue> {
public:
  IndexMap(int (*cmp)(const Key &, const Key &)) : TreeMap<Key, IndexMapValue>(cmp) {
  }
  IndexArray<Key> getEntryArray() const {
    IndexArray<Key> a(size());
    for(ConstIterator<Entry<Key, IndexMapValue> > it = getIterator(); it.hasNext();) {
      a.add(it.next());
    }
    a.sortByIndex();
    const size_t n = size();
    for(UINT c = 0; c < n; c++) {
      a[c].m_count = c;
    }
    return a;
  }
};

typedef BitSet SymbolSet;

static int symbolSetCmp( const SymbolSet &k1, const SymbolSet &k2) {
  return bitSetCmp(k1, k2);
}

class SymbolSetIndexMap : public IndexMap<SymbolSet> {
public:
  SymbolSetIndexMap() : IndexMap<SymbolSet>(symbolSetCmp) {
  }
};

typedef IndexArray<SymbolSet> SymbolSetIndexArray;

static constexpr UINT compressedBit      = 0x00010000;
static constexpr UINT multiItemIndicator = 0x00008000;
static constexpr UINT codeMask           = compressedBit | multiItemIndicator;
static constexpr UINT dataMask           = ~codeMask;

static void checkMax15Bits(const TCHAR *method, int line, int v, const TCHAR *varName) {
  if((v & 0xffff8000) != 0) {
    throwException(_T("%s:(%d):value %s=(%08x) cannot be contained in 15 bits"), method, line, varName, v);
  }
}

#define CHECKMAX15BITS(v) checkMax15Bits(__TFUNCTION__,__LINE__,v,_T(#v))

/************************************\
 *  3         2         1         0  *
 * 10987654321098765432109876543210  *
 * aaaaaaaaaaaaaaaCIttttttttttttttt  *
\************************************/
ByteCount GrammarTables::printCompressedActionsCpp(MarginFile &output, StringArray &defines) const {
  const UINT        stateCount = getStateCount();
  const UINT        setSizeInBytes    = (m_terminalCount - 1) / 8 + 1;
  SymbolSetIndexMap laSetMap;
  UINT              currentArrayIndex = 0;
  ByteCount         totalByteCount;

  for(UINT state = 0; state < stateCount; state++) {
    if(isCompressibleState(state)) {
      if(isOneItemState(state)) {

        const ParserAction &pa           = m_stateActions[state][0];
        const int           action       = pa.m_action;                    // positive or negative
        const UINT          token        = pa.m_token;
        const String        comment      = (action <= 0)
                                         ? format(_T("Reduce by %4u on %s"), -action, getSymbolName(pa.m_token))
                                         : format(_T("Shift  to %4u on %s"),  action, getSymbolName(pa.m_token));
        CHECKMAX15BITS(token      );
        CHECKMAX15BITS(abs(action));
        const UINT          encodedValue = ((action << 17) | token);
        assert((encodedValue & codeMask) == 0);
        const String        macroValue   = format(_T("0x%08x"), compressedBit | encodedValue);
        defines.add(format(_T("_ac%04u %-10s /* %-40s*/"), state, macroValue.cstr(), comment.cstr()));

      } else { // compressed multi-item-state

        const BitSet   laSet  = getLookaheadSet(state);
        IndexMapValue *vp     = laSetMap.get(laSet);
        UINT           index;
        if(vp != nullptr) {
          index = vp->m_arrayIndex;
          vp->addState(state);
        } else {
          index = currentArrayIndex;
          IndexMapValue nv(stateCount, state, index);
          laSetMap.put(laSet, nv);
          currentArrayIndex += setSizeInBytes;
        }
      }
    }
  }

  if(laSetMap.size() == 0) {
    output.printf(_T("#define compressedLAsets nullptr\n\n"));
  } else {
    const SymbolSetIndexArray laSetArray = laSetMap.getEntryArray();
    laSetMap.clear();
    output.printf(_T("static const unsigned char compressedLAsets[%u] = {\n"), currentArrayIndex);
    output.setLeftMargin(2);
    TCHAR delim = ' ';
    for(ConstIterator<IndexArrayEntry<SymbolSet> > it = laSetArray.getIterator(); it.hasNext();) {
      const IndexArrayEntry<SymbolSet> &e       = it.next();
      const ByteArray                   ba      = bitSetToByteArray(e.m_key);
      const UINT                        n       = (UINT)ba.size();
      String                            comment = format(_T("%3u %3u tokens %s"), e.m_count, (UINT)e.m_key.size(), e.getComment().cstr());
      for(const BYTE *cp = ba.getData(), *last = cp+n; cp < last; delim = ',') {
        output.printf(_T("%c0x%02x"), delim, *(cp++));
      }
      newLine(output, comment);

      for(ConstIterator<size_t> it1 = e.m_stateSet.getIterator(); it1.hasNext();) {
        const UINT          state        = (UINT)it1.next();
        const int           action       = m_stateActions[state][0].m_action; // always negative
        const UINT          byteIndex    = e.m_arrayIndex;
        const String        comment      = format(_T("Reduce by %u on tokens in LAset[%u]"), -action, e.m_count);
        CHECKMAX15BITS(byteIndex);
        CHECKMAX15BITS(abs(action));
        const UINT          encodedValue = ((action<<17) | byteIndex);
        assert((encodedValue & codeMask) == 0);
        const String macroValue = format(_T("0x%08x"), compressedBit | multiItemIndicator | encodedValue );
        defines.add(format(_T("_ac%04u %-10s /* %-40s*/"), state, macroValue.cstr(), comment.cstr()));
      }
    }
    output.setLeftMargin(0);
    const ByteCount byteCount = wordAlignedSize(currentArrayIndex*sizeof(char));
    output.printf(_T("}; // Size of table:%s.\n\n"), byteCount.toString().cstr());
    totalByteCount += byteCount;
  }
  return totalByteCount;
}

// ---------------------------------------- Uncompressed actions ---------------------------------------------

static int rawActionArrayCmp(const RawActionArray &a1, const RawActionArray &a2) {
  const size_t n = a1.size();
  int          c = sizetHashCmp(n, a2.size());
  if(c) return c;
  for(const short *v1 = &a1.first(), *v2 = &a2.first(), *end = v1+n; v1 < end;) {
    c = shortHashCmp(*(v1++), *(v2++));
    if(c) return c;
  }
  return 0;
}

class RawActionArrayIndexMap : public IndexMap<RawActionArray> {
public:
  RawActionArrayIndexMap() : IndexMap<RawActionArray>(rawActionArrayCmp) {
  }
};

typedef IndexArray<RawActionArray> RawActionArrayIndexArray;

static int stringCmp(const String &s1, const String &s2) {
  return _tcscmp(s1.cstr(), s2.cstr());
}

ByteCount GrammarTables::printUncompressedActionMatrixCpp(MarginFile &output, StringArray &defines) const {
  const UINT             stateCount = getStateCount();
  SymbolSetIndexMap      laSetMap;
  RawActionArrayIndexMap raaMap;
  UINT                   currentLAListSize = 0;
  UINT                   currentRAListSize = 0;
  ByteCount              totalByteCount;

  for(UINT state = 0; state < stateCount; state++) {
    if(isCompressibleState(state)) {
      continue;
    }
    const SymbolSet   laSet  = getLookaheadSet(state);
    IndexMapValue    *imvp   = laSetMap.get(laSet);
    UINT              laIndex, laCount;

    if(imvp != nullptr) {
      laIndex = imvp->m_arrayIndex;
      laCount = imvp->m_count;
      imvp->addState(state);
    } else {
      laIndex = currentLAListSize;
      laCount = (UINT)laSetMap.size();
      IndexMapValue nv(stateCount, state, laIndex);
      nv.m_count = laCount;
      laSetMap.put(laSet, nv);
      currentLAListSize += (UINT)laSet.size() + 1;
    }

    const RawActionArray raa   = getRawActionArray(state);
    imvp                       = raaMap.get(raa);
    UINT                 raIndex, raCount;
    if(imvp != nullptr) {
      raIndex = imvp->m_arrayIndex;
      raCount = imvp->m_count;
      imvp->addState(state);
    } else {
      raIndex = currentRAListSize;
      raCount = (UINT)raaMap.size();
      IndexMapValue nv(stateCount, state, raIndex);
      nv.m_count = laCount;
      raaMap.put(raa, nv);
      currentRAListSize += (UINT)raa.size();
    }
    const UINT encodedValue = ((UINT)raIndex << 17) | (UINT)laIndex;
    assert((encodedValue & compressedBit) == 0);
    const String macroValue = format(_T("0x%08x"), encodedValue);
    const String comment    = format(_T("termList %3u, actionList %3u"), laCount, raCount);
    defines.add(format(_T("_ac%04u %-10s /* %-40s*/"), state, macroValue.cstr(), comment.cstr()));
  }

  if(currentLAListSize == 0) {
    output.printf(_T("#define termListTable   nullptr\n"));
    output.printf(_T("#define actionListTable nullptr\n\n"));
  } else {
    { const SymbolSetIndexArray laSetArray = laSetMap.getEntryArray();
      laSetMap.clear();

      UINT       tableSize = 0;
      TCHAR      delim     = ' ';
      output.printf(_T("static const %s termListTable[] = {\n"), getTypeName(m_terminalType));
      for(ConstIterator<IndexArrayEntry<SymbolSet>> it = laSetArray.getIterator(); it.hasNext();) {
        const IndexArrayEntry<SymbolSet> &e       = it.next();
        String                            comment = format(_T(" %3u %s"), e.m_count, e.getComment().cstr());
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
      const ByteCount byteCount = wordAlignedSize(tableSize * getTypeSize(m_terminalType));
      output.setLeftMargin(0);
      output.printf(_T("}; // Size of table:%s.\n\n"), byteCount.toString().cstr());
      totalByteCount += byteCount;
    }

    { const RawActionArrayIndexArray raaArray  = raaMap.getEntryArray();
      raaMap.clear();

      UINT       tableSize = 0;
      TCHAR      delim     = ' ';
      output.printf(_T("static const %s actionListTable[] = {\n"), getTypeName(m_actionType));
      output.setLeftMargin(2);
      for(ConstIterator<IndexArrayEntry<RawActionArray>> it = raaArray.getIterator(); it.hasNext();) {
        const IndexArrayEntry<RawActionArray> &e       = it.next();
        String                                 comment = format(_T("%3u %s"), e.m_count, e.getComment().cstr());
        const UINT                             n       = (UINT)e.m_key.size();
        UINT                                   counter = 0;
        for(ConstIterator<short> it1 = e.m_key.getIterator(); it1.hasNext(); counter++, delim=',') {
          output.printf(_T("%c%4d"), delim,it1.next());
          if((counter % 20 == 19) && (counter != n - 1)) {
            newLine(output, comment, 108);
          }
        }
        newLine(output, comment, 108);
        tableSize += n;
      }
      const ByteCount byteCount = wordAlignedSize(tableSize * getTypeSize(m_actionType));
      output.setLeftMargin(0);
      output.printf(_T("}; // Size of table:%s.\n\n"), byteCount.toString().cstr());
      totalByteCount += byteCount;
    }
  }
  return totalByteCount;
}

ByteCount GrammarTables::printActionMatrixCpp(MarginFile &output) const {
  const UINT  stateCount = getStateCount();
  StringArray defines(stateCount);
  ByteCount   byteCount = printCompressedActionsCpp( output, defines);
  byteCount += printUncompressedActionMatrixCpp(     output, defines);

  if(defines.size() > 0) {
    defines.sort(stringCmp);
    for(ConstIterator<String> it = defines.getIterator(); it.hasNext();) {
      output.printf(_T("#define %s\n"), it.next().cstr());
    }
    output.printf(_T("\n"));
  }

  output.setLeftMargin(0);
  output.printf(_T("static const unsigned int actionCode[%u] = {\n"), stateCount);
  output.setLeftMargin(2);
  TCHAR delim = ' ';
  for(UINT state = 0; state < stateCount; state++, delim=',') {
    output.printf(_T("%c_ac%04d"), delim, state);
    if((state % 10 == 9) || (state == stateCount-1)) {
      output.printf(_T("\n"));
    }
  }
  output.setLeftMargin(0);
  const ByteCount codeByteCount = wordAlignedSize(stateCount*sizeof(UINT));
  output.printf(_T("}; // Size of table:%s.\n\n"), codeByteCount.toString().cstr());
  return byteCount + codeByteCount;
}

// ---------------------------------------- Successor tables ---------------------------------------------

static int succesorArrayCmp(const SuccesorArray &a1, const SuccesorArray &a2) {
  const size_t n = a1.size();
  int          c = sizetHashCmp(n, a2.size());
  if(c) return c;
  for(const unsigned short *v1 = &a1.first(), *v2 = &a2.first(), *end = v1+n; v1 < end;) {
    c = shortHashCmp(*(v1++), *(v2++));
    if(c) return c;
  }
  return 0;
}

class SuccessorArrayIndexMap : public IndexMap<SuccesorArray> {
public:
  SuccessorArrayIndexMap() : IndexMap<SuccesorArray>(succesorArrayCmp) {
  }
};

typedef IndexArray<SuccesorArray> SuccesorArrayIndexArray;

ByteCount GrammarTables::printSuccessorMatrixCpp(MarginFile &output) const {
  output.printf(_T("%s"), comment2);
  const UINT  stateCount = getStateCount();
  StringArray defines(stateCount);
  BitSet      definedStateSet(stateCount);

  SymbolSetIndexMap      ntSetMap;
  SuccessorArrayIndexMap saMap;
  UINT                   currentNTListSize = 0;
  UINT                   currentSAListSize = 0;
  ByteCount              totalByteCount;

  for(UINT state = 0; state < stateCount; state++) {
    const ActionArray &succList = m_stateSucc[state];
    const UINT succCount = (UINT)succList.size();
    if(succCount == 0) {
      continue;
    }
    definedStateSet.add(state);
    if(succCount == 1) {
      const ParserAction &pa = succList[0];
      const UINT          NT           = pa.m_token;
      const UINT          newState     = pa.m_action;
      const String        comment      = format(_T("Goto %4u on %s"), newState, getSymbolName(NT));
      const UINT          NTIndex      = NT - m_terminalCount;
      CHECKMAX15BITS(newState);
      const UINT          encodedValue = ((newState << 17) | NTIndex);
      assert((compressedBit & encodedValue) == 0);
      const String        macroValue   = format(_T("0x%08x"), compressedBit | encodedValue);
      defines.add(format(_T("_su%04u %-10s /* %-40s*/"), state, macroValue.cstr(), comment.cstr()));
    } else {
      const SymbolSet   ntSet  = getNTOffsetSet(state);
      IndexMapValue    *imvp   = ntSetMap.get(ntSet);
      UINT              ntIndex, ntCount;

      if(imvp != nullptr) {
        ntIndex = imvp->m_arrayIndex;
        ntCount = imvp->m_count;
        imvp->addState(state);
      } else {
        ntIndex = currentNTListSize;
        ntCount = (UINT)ntSetMap.size();
        IndexMapValue nv(stateCount, state, ntIndex);
        nv.m_count = ntCount;
        ntSetMap.put(ntSet, nv);
        currentNTListSize += (UINT)ntSet.size() + 1;
      }

      const SuccesorArray  sa   = getSuccessorArray(state);
      imvp                      = saMap.get(sa);
      UINT                 saIndex, saCount;
      if(imvp != nullptr) {
        saIndex = imvp->m_arrayIndex;
        saCount = imvp->m_count;
        imvp->addState(state);
      } else {
        saIndex = currentSAListSize;
        saCount = (UINT)saMap.size();
        IndexMapValue nv(stateCount, state, saIndex);
        nv.m_count = saCount;
        saMap.put(sa, nv);
        currentSAListSize += (UINT)sa.size();
      }
      CHECKMAX15BITS(saIndex);
      const UINT   encodedValue = ((UINT)saIndex << 17) | ntIndex;
      assert((encodedValue & compressedBit) == 0);
      const String macroValue = format(_T("0x%08x"), encodedValue);
      const String comment    = format(_T("NTindexList %3u, stateList %3u"), ntCount, saCount);
      defines.add(format(_T("_su%04u %-10s /* %-40s*/"), state, macroValue.cstr(), comment.cstr()));
    }
  }

  if(currentNTListSize == 0) {
    output.printf(_T("#define NTindexListTable nullptr\n"));
    output.printf(_T("#define stateListTable   nullptr\n\n"));
  } else {
    { const SymbolSetIndexArray ntSetArray = ntSetMap.getEntryArray();
      ntSetMap.clear();

      UINT       tableSize = 0;
      TCHAR      delim     = ' ';
      output.printf(_T("static const %s NTindexListTable[] = {\n"), getTypeName(m_NTIndexType));
      for(ConstIterator<IndexArrayEntry<SymbolSet> > it = ntSetArray.getIterator(); it.hasNext();) {
        const IndexArrayEntry<SymbolSet> &e       = it.next();
        String                            comment = format(_T(" %3u %s"), e.m_count, e.getComment().cstr());
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
      const ByteCount byteCount = wordAlignedSize(tableSize * getTypeSize(m_NTIndexType));
      output.setLeftMargin(0);
      output.printf(_T("}; // Size of table:%s.\n\n"), byteCount.toString().cstr());
      totalByteCount += byteCount;
    }

    { const SuccesorArrayIndexArray saArray  = saMap.getEntryArray();
      saMap.clear();

      UINT       tableSize = 0;
      TCHAR      delim     = ' ';
      output.printf(_T("static const %s stateListTable[] = {\n"), getTypeName(m_stateType));
      output.setLeftMargin(2);
      for(ConstIterator<IndexArrayEntry<SuccesorArray> > it = saArray.getIterator(); it.hasNext();) {
        const IndexArrayEntry<SuccesorArray> &e       = it.next();
        String                                comment = format(_T("%3u %s"), e.m_count, e.getComment().cstr());
        const UINT                            n       = (UINT)e.m_key.size();
        UINT                                  counter = 0;
        for(ConstIterator<unsigned short> it1 = e.m_key.getIterator(); it1.hasNext(); counter++, delim=',') {
          output.printf(_T("%c%4u"), delim, it1.next());
          if((counter % 20 == 19) && (counter != n - 1)) {
            newLine(output, comment, 108);
          }
        }
        newLine(output, comment, 108);
        tableSize += n;
      }
      const ByteCount byteCount = wordAlignedSize(tableSize * getTypeSize(m_stateType));
      output.setLeftMargin(0);
      output.printf(_T("}; // Size of table:%s.\n\n"), byteCount.toString().cstr());
      totalByteCount += byteCount;
    }
  }

  if(defines.size() > 0) {
    defines.sort(stringCmp);
    for(ConstIterator<String> it = defines.getIterator(); it.hasNext();) {
      output.printf(_T("#define %s\n"), it.next().cstr());
    }
    output.printf(_T("\n"));
  }
  output.printf(_T("#define nil (unsigned int)-1\n"));
  output.setLeftMargin(0);
  output.printf(_T("static const unsigned int successorCode[%u] = {\n"), stateCount);
  output.setLeftMargin(2);

  TCHAR delim     = ' ';
  for(UINT state = 0; state < stateCount; state++, delim=',') {
    if(!definedStateSet.contains(state)) {
      output.printf(_T("%c%-7s"), delim, _T("nil"));
    } else {
      output.printf(_T("%c_su%04d"), delim, state);
    }
    if((state % 10 == 9) || (state == stateCount-1)) {
      output.printf(_T("\n"));
    }
  }
  output.setLeftMargin(0);
  const ByteCount codeByteCount = wordAlignedSize(stateCount*sizeof(UINT));
  output.printf(_T("}; // Size of table:%s.\n\n"), codeByteCount.toString().cstr());
  return totalByteCount + codeByteCount;
}

ByteCount GrammarTables::printProductionLengthTableCpp(MarginFile &output) const {
  output.printf(_T("%s"), comment3);

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
  output.printf(_T("%s"), comment4);

  const UINT productionCount = getProductionCount();
  output.printf(_T("static const %s leftSideTable[%u] = {\n"), getTypeName(m_NTIndexType), productionCount);
  output.setLeftMargin(2);

  for(UINT p = 0; p < productionCount; p++) {
    const int l = m_left[p] - m_terminalCount;
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

  const ByteCount byteCount = wordAlignedSize(productionCount * getTypeSize(m_NTIndexType));
  output.printf(_T("\n}; // Size of table:%s.\n\n"), byteCount.toString().cstr());
  return byteCount;
}

ByteCount GrammarTables::printRightSideTableCpp(MarginFile &output) const {
  output.printf(_T("%s"), comment5);
  const UINT productionCount = getProductionCount();
  UINT       totalItemCount  = 0;

  for(UINT p = 0; p < productionCount; p++) {
    totalItemCount += (UINT)m_rightSide[p].size();
  }
  TCHAR *delim = _T(" ");
  output.printf(_T("static const %s rightSideTable[%u] = {\n"), getTypeName(m_symbolType), totalItemCount);
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

  const ByteCount byteCount = wordAlignedSize(totalItemCount * getTypeSize(m_symbolType));
  output.printf(_T("}; // Size of table:%s.\n\n"), byteCount.toString().cstr());
  return byteCount;
}

ByteCount GrammarTables::printSymbolNameTableCpp(MarginFile &output) const {
  output.printf(_T("%s"), comment6);

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
  output.printf(_T("}; // Total size of string:%s\n\n")
               ,stringByteCount.toString().cstr());

  return stringByteCount;
}

void GrammarTables::newLine(MarginFile &output, String &comment, int minColumn) const {
  if(comment.length() > 0) {
    if(minColumn > 0) {
      const int fillerSize = minColumn - output.getCurrentLineLength();
      if(fillerSize > 0) {
        output.printf(_T("%*s"), fillerSize, EMPTYSTRING);
      }
    }
    output.printf(_T(" /* %s */\n"), comment.cstr());
    comment = EMPTYSTRING;
  } else {
    output.printf(_T("\n"));
  }
}

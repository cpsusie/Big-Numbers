#include "stdafx.h"
#include <Comparator.h>
#include "GrammarCode.h"

static const TCHAR *comment1 =
_T("/**********************************************************************************\\\n"
   "* The 4 arrays actionCode, termListTable, actionstateListTable, compressedLAsets   *\n"
   "* holds a compressed action-matrix, used by LRParser to find                       *\n"
   "* action = getAction(S,T), where S is current state, T is next terminal on input   *\n"
   "*                                                                                  *\n"
   "* The interpretation of action is:                                                 *\n"
   "*   action <  0 - Reduce by production p, p == -action.                            *\n"
   "*   action == 0 - Accept. Reduce by production 0.                                  *\n"
   "*   action >  0 - Shift to newstate (=action),                                     *\n"
   "*                 ie. push(newstate), set current state=newstate                   *\n"
   "*                 and advance input 1 symbol.                                      *\n"
   "*   action == _ParserError - Unexpected input. Do some recovery, to try to         *\n"
   "*                 synchronize input and stack, in order to continue parse.         *\n"
   "*                 (See LRParser::recover() in LRParser.cpp)                        *\n"
   "*                                                                                  *\n"
   "* For each state S, a #define is generated and used as element S in array          *\n"
   "* actionCode. Each define looks as:                                                *\n"
   "*                                                                                  *\n"
   "* #define _acDDDD Code                                                             *\n"
   "*                                                                                  *\n"
   "* where DDDD is the statenumber S and Code is an unsigned int with the following   *\n"
   "* format:                                                                          *\n"
   "*                   0         1         2         3                                *\n"
   "* Bit index:        01234567890123456789012345678901                               *\n"
   "* Code     :        tttttttttttttttIFaaaaaaaaaaaaaaa                               *\n"
   "*                                                                                  *\n"
   "* t          : Bit[ 0-14]: unsigned short                                          *\n"
   "* a          : Bit[17-31]: signed short                                            *\n"
   "* F          : Bit 16    : Indicates how to interpret t and a.                     *\n"
   "* I          : Bit 15    : In case F==1, indicates how to interpret t.             *\n"
   "*                                                                                  *\n"
   "* F == 0: Uncompressed Format.                                                     *\n"
   "*      t: Index into array termListTable, pointing at the first element of         *\n"
   "*         termList (see below).                                                    *\n"
   "*      a: Index into array actionListTable, pointing at the first element of       *\n"
   "*         actionList (see below).                                                  *\n"
   "* F == 1: Compressed Format, used if there is only 1 possible action, a.           *\n"
   "*         I==0: There is only 1 legal terminal in the state.                       *\n"
   "*            t: Legal terminal.                                                    *\n"
   "*            a: Action.                                                            *\n"
   "*                                                                                  *\n"
   "*         I==1: All actions in the state are reduce by the same production P = -a. *\n"
   "*            t: Index into compressedLAsets, pointing at the first element of      *\n"
   "*               LASet (see below).                                                 *\n"
   "*            a: Action.                                                            *\n"
   "*                                                                                  *\n"
   "* F == 0: Use arrays termListTable and actionListTable to find action.             *\n"
   "*      n                 : termListTable[t] = number of elements in termList.      *\n"
   "*      termList[0..n-1]  : termListTable[t+1..t+n]                                 *\n"
   "*                          Ordered list of legal terminals                         *\n"
   "*      actionList[0..n-1]: actionListTable[a..a+n-1] (same length as termList).    *\n"
   "*                                                                                  *\n"
   "*      To get action, find index k in termList, so termList[k] == T                *\n"
   "*      and set action = actionList[k]. If T is not found, set action = _ParseError.*\n"
   "*      Note that both termList and actionList may be shared by several states.     *\n"
   "*                                                                                  *\n"
   "* F == 1 and I==1: Use array compressedLAsets which is a list of bitsets, each     *\n"
   "*                  with terminalCount bits, a lookaheadset LAset with 1-bits       *\n"
   "*                  for legal terminals, and 0-bits for illegal terminals.          *\n"
   "*                                                                                  *\n"
   "*      b                 : Number of bytes in each LAset = (terminalCount-1)/8+1   *\n"
   "*      LAset[0..b-1]     : compressedLAsets[t..t+b-1]                              *\n"
   "*                                                                                  *\n"
   "*      As for uncompressed states, the same check for existence is done.           *\n"
   "*      If terminal T is not present in LAset, set action = _ParseError.            *\n"
   "*      Note that each LAset may be shared by several states.                       *\n"
   "\\**********************************************************************************/\n");

static const TCHAR *comment2 =
_T("/**********************************************************************************\\\n"
   "* The 3 arrays NTindexListTable, stateListTable and successorCode holds a          *\n"
   "* compressed succesor-matrix, used by LRParser to find newstate = successor(S,A)   *\n"
   "* as last part of a reduction with production P, A -> alfa.                        *\n"
   "* A reduction by production P goes as follows:                                     *\n"
   "*   Pop L elements from stack, where L = length of alfa;                           *\n"
   "*   S = state on stacktop;                                                         *\n"
   "*   A = leftside of the reduce production P;                                       *\n"
   "*   newstate = successor(S,A);                                                     *\n"
   "*   push(newstate), and set current state = newstate.                              *\n"
   "*                                                                                  *\n"
   "* For each relevant state S, a #define is generated and used as element S in array *\n"
   "* successorCode. Each define has the format:                                       *\n"
   "*                                                                                  *\n"
   "* #define _suDDDD Code                                                             *\n"
   "*                                                                                  *\n"
   "* where DDDD is the statenumber S and Code is an unsigned int with the following   *\n"
   "* format:                                                                          *\n"
   "*                   0         1         2         3                                *\n"
   "* Bit index:        01234567890123456789012345678901                               *\n"
   "* Code              iiiiiiiiiiiiiiiiFrrrrrrrrrrrrrrr                               *\n"
   "*                                                                                  *\n"
   "* i          : Bit[ 0-15]: unsigned short                                          *\n"
   "* r          : Bit[17-31]: unsigned short                                          *\n"
   "* F          : Bit 16    : Indicates how to interpret i and r.                     *\n"
   "*                                                                                  *\n"
   "* F == 0: Uncompressed Format.                                                     *\n"
   "*      i: Index into array NTindexListTable, pointing at the first element of      *\n"
   "*         NTIndexList (see below).                                                 *\n"
   "*      r: Index into array stateListTable, pointing at the first element of        *\n"
   "*         stateList (see below).                                                   *\n"
   "* F == 1: Compressed Format, used if there is only 1 possible newstate.            *\n"
   "*      i: Index A' of nonterminal A, A' = (A - terminalCount).                     *\n"
   "*      r: New state.                                                               *\n"
   "*                                                                                  *\n"
   "* F == 0: Use arrays NTIndexListTable and stateListTable to find newstate.         *\n"
   "*      n                  : NTIndexListTable[i] = number of elements in NTIndexList*\n"
   "*      NTIndexList[0..n-1]: NTIndexListTable[i+1..i+n]                             *\n"
   "*                           Ordered list of possible nonterminal-indices.          *\n"
   "*      stateList[0..n-1]  : stateListTable[r..r+n-1], same length as NTIndexList   *\n"
   "*                                                                                  *\n"
   "*      To get newstate, find index k in NTIndexList, so NTIndexList[k] == A',      *\n"
   "*      and set newstate = stateList[k].                                            *\n"
   "*      A' = (A - terminalCount) will always exist.                                 *\n"
   "*      Note that both NTIndexList and stateList may be shared by several states.   *\n"
   "\\**********************************************************************************/\n");

static const TCHAR *comment3 =
_T("/**********************************************************************************\\\n"
   "* The productionLength[] is indexed by production number and holds the number of   *\n"
   "* symbols on the right side of each production.                                    *\n"
   "\\**********************************************************************************/\n");

static const TCHAR *comment4 =
_T("/**********************************************************************************\\\n"
   "* leftSideTable[] is indexed by production number.                                 *\n"
   "* leftSideTable[p] = A', A' = (A - terminalCount)                                  *\n"
   "*                        where A is the left side of production p.                 *\n"
   "* A' = 0..nonterminalCount-1.                                                      *\n"
   "* p  = 0..productionCount-1                                                        *\n"
   "\\**********************************************************************************/\n");

static const TCHAR *comment5 =
_T("/**********************************************************************************\\\n"
   "* rightSideTable[] holds a compressed form of the rightsides of all                *\n"
   "* productions in the grammar. Only used for debugging.                             *\n"
   "\\**********************************************************************************/\n");

static const TCHAR *comment6 =
_T("/**********************************************************************************\\\n"
   "* symbolNames is a space separated string with the names of all symbols used in    *\n"
   "* grammar, terminals and nonTerminals. Only used for debugging.                    *\n"
   "\\**********************************************************************************/\n");

void GrammarTables::printCpp(MarginFile &output, bool useTableCompression) const {
  m_countTableBytes.reset();

  if(!useTableCompression) {
    m_compressibleStateSet.clear();
  }
  findTemplateTypes();
  m_countTableBytes += printActionMatrixCpp(             output);
  m_countTableBytes += printSuccessorMatrixCpp(          output);
  m_countTableBytes += printProductionLengthTableCpp(    output);
  m_countTableBytes += printLeftSideTableCpp(            output);
  m_countTableBytes += printRightSideTableCpp(           output);
  m_countTableBytes += printSymbolNameTableCpp(          output);
//  printf("sizeof parserTableTemplate:%zu bytes", sizeof(ParserTablesTemplate<2,2,2,2,char,char,char,char,char>));
  const int sizeofTableTemlatex86  = 68;  // sizeof(ParserTablesTemplate) x86
  const int sizeofTableTemplatex64 = 128; // sizeof(ParserTablesTemplate) x64

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
  output.printf(_T("actionCode      , termListTable     , actionListTable, compressedLAsets\n"
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

static int stringCmp(const String &s1, const String &s2) {
  return _tcscmp(s1.cstr(), s2.cstr());
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

// ---------------------------------------- Compressed Action Matrix ---------------------------------------------

ByteCount GrammarTables::printActionMatrixCpp(MarginFile &output) const {
  const UINT  stateCount = getStateCount();
  StringArray defines(stateCount);
  ByteCount   totalByteCount;

  output.printf(_T("%s"), comment1);

  tostrstream tmpOutput;
  MarginFile  tmpMarginFile(tmpOutput);

  totalByteCount += printUncompressedActionMatrixCpp(   tmpMarginFile, defines);
  totalByteCount += printCompressedActionsCpp(          tmpMarginFile, defines);

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
    if((state % 10 == 9) && (state != stateCount-1)) {
      output.printf(_T("\n"));
    }
  }
  output.setLeftMargin(0);
  const ByteCount byteCount = wordAlignedSize(stateCount*sizeof(UINT));
  output.printf(_T("\n}; // Size of table:%s.\n\n"), byteCount.toString().cstr());
  totalByteCount += byteCount;
  tmpMarginFile.close();
  output.puts(tmpOutput.str().c_str());
  return totalByteCount;
}


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
  UINT getElementCount(bool addArraySize) const {
    UINT elemCount = 0;
    for(ConstIterator<IndexArrayEntry<Key> > it = getIterator(); it.hasNext();) {
      const IndexArrayEntry<Key> &e = it.next();
      elemCount += (UINT)e.m_key.size();
    }
    return elemCount + (addArraySize ? (UINT)size() : 0);
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
      nv.m_count = raCount;
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
      output.printf(_T("static const %s termListTable[%u] = {\n"), getTypeName(m_terminalType), laSetArray.getElementCount(true));
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
      output.printf(_T("static const %s actionListTable[%u] = {\n"), getTypeName(m_actionType), raaArray.getElementCount(false));
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

// ---------------------------------------- Compressed actions ---------------------------------------------

/************************************\
 * 0         1         2         3   *
 * 01234567890123456789012345678901  *
 * tttttttttttttttIFaaaaaaaaaaaaaaa  *
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
                                         ? format(_T("Reduce by %u on %s"), -action, getSymbolName(pa.m_token))
                                         : format(_T("Shift  to %u on %s"),  action, getSymbolName(pa.m_token));
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


// ---------------------------------------- Successor Matrix ---------------------------------------------

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

  tostrstream tmpOutput;
  MarginFile  output1(tmpOutput);

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
      const String        comment      = format(_T("Goto %u on %s"), newState, getSymbolName(NT));
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
    output1.printf(_T("#define NTindexListTable nullptr\n"));
    output1.printf(_T("#define stateListTable   nullptr\n\n"));
  } else {
    { const SymbolSetIndexArray ntSetArray = ntSetMap.getEntryArray();
      ntSetMap.clear();
      
      UINT       tableSize = 0;
      TCHAR      delim     = ' ';
      output1.printf(_T("static const %s NTindexListTable[%u] = {\n"), getTypeName(m_NTIndexType), ntSetArray.getElementCount(true));
      for(ConstIterator<IndexArrayEntry<SymbolSet> > it = ntSetArray.getIterator(); it.hasNext();) {
        const IndexArrayEntry<SymbolSet> &e       = it.next();
        String                            comment = format(_T(" %3u %s"), e.m_count, e.getComment().cstr());
        const UINT                        n       = (UINT)e.m_key.size();
        UINT                              counter = 0;
        output1.setLeftMargin(2);
        output1.printf(_T("%c%3u"), delim, n); delim = ',';
        output1.setLeftMargin(6);
        for(ConstIterator<size_t> it1 = e.m_key.getIterator(); it1.hasNext(); counter++) {
          output1.printf(_T(",%4zu"), it1.next());
          if((counter % 20 == 19) && (counter != n - 1)) {
            newLine(output1, comment, 108);
          }
        }
        newLine(output1, comment, 108);
        tableSize += n + 1;
      }
      const ByteCount byteCount = wordAlignedSize(tableSize * getTypeSize(m_NTIndexType));
      output1.setLeftMargin(0);
      output1.printf(_T("}; // Size of table:%s.\n\n"), byteCount.toString().cstr());
      totalByteCount += byteCount;
    }

    { const SuccesorArrayIndexArray saArray  = saMap.getEntryArray();
      saMap.clear();

      UINT       tableSize = 0;
      TCHAR      delim     = ' ';
      output1.printf(_T("static const %s stateListTable[%u] = {\n"), getTypeName(m_stateType), saArray.getElementCount(false));
      output1.setLeftMargin(2);
      for(ConstIterator<IndexArrayEntry<SuccesorArray> > it = saArray.getIterator(); it.hasNext();) {
        const IndexArrayEntry<SuccesorArray> &e       = it.next();
        String                                comment = format(_T("%3u %s"), e.m_count, e.getComment().cstr());
        const UINT                            n       = (UINT)e.m_key.size();
        UINT                                  counter = 0;
        for(ConstIterator<unsigned short> it1 = e.m_key.getIterator(); it1.hasNext(); counter++, delim=',') {
          output1.printf(_T("%c%4u"), delim, it1.next());
          if((counter % 20 == 19) && (counter != n - 1)) {
            newLine(output1, comment, 108);
          }
        }
        newLine(output1, comment, 108);
        tableSize += n;
      }
      const ByteCount byteCount = wordAlignedSize(tableSize * getTypeSize(m_stateType));
      output1.setLeftMargin(0);
      output1.printf(_T("}; // Size of table:%s.\n\n"), byteCount.toString().cstr());
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
    if((state % 10 == 9) && (state != stateCount-1)) {
      output.printf(_T("\n"));
    }
  }
  output.setLeftMargin(0);
  const ByteCount byteCount = wordAlignedSize(stateCount*sizeof(UINT));
  output.printf(_T("\n}; // Size of table:%s.\n\n"), byteCount.toString().cstr());
  totalByteCount += byteCount;
  output1.close();
  output.puts(tmpOutput.str().c_str());

  return totalByteCount;
}

ByteCount GrammarTables::printProductionLengthTableCpp(MarginFile &output) const {
  output.printf(_T("%s"), comment3);

  const UINT productionCount = getProductionCount();

  output.printf(_T("static const unsigned char productionLength[%u] = {\n"), productionCount);
  output.setLeftMargin(2);
  TCHAR delim = ' ';
  for(UINT p = 0; p < productionCount; p++, delim = ',') {
    const UINT l = m_productionLength[p];
    if(p % 10 == 0) {
      output.printf(_T("/* %3u */ "), p);
    }
    output.printf(_T("%c%3u"), delim, l);
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

  TCHAR delim = ' ';
  for(UINT p = 0; p < productionCount; p++, delim = ',') {
    const int l = m_left[p] - m_terminalCount;
    if(p % 10 == 0) {
      output.printf(_T("/* %3u */ "), p);
    }
    output.printf(_T("%c%3u"), delim, l);
    if((p % 10 == 9) == (p != productionCount)) {
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
  output.printf(_T("static const %s rightSideTable[%u] = {\n"), getTypeName(m_symbolType), totalItemCount);
  output.setLeftMargin(2);
  TCHAR delim = ' ';
  for(UINT p = 0; p < productionCount; p++) {
    const CompactIntArray &r = m_rightSide[p];
    if(r.size() == 0) {
      continue;
    }
    output.printf(_T("/* %3u */ "), p);
    for(size_t i = 0; i < r.size(); i++, delim = ',') {
      output.printf(_T("%c%3d"), delim, r[i]);
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
    output.printf(s ? _T("\" ") : _T("\""));
    const int    l       = (int)name.length()+(s?1:0), fillerLen = minMax(50 - l, 0, 50);
    const String comment = (s < m_terminalCount)
                         ? format(_T("T  %4u"), s)
                         : format(_T("NT %4u NTindex=%u"), s, s-m_terminalCount);
    output.printf(_T("%s\"%*s/* %-21s */\n"), name.cstr(), fillerLen,_T(""),comment.cstr());
    charCount += l;
  }
  output.setLeftMargin(0);

  const ByteCount stringByteCount = wordAlignedSize(sizeof(char)*((UINT)charCount+1));
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

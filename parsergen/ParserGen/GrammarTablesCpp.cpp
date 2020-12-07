#include "stdafx.h"
#include <Comparator.h>
#include "GrammarCode.h"
#include "CompressedTransShiftMatrixCpp.h"
#include "CompressedTransSuccMatrixCpp.h"

static const TCHAR *comment1 =
_T("/************************************************************************************\\\n"
   "* The 4 arrays actionCodeArray, termArrayTable, actionArrayTable and termBitSetTable *\n"
   "* holds a compressed action-matrix, used by LRParser to find                         *\n"
   "* action = getAction(S,T), where S is current state, T is next terminal on input     *\n"
   "*                                                                                    *\n"
   "* The interpretation of action is:                                                   *\n"
   "*   action <  0 - Reduce by production p, p == -action.                              *\n"
   "*   action == 0 - Accept. Reduce by production 0.                                    *\n"
   "*   action >  0 - Shift to newstate (=action),                                       *\n"
   "*                 ie. push(newstate), set current state=newstate                     *\n"
   "*                 and advance input 1 symbol.                                        *\n"
   "*   action == _ParserError - Unexpected input. Do some recovery, to try to           *\n"
   "*                 synchronize input and stack, in order to continue parse.           *\n"
   "*                 (See LRParser::recover() in LRParser.cpp)                          *\n"
   "*                                                                                    *\n"
   "* For each state S, a #define is generated and used as element S in array            *\n"
   "* actionCode. Each define looks as:                                                  *\n"
   "*                                                                                    *\n"
   "* #define _acDDDD Code                                                               *\n"
   "*                                                                                    *\n"
   "* where DDDD is the statenumber S and Code is an unsigned int with the following     *\n"
   "* format:                                                                            *\n"
   "*            0         1         2         3                                         *\n"
   "* Bit index: 01234567890123456789012345678901                                        *\n"
   "* Code     : tttttttttttttttCCaaaaaaaaaaaaaaa                                        *\n"
   "*                                                                                    *\n"
   "* t        : Bit[ 0-14] : unsigned short                                             *\n"
   "* a        : Bit[17-31] : signed short                                               *\n"
   "* CC       : Bit[15-16] : Indicates how to interpret t and a:                        *\n"
   "*                                                                                    *\n"
   "* CC==0: CompCodeBinSearch                                                           *\n"
   "*     t: Index into array termArrayTable, pointing at the first element of           *\n"
   "*        termArray                                                                   *\n"
   "*     a: Index into array actionArrayTable, pointing at the first element of         *\n"
   "*        actionArray                                                                 *\n"
   "*                                                                                    *\n"
   "*     n                  : termArrayTable[t] = number of elements in termArray.      *\n"
   "*     termArray[0..n-1]  : termArrayTable[t+1..t+n]                                  *\n"
   "*                          Ordered list of legal terminal symbols                    *\n"
   "*     actionArray[0..n-1]: actionArrayTable[a..a+n-1] (same length as termArray).    *\n"
   "*                                                                                    *\n"
   "*     To get action, find index k in termArray, so termArray[k] == T,k=[0..n-1]      *\n"
   "*     and set action = actionArray[k].                                               *\n"
   "*     If T is not found, set action = _ParseError.                                   *\n"
   "*     Note that both termArray and actionArray may be shared by several states.      *\n"
   "*                                                                                    *\n"
   "* CC==1: CompCodeSplitNode                                                           *\n"
   "*     t and a are both indices to 2 child entries in actionCodeArray, which can be   *\n"
   "*     another _acNNNN or an extra node, _asNNNN, whichever is needed (values are     *\n"
   "*     reused as much as possible, ie. if _asNNNN equals some _acNNNN then no         *\n"
   "*     _asNNNN is added, but parent entry will point to _acNNNN instead.              *\n"
   "*     Recursive tree search, with max-recursion level specified in                   *\n"
   "*     parsergen +c<level> option. Search childnodes until an action != _ParseError   *\n"
   "*     is returned, or no more nodes to search.                                       *\n"
   "*                                                                                    *\n"
   "* CC==2: CompCodeImmediate, used if there is only 1 legal terminal in the state      *\n"
   "*     t: Legal terminal.                                                             *\n"
   "*     a: Action.                                                                     *\n"
   "*     If terminal T != t, set action = _ParseError, else use a as action             *\n"
   "*                                                                                    *\n"
   "* CC==3: CompCodeBitSet (always reduce by same reduce production P = -a)             *\n"
   "*     t: Index into termBitSetTable, pointing at the first element of termBitSet     *\n"
   "*     a: Action.                                                                     *\n"
   "*                                                                                    *\n"
   "*     termBitSetTable is an array of termBitSet, each with same number of bits,      *\n"
   "*     capacity, 1-bits for legal terminals, 0-bits for illegal terminals.            *\n"
   "*                                                                                    *\n"
   "*     b                  : Number of bytes in each termBitSet=(capacity-1)/8+1       *\n"
   "*     termBitSet[0..b-1] : termBitSetTable[t..t+b-1]                                 *\n"
   "*                                                                                    *\n"
   "*     The value of capacity is minimized, capacity <= termCount                      *\n"
   "*     As for other node types, the same check for existence is done. If terminal T   *\n"
   "*     is not present in termBitSet, or T >= capacity, set action = _ParseError.      *\n"
   "*     Note that each termBitSet may be shared by several states.                     *\n"
   "\\************************************************************************************/\n\n");

static const TCHAR *comment2 =
_T("/************************************************************************************\\\n"
   "* The 3 arrays successorCodeArray, ntIndexArrayTable , newStateArrayTable            *\n"
   "* compressed succesor-matrix, used by LRParser to find newstate = successor(S,A)     *\n"
   "* as last part of a reduction with production P, A -> alfa.                          *\n"
   "* A reduction by production P goes as follows:                                       *\n"
   "*   Pop L elements from stack, where L = length of alfa;                             *\n"
   "*   S = state on stacktop;                                                           *\n"
   "*   A = leftside of the reduce production P;                                         *\n"
   "*   newstate = successor(S,A);                                                       *\n"
   "*   push(newstate), and set current state = newstate.                                *\n"
   "*                                                                                    *\n"
   "* For each relevant state S, a #define is generated and used as element S in array   *\n"
   "* successorCodeArray. Each define has the format:                                    *\n"
   "*                                                                                    *\n"
   "* #define _scDDDD Code                                                               *\n"
   "*                                                                                    *\n"
   "* where DDDD is the statenumber S and Code is an unsigned int with the following     *\n"
   "* format:                                                                            *\n"
   "*            0         1         2         3                                         *\n"
   "* Bit index: 01234567890123456789012345678901                                        *\n"
   "* Code       iiiiiiiiiiiiiiiCCsssssssssssssss                                        *\n"
   "*                                                                                    *\n"
   "* i          : Bit[ 0-14]  : unsigned short                                          *\n"
   "* s          : Bit[17-31]  : unsigned short                                          *\n"
   "* CC         : Bit[15-16]  : Indicates how to interpret i and s.                     *\n"
   "*                                                                                    *\n"
   "* CC has the same meaning as for actionCodeArray, but only CC={0,2} are used.        *\n"
   "* CC==0: CompCodeBinSearch                                                           *\n"
   "*     i: Index into array ntIndexArrayTable, pointing at the first element of        *\n"
   "*        ntIndexArray                                                                *\n"
   "*     s: Index into array newStateArrayTable, pointing at the first element of       *\n"
   "*        newStateArray                                                               *\n"
   "*                                                                                    *\n"
   "*     n                    : ntIndexArrayTable[i]=number of elements in ntIndexArray *\n"
   "*     ntIndexArray[0..n-1] : ntIndexArrayTable[i+1..i+n]                             *\n"
   "*                            Ordered list of possible nonterminal-indices.           *\n"
   "*     newStateArray[0..n-1]: newStateArrayTable[s..s+n-1], length = n                *\n"
   "*                                                                                    *\n"
   "*     To get newstate, find k so ntIndexArray[k] == A', k=[0..n-1] and set           *\n"
   "*     newstate = newStateArray[k].                                                   *\n"
   "*     A' = (A - termCount) will always exist.                                        *\n"
   "*     Note that both ntIndexArray and newStateArray may be shared by several states. *\n"
   "* CC==2: CompCodeImmediate, used if there is only 1 possible newstate.               *\n"
   "*     i: Index A' of nonterminal A, A' = (A - termCount).                            *\n"
   "*     s: New state.                                                                  *\n"
   "*                                                                                    *\n"
   "\\************************************************************************************/\n\n");


static const TCHAR *comment2Transposed =
_T("/************************************************************************************\\\n"
   "* The 4 arrays successorCodeArray, stateArrayTable, newStateArrayTable and           *\n"
   "* stateBitSetTable holds a compressed succesor-matrix, used by LRParser to find      *\n"
   "* newstate = successor(S,A) as last part of a reduction with production P, A -> alfa *\n"
   "* A reduction by production P goes as follows:                                       *\n"
   "*   Pop L elements from stack, where L = length of alfa;                             *\n"
   "*   S = state on stacktop;                                                           *\n"
   "*   A = leftside of the reduce production P;                                         *\n"
   "*   newstate = successor(S,A);                                                       *\n"
   "*   push(newstate), and set current state = newstate.                                *\n"
   "* Because the values of all non-terminals A = [termCount..symbolCount-1], the        *\n"
   "* value ntIndex = A' = A - termCount is used as index into successorCodeArray.       *\n"
   "* ntIndex = [0..ntermCount-1]                                                        *\n"
   "*                                                                                    *\n"
   "* For each non-terminal A, a #define is generated and used as element A' in array    *\n"
   "* successorCodeArray. Each define has the format:                                    *\n"
   "*                                                                                    *\n"
   "* #define _scDDDD Code                                                               *\n"
   "*                                                                                    *\n"
   "* where DDDD is ntIndex A' and Code is an unsigned int with the following format     *\n"
   "*                                                                                    *\n"
   "*            0         1         2         3                                         *\n"
   "* Bit index: 01234567890123456789012345678901                                        *\n"
   "* Code       sssssssssssssssCCrrrrrrrrrrrrrrr                                        *\n"
   "*                                                                                    *\n"
   "* s          : Bit[ 0-14]  : unsigned short                                          *\n"
   "* r          : Bit[17-31]  : unsigned short                                          *\n"
   "* CC         : Bit[15-16]  : Indicates how to interpret s and r.                     *\n"
   "*                                                                                    *\n"
   "* CC has the same meaning as for actionCodeArray                                     *\n"
   "* CC==0: CompCodeBinSearch                                                           *\n"
   "*     s: Index into stateArrayTable, pointing at the first element of stateArray     *\n"
   "*     r: Index into newStateArrayTable, pointing at the first element of             *\n"
   "*        newStateArray                                                               *\n"
   "*                                                                                    *\n"
   "*     n                    : stateArrayTable[s] = number of elements in stateArray   *\n"
   "*     stateArray[0..n-1]   : stateArrayTable[s+1..s+n]                               *\n"
   "*                            Ordered list of n possible states S                     *\n"
   "*     newStateArray[0..n-1]: newStateArrayTable[r..r+n-1], length = n                *\n"
   "*                                                                                    *\n"
   "*     To find returnvalue, find index k so stateArray[k]==S, k=[0..n-1]              *\n"
   "*     and return newStateArray[k]. If not found, return _ParseError (= 0xffff).      *\n"
   "*     Note that both stateArray and newStateArray may be shared by several NTindices *\n"
   "*                                                                                    *\n"
   "* CC==1: CompCodeSplitNode                                                           *\n"
   "*     s and r are both indices to 2 child entries in successorCodeArray, which can   *\n"
   "*     be another _scNNNN or an extra node, _ssNNNN, whichever is needed (see         *\n"
   "*     actionCodeArray). Recursive tree search, with max-recursion level specified in *\n"
   "*     parsergen -T,r<level> option. Search childnodes until the returned value       *\n"
   "*     != _ParseError. Note, that the search will always succeed.                     *\n"
   "*                                                                                    *\n"
   "* CC==2: CompCodeImmediate, used if there is only 1 possible newstate.               *\n"
   "*     s: State to come from. If s==0x7fff, the check S==s is skipped                 *\n"
   "*     r: New state.                                                                  *\n"
   "*                                                                                    *\n"
   "* CC==3: CompCodeBitSet (Same newState for all states present in stateBitSet)        *\n"
   "*     s: Index into stateBitSetTable, pointing at the first element of stateBitSet   *\n"
   "*     r: New state.                                                                  *\n"
   "*                                                                                    *\n"
   "*     stateBitSetTable is an array of stateBitSet, each with same number of bits,    *\n"
   "*     capacity, 1-bits for possible from-states, 0-bits for irrelevant states        *\n"
   "*                                                                                    *\n"
   "*     b                  : Number of bytes in each stateBitSet=(capacity-1)/8+1      *\n"
   "*     stateBitSet[0..b-1]: stateBitSetTable[r..r+b-1]                                *\n"
   "*                                                                                    *\n"
   "*     The value of capacity is minimized, capacity <= stateCount.                    *\n"
   "*     If state S is present in stateBitSet, return r, else return _ParseError.       *\n"
   "*     Contrary to action, the value _ParseError will never propagate all the way     *\n"
   "*     back to LRParser (unless something is completely wrong). The search for a new  *\n"
   "*     state should ALWAYS succeed in the end.                                        *\n"
   "*     Note that each stateBitSet may be shared by several NTindices.                 *\n"
   "\\************************************************************************************/\n\n");


static const TCHAR *comment3 =
_T("/************************************************************************************\\\n"
   "* The prodLengthArray[] is indexed by production number and holds the number of      *\n"
   "* symbols on the right side of each production.                                      *\n"
   "\\************************************************************************************/\n");

static const TCHAR *comment4 =
_T("/************************************************************************************\\\n"
   "* leftSideArray[] is indexed by production number.                                   *\n"
   "* leftSideArray[p] = A', A' = (A - termCount)                                        *\n"
   "*                        where A is the left side of production p.                   *\n"
   "* A' = [0..ntermCount-1]                                                             *\n"
   "* p  = [0..productionCount-1]                                                        *\n"
   "\\************************************************************************************/\n");

static const TCHAR *comment5 =
_T("/************************************************************************************\\\n"
   "* rightSideTable[] holds a compressed form of the rightsides of all productions in   *\n"
   "* the grammar. Only used for debugging.                                              *\n"
   "\\************************************************************************************/\n");

static const TCHAR *comment6 =
_T("/************************************************************************************\\\n"
   "* symbolNames is a space separated string with the names of all symbols used in the  *\n"
   "* grammar, terminals and nonTerminals. Only used for debugging.                      *\n"
   "\\************************************************************************************/\n");

void GrammarTables::printCpp(MarginFile &output) const {
  const Options &options = Options::getInstance();
  m_countTableBytes.clear();

  m_countTableBytes = printCppParserTablesTemplateTransShift(output);

  output.printf(_T("// Total size of table data:%s.\n")
               ,m_countTableBytes.toString().cstr()
               );
}

// Return size in bytes
ByteCount GrammarTables::printCppParserTablesTemplateTransShift(MarginFile &output) const {
  ByteCount byteCount;
  byteCount += printCompressedTransShiftMatrixCpp(output);
  byteCount += printProdLengthArrayCpp(           output);
  byteCount += printLeftSideArrayCpp(             output);
  byteCount += printRightSideTableCpp(            output);
  byteCount += printSymbolNamesCpp(               output);
//  printf("sizeof parserTableTemplate:%zu bytes", sizeof(ParserTablesTemplate<2,2,2,2,char,char,char,char,char>));
  const int sizeofTableTemlatex86  =  68; // sizeof(ParserTablesTemplate) x86
  const int sizeofTableTemplatex64 = 128; // sizeof(ParserTablesTemplate) x64

  const ByteCount tableClassSize(sizeofTableTemlatex86, sizeofTableTemplatex64);
  byteCount += tableClassSize;
  byteCount += ByteCount::s_pointerSize;

  const String &tablesClassName = getGrammarCode().getTablesClassName();
  const String &parserClassName = getGrammarCode().getParserClassName();

  output.printf(_T("static const ParserTablesTemplateTransShift"));
  const BitSetInterval &shiftStateBitSetInterval = m_grammar.getShiftStateBitSetInterval();
  const BitSetInterval &succStateBitSetInterval  = m_grammar.getSuccStateBitSetInterval();

  int column = output.getCurrentLineLength();
  output.setLeftMargin(column - 1);
  output.printf(_T("<%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\n,%s\n,%s\n,%s\n,%s\n,%s> %s_s(")
               ,getSymbolCount(), getTermCount(), getProductionCount(), getStateCount()
               ,byteCount.getByteCount(PLATFORM_X86),byteCount.getByteCount(PLATFORM_X64)
               ,m_grammar.getStartState()
               ,m_grammar.getTermBitSetCapacity()
               ,shiftStateBitSetInterval.getFrom(), shiftStateBitSetInterval.getCapacity()
               ,succStateBitSetInterval.getFrom() , succStateBitSetInterval.getCapacity()
               ,getTypeName(m_types.getSymbolType()    )
               ,getTypeName(m_types.getTermType()      )
               ,getTypeName(m_types.getNTIndexType()   )
               ,getTypeName(m_types.getProductionType())
               ,getTypeName(m_types.getStateType()     )
               ,tablesClassName.cstr()
               );

  column = output.getCurrentLineLength();
  output.setLeftMargin(column - 1);
  output.printf(_T("prodLengthArray   , leftSideArray\n"
                   ",rightSideTable    , symbolNames\n"
                   ",shiftCodeArray    , shiftFromStateArrayTable, shiftToStateArrayTable, shiftStateBitSetTable\n"
                   ",reduceCodeArray   , termArrayTable          , reduceArrayTable      , termBitSetTable\n"
                   ",succCodeArray     , succFromStateArrayTable , succToStateArrayTable , succStateBitSetTable\n);\n\n")
               );
  output.setLeftMargin(0);

  output.printf(_T("const AbstractParserTables *%s::%s = &%s_s;\n")
               ,parserClassName.cstr()
               ,tablesClassName.cstr()
               ,tablesClassName.cstr()
               );
  output.printf(_T("// Size of %s_s: %s. Size of %s:%s\n\n")
               ,tablesClassName.cstr(), tableClassSize.toString().cstr()
               ,tablesClassName.cstr(), ByteCount::s_pointerSize.toString().cstr()
               );
  return byteCount;
}

// return size in bytes
ByteCount GrammarTables::printCompressedTransShiftMatrixCpp(MarginFile &output) const {
  return CompressedTransShiftMatrix(m_grammar).print(output);
}

ByteCount GrammarTables::printProdLengthArrayCpp(MarginFile &output) const {
  output.printf(_T("%s"), comment3);
  const UINT productionCount = getProductionCount();
  outputBeginArrayDefinition(output, _T("prodLengthArray"), TYPE_UCHAR, productionCount);
  TCHAR delim = ' ';
  for(UINT p = 0; p < productionCount; p++, delim = ',') {
    const UINT l = getProductionLength(p);
    if(p % 10 == 0) {
      output.printf(_T("/* %3u */ "), p);
    }
    output.printf(_T("%c%3u"), delim, l);
    if(p % 10 == 9) {
      output.printf(_T("\n"));
    }
  }
  return outputEndArrayDefinition(output, TYPE_UCHAR, productionCount, true);
}

ByteCount GrammarTables::printLeftSideArrayCpp(MarginFile &output) const {
  output.printf(_T("%s"), comment4);
  const UINT productionCount = getProductionCount();
  outputBeginArrayDefinition(output, _T("leftSideArray"), m_types.getNTIndexType(), productionCount);
  TCHAR delim = ' ';
  for(UINT p = 0; p < productionCount; p++, delim = ',') {
    const int l = getLeftSymbol(p) - getTermCount();
    if(p % 10 == 0) {
      output.printf(_T("/* %3u */ "), p);
    }
    output.printf(_T("%c%3u"), delim, l);
    if((p % 10 == 9) == (p != productionCount)) {
      output.printf(_T("\n"));
    }
  }
  return outputEndArrayDefinition(output, m_types.getNTIndexType(), productionCount, true);
}

ByteCount GrammarTables::printRightSideTableCpp(MarginFile &output) const {
  output.printf(_T("%s"), comment5);
  const UINT productionCount = getProductionCount();
  UINT       itemCount       = 0;
  for(UINT p = 0; p < productionCount; p++) {
    itemCount += m_grammar.getProduction(p).getLength();
  }
  outputBeginArrayDefinition(output, _T("rightSideTable"), m_types.getSymbolType(), itemCount);
  TCHAR delim = ' ';
  for(UINT p = 0; p < productionCount; p++) {
    const CompactArray<RightSideSymbol> &rs = m_grammar.getProduction(p).m_rightSide;
    if(rs.size() == 0) {
      continue;
    }
    output.printf(_T("/* %3u */ "), p);
    for(auto s : rs) {
      output.printf(_T("%c%3u"), delim, s.m_index); delim = ',';
    }
    output.printf(_T("\n"));
  }
  return outputEndArrayDefinition(output, m_types.getSymbolType(), itemCount);
}

ByteCount GrammarTables::printSymbolNamesCpp(MarginFile &output) const {
  output.printf(_T("%s"), comment6);

  const UINT termCount   = getTermCount();
  const UINT symbolCount = getSymbolCount();
  size_t     charCount   = 0;
  output.printf(_T("static const char *symbolNames = {\n"));
  output.setLeftMargin(2);
  for(UINT s = 0; s < symbolCount; s++) {
    const String &name = getSymbolName(s);
    output.printf(s ? _T("\" ") : _T("\""));
    const int    l       = (int)name.length()+(s?1:0), fillerLen = minMax(50 - l, 0, 50);
    const String comment = (s < termCount)
                         ? format(_T("T  %4u"), s)
                         : format(_T("NT %4u ntIndex=%u"), s, s-termCount);
    output.printf(_T("%s\"%*s/* %-21s */\n"), name.cstr(), fillerLen,_T(""),comment.cstr());
    charCount += l;
  }
  output.setLeftMargin(0);

  const ByteCount byteCount = ByteCount::wordAlignedSize(sizeof(char)*(UINT)(charCount+1));
  output.printf(_T("}; // Size of string:%s\n\n"), byteCount.toString().cstr());
  return byteCount;
}

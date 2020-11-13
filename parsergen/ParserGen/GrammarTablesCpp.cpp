﻿#include "stdafx.h"
#include <Comparator.h>
#include "GrammarCode.h"
#include "CompressedActionMatrixCpp.h"

static const TCHAR *comment1 =
_T("/************************************************************************************\\\n"
   "* The 4 arrays actionCode, termListTable, actionListTable and termSetTable           *\n"
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
   "* CC == 0: CompCodeTermList (uncompressed)                                           *\n"
   "*       t: Index into array termListTable, pointing at the first element of          *\n"
   "*          termList                                                                  *\n"
   "*       a: Index into array actionListTable, pointing at the first element of        *\n"
   "*          actionList                                                                *\n"
   "*                                                                                    *\n"
   "*       n                  : termListTable[t] = number of elements in termList.      *\n"
   "*       termList[0..n-1]   : termListTable[t+1..t+n]                                 *\n"
   "*                            Ordered list of legal terminals                         *\n"
   "*       actionList[0..n-1] : actionListTable[a..a+n-1] (same length as termList).    *\n"
   "*                                                                                    *\n"
   "*       To get action, find index k in termList, so termList[k] == T,k=[0..n-1]      *\n"
   "*       and set action = actionList[k].                                              *\n"
   "*       If T is not found, set action = _ParseError.                                 *\n"
   "*       Note that both termList and actionList may be shared by several states.      *\n"
   "*                                                                                    *\n"
   "* CC == 1: CompCodeSplitNode                                                         *\n"
   "*       t and a are both indices to 2 child entries in actionCode, which can be      *\n"
   "*       another _acNNNN or an extra node, _snNNNN, whichever is needed (values are   *\n"
   "*       reused as much as possible, ie. if _snNNNN equals some _acNNNN then no       *\n"
   "*       _snNNNN is added, but parent entry will point to _acNNNN instead.            *\n"
   "*       Recursive tree search, with max-recursion level specified in                 *\n"
   "*       parsegen +c options                                                          *\n"
   "*                                                                                    *\n"
   "* CC == 2: CompCodeOneItem (Only 1 legal terminal in the state)                      *\n"
   "*       t: Legal terminal.                                                           *\n"
   "*       a: Action.                                                                   *\n"
   "*                                                                                    *\n"
   "* CC == 3: CompCodeTermSet (always reduce by same reduce production P = -a)          *\n"
   "*       t: Index into termSetTable, pointing at the first element of termSet         *\n"
   "*       a: Action.                                                                   *\n"
   "*                                                                                    *\n"
   "*       termSetTable is a list of termSet, bitsets, each with terminalCount bits     *\n"
   "*       1-bits for legal terminals, 0-bits for illegal terminals.                    *\n"
   "*                                                                                    *\n"
   "*       b                  : Number of bytes in each termSet=(terminalCount-1)/8+1   *\n"
   "*       termSet[0..b-1]    : termSetTable[t..t+b-1]                                  *\n"
   "*                                                                                    *\n"
   "*       As for uncompressed states, the same check for existence is done.            *\n"
   "*       If terminal T is not present in termSet, set action = _ParseError.           *\n"
   "*       Note that each termSet may be shared by several states.                      *\n"
   "\\************************************************************************************/\n\n");

static const TCHAR *comment2 =
_T("/************************************************************************************\\\n"
   "* The 3 arrays NTindexListTable, stateListTable and successorCode holds a            *\n"
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
   "* successorCode. Each define has the format:                                         *\n"
   "*                                                                                    *\n"
   "* #define _suDDDD Code                                                               *\n"
   "*                                                                                    *\n"
   "* where DDDD is the statenumber S and Code is an unsigned int with the following     *\n"
   "* format:                                                                            *\n"
   "*            0         1         2         3                                         *\n"
   "* Bit index: 01234567890123456789012345678901                                        *\n"
   "* Code       iiiiiiiiiiiiiiiCCsssssssssssssss                                        *\n"
   "*                                                                                    *\n"
   "* i          : Bit[ 0-15]: unsigned short                                            *\n"
   "* s          : Bit[17-31]: unsigned short                                            *\n"
   "* CC         : Bit[15-16]: Indicates how to interpret i and s.                       *\n"
   "*                                                                                    *\n"
   "* CC has the same meaning as for actionCode, but only use CC={0,2}                   *\n"
   "* CC == 0: Uncompressed Format.                                                      *\n"
   "*       i: Index into array NTindexListTable, pointing at the first element of       *\n"
   "*          NTIndexList                                                               *\n"
   "*       s: Index into array stateListTable, pointing at the first element of         *\n"
   "*          stateList                                                                 *\n"
   "*                                                                                    *\n"
   "*       n                  : NTIndexListTable[i] = number of elements in NTIndexList *\n"
   "*       NTIndexList[0..n-1]: NTIndexListTable[i+1..i+n]                              *\n"
   "*                            Ordered list of possible nonterminal-indices.           *\n"
   "*       stateList[0..n-1]  : stateListTable[s..s+n-1], same length as NTIndexList    *\n"
   "*                                                                                    *\n"
   "*       To get newstate, find index k in NTIndexList, so NTIndexList[k] == A',       *\n"
   "*       and set newstate = stateList[k].                                             *\n"
   "*       A' = (A - terminalCount) will always exist.                                  *\n"
   "*       Note that both NTIndexList and stateList may be shared by several states.    *\n"
   "* CC == 2: Compressed Format, used if there is only 1 possible newstate.             *\n"
   "*       i: Index A' of nonterminal A, A' = (A - terminalCount).                      *\n"
   "*       s: New state.                                                                *\n"
   "*                                                                                    *\n"
   "\\************************************************************************************/\n\n");

static const TCHAR *comment3 =
_T("/************************************************************************************\\\n"
   "* The productionLength[] is indexed by production number and holds the number of     *\n"
   "* symbols on the right side of each production.                                      *\n"
   "\\************************************************************************************/\n");

static const TCHAR *comment4 =
_T("/************************************************************************************\\\n"
   "* leftSideTable[] is indexed by production number.                                   *\n"
   "* leftSideTable[p] = A', A' = (A - terminalCount)                                    *\n"
   "*                        where A is the left side of production p.                   *\n"
   "* A' = 0..nonterminalCount-1.                                                        *\n"
   "* p  = 0..productionCount-1                                                          *\n"
   "\\************************************************************************************/\n");

static const TCHAR *comment5 =
_T("/************************************************************************************\\\n"
   "* rightSideTable[] holds a compressed form of the rightsides of all                  *\n"
   "* productions in the grammar. Only used for debugging.                               *\n"
   "\\************************************************************************************/\n");

static const TCHAR *comment6 =
_T("/************************************************************************************\\\n"
   "* symbolNames is a space separated string with the names of all symbols used in      *\n"
   "* grammar, terminals and nonTerminals. Only used for debugging.                      *\n"
   "\\************************************************************************************/\n");

void GrammarTables::printCpp(MarginFile &output) const {
  m_countTableBytes.clear();

  if(!Options::getInstance().m_useTableCompression) {
    m_compressibleStateSet.clear();
  }
  findTemplateTypes();
  m_countTableBytes += printCompressedActionMatrixCpp(   output);
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
  output.printf(_T("actionCode      , termListTable     , actionListTable, termSetTable\n"
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
  m_terminalType     = findIntType(0,m_terminalCount  - 1);
  m_NTIndexType      = findIntType(0,ntCount          - 1);
  m_symbolType       = findIntType(0,getSymbolCount() - 1);
  m_stateType        = findIntType(0,getStateCount()  - 1);
  m_actionType       = ((getStateCount() < 128) && (getProductionCount() < 128))
                     ? TYPE_CHAR
                     : TYPE_SHORT;
}

// ---------------------------------------- Compressed Action Matrix ---------------------------------------------

// return size in bytes
ByteCount GrammarTables::printCompressedActionMatrixCpp(MarginFile &output) const {
  output.printf(_T("%s"), comment1);
  return ActionMatrix::CompressedActionMatrix(*this).print(output);
}

// ---------------------------------------- Successor Matrix ---------------------------------------------

class SuccessorArrayIndexMap : public IndexMap<SuccesorArray> {
public:
  SuccessorArrayIndexMap() : IndexMap<SuccesorArray>(successorArrayCmp) {
  }
};

typedef IndexArray<SuccesorArray> SuccesorArrayIndexArray;

ByteCount GrammarTables::printSuccessorMatrixCpp(MarginFile &output) const {
  output.printf(_T("%s"), comment2);
  const UINT             stateCount = getStateCount();
  StringArray            defines(stateCount);
  BitSet                 definedStateSet(stateCount);
  tostrstream            tmpOutput;
  MarginFile             output1(tmpOutput);
  SymbolSetIndexMap      ntSetMap;
  SuccessorArrayIndexMap saMap;
  UINT                   currentNTListSize = 0;
  UINT                   currentSAListSize = 0;
  ByteCount              byteCount;

  for(UINT state = 0; state < stateCount; state++) {
    const ActionArray &succList  = m_stateSucc[state];
    const UINT         succCount = (UINT)succList.size();
    if(succCount == 0) {
      continue;
    }
    definedStateSet.add(state);
    if(succCount == 1) {
      const ParserAction &pa         = succList[0];
      const UINT          NT         = pa.m_token;
      const UINT          newState   = pa.m_action;
      const UINT          NTIndex    = NT - m_terminalCount;
      const String        macroValue = encodeMacroValue(ParserTables::CompCodeOneItem, newState, NTIndex);
      const String        comment    = format(_T("Goto %u on %s"), newState, getSymbolName(NT));
      defines.add(format(_T("_su%04u %-10s /* %-*s*/"), state, macroValue.cstr(), commentWidth, comment.cstr()));
    } else {
      const SymbolSet     ntSet      = getNTOffsetSet(state);
      IndexMapValue      *imvp       = ntSetMap.get(ntSet);
      UINT                ntIndex, ntCount;

      if(imvp != nullptr) {
        ntIndex = imvp->m_arrayIndex;
        ntCount = imvp->m_commentIndex;
        imvp->addState(state);
      } else {
        ntIndex = currentNTListSize;
        ntCount = ntSetMap.getCount();
        IndexMapValue nv(stateCount, state, ntIndex);
        ntSetMap.put(ntSet, nv);
        currentNTListSize += (UINT)ntSet.size() + 1;
      }

      const SuccesorArray  sa   = getSuccessorArray(state);
      imvp                      = saMap.get(sa);
      UINT                 saIndex, saCount;
      if(imvp != nullptr) {
        saIndex = imvp->m_arrayIndex;
        saCount = imvp->m_commentIndex;
        imvp->addState(state);
      } else {
        saIndex = currentSAListSize;
        saCount = saMap.getCount();
        IndexMapValue nv(stateCount, state, saIndex);
        saMap.put(sa, nv);
        currentSAListSize += (UINT)sa.size();
      }
      const String macroValue = encodeMacroValue(ParserTables::CompCodeTermList, saIndex, ntIndex);
      const String comment    = format(_T("NTindexList %3u, stateList %3u"), ntCount, saCount);
      defines.add(format(_T("_su%04u %-10s /* %-*s*/"), state, macroValue.cstr(), commentWidth, comment.cstr()));
    }
  }

  if(currentNTListSize == 0) {
    output1.printf(_T("#define NTindexListTable nullptr\n"));
    output1.printf(_T("#define stateListTable   nullptr\n\n"));
  } else {
    { const SymbolSetIndexArray ntSetArray = ntSetMap.getEntryArray();
      ntSetMap.clear();

      outputBeginArrayDefinition(output1, _T("NTindexListTable"), m_NTIndexType, ntSetArray.getElementCount(true));
      UINT  tableSize = 0;
      TCHAR delim     = ' ';
      for(auto it = ntSetArray.getIterator(); it.hasNext();) {
        const IndexArrayEntry<SymbolSet> &e       = it.next();
        String                            comment = format(_T("%3u %s"), e.m_commentIndex, e.getComment().cstr());
        const UINT                        n       = (UINT)e.m_key.size();
        UINT                              counter = 0;
        output1.setLeftMargin(2);
        output1.printf(_T("%c%3u"), delim, n); delim = ',';
        output1.setLeftMargin(6);
        for(auto it1 = e.m_key.getIterator(); it1.hasNext(); counter++) {
          output1.printf(_T(",%4zu"), it1.next());
          if((counter % 20 == 19) && (counter != n - 1)) {
            newLine(output1, comment, 108);
          }
        }
        newLine(output1, comment, 108);
        tableSize += n + 1;
      }
      byteCount += outputEndArrayDefinition(output1, m_NTIndexType, tableSize);
    }

    { const SuccesorArrayIndexArray saArray  = saMap.getEntryArray();
      saMap.clear();

      outputBeginArrayDefinition(output1, _T("stateListTable"), m_stateType, saArray.getElementCount(false));
      UINT  tableSize = 0;
      TCHAR delim     = ' ';
      for(auto it = saArray.getIterator(); it.hasNext();) {
        const IndexArrayEntry<SuccesorArray> &e       = it.next();
        String                                comment = format(_T("%3u %s"), e.m_commentIndex, e.getComment().cstr());
        const UINT                            n       = (UINT)e.m_key.size();
        UINT                                  counter = 0;
        for(auto it1 = e.m_key.getIterator(); it1.hasNext(); counter++, delim=',') {
          output1.printf(_T("%c%4u"), delim, it1.next());
          if((counter % 20 == 19) && (counter != n - 1)) {
            newLine(output1, comment, 108);
          }
        }
        newLine(output1, comment, 108);
        tableSize += n;
      }
      byteCount += outputEndArrayDefinition(output1, m_stateType, tableSize);
    }
  }

  if(defines.size() > 0) {
    defines.sort(stringCmp);
    for(auto it = defines.getIterator(); it.hasNext();) {
      output.printf(_T("#define %s\n"), it.next().cstr());
    }
    output.printf(_T("\n"));
  }
  output.printf(_T("#define nil (unsigned int)-1\n"));
  outputBeginArrayDefinition(output, _T("successorCode"), TYPE_UINT, stateCount);

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
  byteCount += outputEndArrayDefinition(output, TYPE_UINT, stateCount, true);
  output1.close();
  output.puts(tmpOutput.str().c_str());
  return byteCount;
}

ByteCount GrammarTables::printProductionLengthTableCpp(MarginFile &output) const {
  output.printf(_T("%s"), comment3);

  const UINT productionCount = getProductionCount();

  outputBeginArrayDefinition(output, _T("productionLength"), TYPE_UCHAR, productionCount);
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
  return outputEndArrayDefinition(output, TYPE_UCHAR, productionCount, true);
}

ByteCount GrammarTables::printLeftSideTableCpp(MarginFile &output) const {
  output.printf(_T("%s"), comment4);

  const UINT productionCount = getProductionCount();
  outputBeginArrayDefinition(output, _T("leftSideTable"), m_NTIndexType, productionCount);

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
  return outputEndArrayDefinition(output, m_NTIndexType, productionCount, true);
}

ByteCount GrammarTables::printRightSideTableCpp(MarginFile &output) const {
  output.printf(_T("%s"), comment5);
  const UINT productionCount = getProductionCount();
  UINT       itemCount       = 0;

  for(UINT p = 0; p < productionCount; p++) {
    itemCount += (UINT)m_rightSide[p].size();
  }
  outputBeginArrayDefinition(output, _T("rightSideTable"), m_symbolType, itemCount);
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
  return outputEndArrayDefinition(output, m_symbolType, itemCount);
}

ByteCount GrammarTables::printSymbolNameTableCpp(MarginFile &output) const {
  output.printf(_T("%s"), comment6);

  const UINT terminalCount = getTerminalCount();
  const UINT symbolCount   = getSymbolCount();
  size_t     charCount     = 0;
  output.printf(_T("static const char *symbolNames = {\n"));
  output.setLeftMargin(2);
  for(UINT s = 0; s < symbolCount; s++) {
    const String &name = m_symbolNameArray[s];
    output.printf(s ? _T("\" ") : _T("\""));
    const int    l       = (int)name.length()+(s?1:0), fillerLen = minMax(50 - l, 0, 50);
    const String comment = (s < m_terminalCount)
                         ? format(_T("T  %4u"), s)
                         : format(_T("NT %4u NTindex=%u"), s, s-m_terminalCount);
    output.printf(_T("%s\"%*s/* %-21s */\n"), name.cstr(), fillerLen,_T(""),comment.cstr());
    charCount += l;
  }
  output.setLeftMargin(0);

  const ByteCount byteCount = ByteCount::wordAlignedSize(sizeof(char)*(UINT)(charCount+1));
  output.printf(_T("}; // Size of string:%s\n\n"), byteCount.toString().cstr());
  return byteCount;
}
